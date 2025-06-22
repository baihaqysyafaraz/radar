from flask import Flask, request, render_template, jsonify
from datetime import datetime

app = Flask(__name__)

latest_data = {"angle": 0, "distance": 0, "type": "unknown"}
radar_status = {"on": True}
last_buzzer_data = {"time": "", "distance": 0}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/post_data', methods=['POST'])
def post_data():
    global latest_data, last_buzzer_data
    data = request.get_json()
    angle = data.get("angle", 0)
    distance = data.get("distance", 0)

    if distance < 10 and distance > 0:
        now = datetime.now().strftime("%H:%M:%S")
        last_buzzer_data = {
            "time": now,
            "distance": distance
        }

    # Klasifikasi jenis objek
    if distance < 10:
        obj_type = "dekat"
    elif distance < 25:
        obj_type = "sedang"
    elif distance <= 50:
        obj_type = "jauh"
    else:
        obj_type = "kosong"

    latest_data = {
        "angle": angle,
        "distance": distance,
        "type": obj_type
    }
    return jsonify({"status": "ok"})

@app.route('/get_data')
def get_data():
    return jsonify(latest_data)

@app.route('/get_buzzer_history')
def get_buzzer_history():
    return jsonify(last_buzzer_data)

@app.route('/set_radar', methods=['POST'])
def set_radar():
    data = request.get_json()
    radar_status["on"] = data.get("on", True)
    return jsonify({"status": "radar set", "on": radar_status["on"]})

@app.route('/get_radar')
def get_radar():
    return jsonify(radar_status)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
