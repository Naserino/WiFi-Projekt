from flask import Flask, render_template, request
import requests

app = Flask(__name__)

ESP32_IP = "http://172.16.217.27"  # Replace with your ESP32's IP address

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/led/<state>')
def led_control(state):
    if state == 'on':
        requests.get(f"{ESP32_IP}/led/on")
    elif state == 'off':
        requests.get(f"{ESP32_IP}/led/off")
    return f"LED is {state}"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)