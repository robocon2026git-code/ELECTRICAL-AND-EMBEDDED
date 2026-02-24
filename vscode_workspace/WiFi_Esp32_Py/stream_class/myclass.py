import tkinter as tk

class MyContainer(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.build_ui()

    def build_ui(self):
        lbl = tk.Label(self, text="Hello")
        lbl.pack(expand=True)

        