from flask import Flask, jsonify, render_template, request
from flask_mqtt import Mqtt
import json
import time


app = Flask(__name__)
#app.config['MQTT_BROKER_URL'] = "10.0.0.101"
app.config['MQTT_BROKER_URL'] = "broker.emqx.io"
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'aluno'
app.config['MQTT_PASSWORD'] = '@luno*123'
app.config['MQTT_REFRESH_TIME'] = 1.0  # refresh time in seconds
mqtt = Mqtt(app)

SBC_IHM = "sbc/ihm"
IHM_TIME = "ihm/tempo"
data = {
    'a': {},
    'd': {}
}

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe(SBC_IHM)

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    global data
    if 'D' in topic: #verifica todos os topicos dos sensores digitais
        for i in range(10):
            if str(i+1) in topic:
                data['d'][str(i+1)] = message.payload.decode()
    if 'A' == topic:
        data['a'] = {
            '1': message.payload.decode()
        }

@app.route('/')
def index():
    return render_template('overview.html', title='Overview', dados=json.dumps(data))

if __name__ == '__main__':
    app.run()