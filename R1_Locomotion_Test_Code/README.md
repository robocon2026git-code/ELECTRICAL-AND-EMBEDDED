This Folder Contains R1 Locomotion Test Code and Telemetry Code

Always run python code first
In py:
1)Run the python code "streampwm.py" in your pc through vscode or cmd

In ESP32 CODE:
2)Replace "pc_ip" with your pc's IP inside "stream_esp32_pwm.ino" in line no 30
	To find pc IP run "ipconfig" on cmd. Find "IPv4 Address"

3)After code uploaded in to ESP32, Reset the ESP32 once and after connected ESP32 with chassis

4)If compiler says "Compilation error: text section exceeds available space in board" then,
Tools -> Partition Scheme -> Huge APP (3MB No OTA/1MB SPIFFS).

NOTE:
5)Always start bot with rotation to ensure ESP32 Working correctly.
