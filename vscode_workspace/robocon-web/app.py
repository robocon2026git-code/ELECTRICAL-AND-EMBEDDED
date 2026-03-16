# app.py
from flask import Flask, render_template, Response
import cv2
import numpy as np
import math
from ultralytics import YOLO
from flask_ngrok import run_with_ngrok

app = Flask(__name__)
# run_with_ngrok(app)

# Constants
g = 9.81
hoop_height = 2.43
focal_length = 680
actual_hoop_width = 0.45
camera_height = 1.2
FIXED_LAUNCH_ANGLE = 45

model_path = "fBlock.pt"
model = YOLO(model_path)
cap = cv2.VideoCapture(1)

def estimate_distance(hoop_width_px):
    return (actual_hoop_width * focal_length) / hoop_width_px if hoop_width_px > 0 else None

def calculate_shot_parameters(distance_x, height_diff, offset):
    if distance_x <= 0:
        return None, None, None
    yaw_angle = math.degrees(math.atan(offset / distance_x))
    try:
        angle_rad = math.radians(FIXED_LAUNCH_ANGLE)
        term1 = distance_x / math.cos(angle_rad)
        term2 = (height_diff - distance_x * math.tan(angle_rad))
        if term2 <= 0:
            return yaw_angle, FIXED_LAUNCH_ANGLE, None
        initial_velocity = math.sqrt((g * term1**2) / (2 * term2))
        return yaw_angle, FIXED_LAUNCH_ANGLE, initial_velocity
    except:
        return None, None, None

def detect_hoop_yolo(frame):
    results = model(frame, verbose=False)
    for result in results:
        boxes = result.boxes
        for box in boxes:
            if box.cls == 0:
                x1, y1, x2, y2 = box.xyxy[0]
                return int(x1), int(y1), int(x2 - x1), int(y2 - y1), float(box.conf[0])
    return None, None, None, None, None

def gen_frames():
    while True:
        success, frame = cap.read()
        if not success:
            break
        x, y, w, h, confidence = detect_hoop_yolo(frame)
        if w:
            hoop_center_x = x + w // 2
            distance_x = estimate_distance(w)
            frame_center_x = frame.shape[1] // 2
            offset = (hoop_center_x - frame_center_x) * (distance_x / focal_length)
            yaw, angle, velocity = calculate_shot_parameters(distance_x, hoop_height - camera_height, offset)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(frame, f"Dist: {distance_x:.2f}m | Offset: {offset:.2f}m", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
            cv2.putText(frame, f"Angle: {FIXED_LAUNCH_ANGLE} | Vel: {velocity:.2f}m/s" if velocity else "Velocity: --", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
            cv2.putText(frame, f"Yaw: {yaw:.1f}°" if yaw else "Yaw: --", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5003, debug=True)

