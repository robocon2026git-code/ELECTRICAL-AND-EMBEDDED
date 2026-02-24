import socket
import threading
import queue
import tkinter as tk
import signal
import sys
import struct

UDP_IP = "0.0.0.0"
UDP_PORT = 12345

FORMAT = "<ffff2Bff3B"
PACKET_SIZE = struct.calcsize(FORMAT)

bot_state = {
    "m1_pwm_g"          : 0,
    "m2_pwm_g"          : 0,
    "m3_pwm_g"          : 0,
    "m4_pwm_g"          : 0,
    "kfs_p2_g"          : 0,
    "kfs_p3_g"          : 0,
    "kfs_ToF_g"         : 0,
    "bot_speed_g"       : 0,
    "esp_rst_prv_g"     : 0,
    "esp_rst_curr_g"    : 0,
    "esp_boot_count_g"  : 0
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
                m1_pwm,
                m2_pwm,
                m3_pwm,
                m4_pwm,
                kfs_p2,
                kfs_p3,
                kfs_ToF,
                bot_speed,
                esp_rst_prv,
                esp_rst_curr,
                esp_boot_count
            ) = struct.unpack(FORMAT, data)

            bot_state["m1_pwm_g"]           = m1_pwm
            bot_state["m2_pwm_g"]           = m2_pwm
            bot_state["m3_pwm_g"]           = m3_pwm
            bot_state["m4_pwm_g"]           = m4_pwm
            bot_state["kfs_p2_g"]           = kfs_p2
            bot_state["kfs_p3_g"]           = kfs_p3
            bot_state["kfs_ToF_g"]          = kfs_ToF
            bot_state["bot_speed_g"]        = bot_speed
            bot_state["esp_rst_prv_g"]      = esp_rst_prv
            bot_state["esp_rst_curr_g"]     = esp_rst_curr
            bot_state["esp_boot_count_g"]   = esp_boot_count


            print(
                f"M1 PWM {m1_pwm: .2f} | "
                f"M2 PWM {m2_pwm: .2f} | "
                f"M3 PWM {m3_pwm: .2f} | "
                f"M4 PWM {m4_pwm: .2f} | "
            )

        except socket.timeout:
            continue
        except OSError:
            break   # socket closed → exit thread

    print("UDP thread exited")

def update_ui():
    lbl_speed.config(
        text=f"M1 PWM\n{bot_state['m1_pwm_g']: .2f}"
    )

    lbl_kfs_tof.config(
        text=f"M2 PWM\n{bot_state['m2_pwm_g']: .2f}"
    )

    lbl_staff_p1.config(
        text=f"M3 PWM\n{bot_state['m3_pwm_g']: .2f}"
    )

    lbl_null.config(
        text=f"M4 PWM\n{bot_state['m4_pwm_g']: .2f}"
    )

    lbl_update.config(
        text=f"UPDATE\nPrevious RST = {bot_state['esp_rst_prv_g']}\nCurrent RST = {bot_state['esp_rst_curr_g']}\nBoot Count = {bot_state['esp_boot_count_g']}"
    )
    if running:
        root.after(20, update_ui)

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

main = tk.Frame(root)
main.pack(expand=True, fill="both")

main.rowconfigure(0, weight=1)
main.rowconfigure(1, weight=0)
main.columnconfigure(0, weight=1)

container = tk.Frame(main)
container.grid(row=0, column=0, sticky="nsew")

update_container = tk.Frame(main, height=200)
update_container.grid(row=1, column=0, sticky="ew")
update_container.grid_propagate(False)

container.rowconfigure(0, weight=3)
container.rowconfigure(1, weight=3)
container.columnconfigure(0, weight=3)
container.columnconfigure(1, weight=3)

update_container.rowconfigure(0, weight=2)
update_container.columnconfigure(0, weight=2)

box_speed = tk.Frame(container, bg="red", bd=2)
box_kfs_distance = tk.Frame(container, bg="green", bd=2)
box_staff_p1 = tk.Frame(container, bg="blue", bd=2)
box= tk.Frame(container, bg="yellow", bd=2)

update_box = tk.Frame(update_container, bg="black", bd=2)


box_speed.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
box_kfs_distance.grid(row=0, column=1, sticky="nsew", padx=5, pady=5)
box_staff_p1.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
box.grid(row=1, column=1, sticky="nsew", padx=5, pady=5)
update_box.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)

lbl_speed = tk.Label(box_speed, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_speed.pack(expand=True)

lbl_kfs_tof = tk.Label(box_kfs_distance, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_kfs_tof.pack(expand=True)

lbl_staff_p1 = tk.Label(box_staff_p1, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_staff_p1.pack(expand=True)

lbl_null = tk.Label(box, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_null.pack(expand=True)

lbl_update = tk.Label(update_box, fg="white", bg="#2b2b2b", font=("Arial", 16), justify="center")
lbl_update.pack(expand=True)

threading.Thread(target=udp_listener, daemon=True).start()
root.after(20, update_ui)

root.mainloop()
