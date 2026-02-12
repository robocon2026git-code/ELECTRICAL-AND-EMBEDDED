import tkinter as tk

speed = 25
dist = 50
staff_p1 = "ON"


root = tk.Tk()
root.title("TKINTER")
root.geometry("700x700")

container = tk.Frame(root)
container.pack(expand=True, fill="both", padx=10, pady=10)

container.rowconfigure(0, weight=1)
container.rowconfigure(1, weight=1)
container.columnconfigure(0, weight=1)
container.columnconfigure(1, weight=1)

box1 = tk.Frame(container, bg="red", bd=2)
box2 = tk.Frame(container, bg="green", bd=2)
box3 = tk.Frame(container, bg="blue", bd=2)
box4 = tk.Frame(container, bg="orange", bd=2)

box1.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
box2.grid(row=0, column=1, sticky="nsew", padx=5, pady=5)
box3.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
box4.grid(row=1, column=1, sticky="nsew", padx=5, pady=5)

tk.Label(box1, text=f"BOT SPEED\n{speed}Km/h", fg="white", bg="#2b2b2b", font=("Arial, 16"), justify="center").pack(expand=True)
tk.Label(box2, text=f"KFS ToF DISTANCE\n{dist}cm", fg="white", bg="#2b2b2b", font=("Arial, 16"), justify="center").pack(expand=True)
tk.Label(box3, text=f"STAFF PIVOT1\n{staff_p1}", fg="white", bg="#2b2b2b", font=("Arial, 16"), justify="center").pack(expand=True)
tk.Label(box4, text="NULL\n:)", fg="white", bg="#2b2b2b", font=("Arial, 16"), justify="center").pack(expand=True)

root.mainloop()