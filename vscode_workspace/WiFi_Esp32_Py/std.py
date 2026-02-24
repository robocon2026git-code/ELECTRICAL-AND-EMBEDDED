import sys
from PyQt6.QtWidgets import *

app = QApplication(sys.argv)

window = QWidget()
window.setWindowTitle("SYSTEM STATUS")
window.resize(400, 300)

# layout = QVBoxLayout()
layout1 = QHBoxLayout()

# layout.addWidget(QPushButton("Start"))
# layout.addWidget(QPushButton("Stop"))
# layout.addWidget(QPushButton("Reset"))

# window.setLayout(layout)

window.show()
app.exec()