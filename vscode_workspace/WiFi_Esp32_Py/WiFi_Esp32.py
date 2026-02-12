import socket
import threading
import queue
import tkinter as tk
from tkinter import scrolledtext
import signal
import sys
import struct

UDP_IP = "0.0.0.0"
UDP_PORT = 12345

FORMAT = "<6Bff"
PACKET_SIZE = struct.calcsize(FORMAT)

bot_state = {
    "staff_p1_g": 0,
    "staff_p2_g": 0,
    "staff_p3_g": 0,
    "kfs_p1_g": 0,
    "kfs_p2_g": 0,
    "kfs_p3_g": 0,
    "kfs_ToF_g": 0,
    "bot_speed_g": 0
}

running = True
sock = None
msg_queue = queue.Queue()

def udp_listener():
    global sock, running

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((UDP_IP, UDP_PORT))
    sock.settimeout(0.5)

    print(f"Listening on UDP {UDP_PORT}")

    while running:
        try:
            data, addr = sock.recvfrom(1024)

            if len(data) != PACKET_SIZE:
                print("Invalid packet size:", len(data))
                continue
                # msg_queue.put(f"{addr[0]}:{addr[1]} -> {message}")
                # sock.sendto(b"PC CONNECTED!", addr)
            (
                staff_p1,
                staff_p2,
                staff_p3,
                kfs_p1,
                kfs_p2,
                kfs_p3,
                kfs_ToF,
                bot_speed
            ) = struct.unpack(FORMAT, data)

            bot_state["staff_p1_g"] = staff_p1
            bot_state["staff_p2_g"] = staff_p2
            bot_state["staff_p3_g"] = staff_p3
            bot_state["kfs_p1_g"] = kfs_p1
            bot_state["kfs_p2_g"] = kfs_p2
            bot_state["kfs_p3_g"] = kfs_p3
            bot_state["kfs_ToF_g"] = kfs_ToF
            bot_state["bot_speed_g"] = bot_speed

            print(
                f"Staff[{staff_p1},{staff_p2},{staff_p3}] | "
                f"KFS[{kfs_p1},{kfs_p2},{kfs_p3}] | "
                f"ToF={kfs_ToF:.2f} | Speed={bot_speed:.2f}"
            )

        except socket.timeout:
            continue
        except OSError:
            break   # socket closed → exit thread

    print("UDP thread exited")

def update_ui():
    lbl_speed.config(
        text=f"BOT SPEED\n{bot_state['bot_speed_g']}Km/h"
    )

    lbl_kfs_tof.config(
        text=f"KFS ToF DISTANCE\n{bot_state['kfs_ToF_g']}cm"
    )

    lbl_staff_p1.config(
        text=f"STAFF PIVOT1\n{bot_state['staff_p1_g']}"
    )

    lbl_null.config(
        text=f"NULL\n:)"
    )
    if running:
        root.after(100, update_ui)

def shutdown():
    global running, sock
    if not running:
        return

    print("Shutting down...")
    running = False

    if sock:
        sock.close()

    root.quit()
    sys.exit(0)

def on_window_close():
    shutdown()


# speed = 17
# dist = 100
# staff_p1 = "OFF"

# ---- SIGNAL HANDLER (Ctrl+C) ----
signal.signal(signal.SIGINT, lambda sig, frame: shutdown())

# ---- TKINTER UI ---- text=f"KFS ToF DISTANCE\n{bot_state['kfs_ToF_g']}cm"
root = tk.Tk()
root.title("ESP32 UDP Monitor")
root.geometry("1200x1000")

root.protocol("WM_DELETE_WINDOW", on_window_close)

label = tk.Label(root, text="WAR STATUS", font=("Arial", 14))
label.pack(pady=5)

# text_area = scrolledtext.ScrolledText(
#     root,
#     width=120,
#     height=50,
#     font=("Consolas", 11)
# )
# text_area.pack(padx=10, pady=10)

container = tk.Frame(root)
container.pack(expand=True, fill="both", padx=10, pady=10)

container.rowconfigure(0, weight=1)
container.rowconfigure(1, weight=1)
container.columnconfigure(0, weight=1)
container.columnconfigure(1, weight=1)

box_speed = tk.Frame(container, bg="red", bd=2)
box_kfs_distance = tk.Frame(container, bg="green", bd=2)
box_staff_p1 = tk.Frame(container, bg="blue", bd=2)
box= tk.Frame(container, bg="yellow", bd=2)

box_speed.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
box_kfs_distance.grid(row=0, column=1, sticky="nsew", padx=5, pady=5)
box_staff_p1.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
box.grid(row=1, column=1, sticky="nsew", padx=5, pady=5)

lbl_speed = tk.Label(box_speed, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_speed.pack(expand=True)

lbl_kfs_tof = tk.Label(box_kfs_distance, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_kfs_tof.pack(expand=True)

lbl_staff_p1 = tk.Label(box_staff_p1, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_staff_p1.pack(expand=True)

lbl_null = tk.Label(box, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_null.pack(expand=True)

threading.Thread(target=udp_listener, daemon=True).start()
root.after(100, update_ui)

root.mainloop()
