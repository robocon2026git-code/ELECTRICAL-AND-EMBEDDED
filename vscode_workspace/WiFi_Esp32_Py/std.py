import tkinter as tk
from stream_class.myclass import MyContainer

root = tk.Tk()
root.title("BOT STATUS MONITOR")
root.geometry("1200x1000")

label = tk.Label(root, text="WAR STATUS", font=("Arial", 14))
label.pack(pady=5)

rt_container = MyContainer(root)
rt_container.pack(expand = True, fill = "both", padx=10, pady=1)

root.mainloop()