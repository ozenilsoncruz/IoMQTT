from flask import Flask, render_template,request, jsonify
from flask_mqtt import Mqtt
from flask_socketio import SocketIO, emit, send
import pandas as pd
from decouple import config


app = Flask(__name__)
io = SocketIO(app)
# mqtt_client = Mqtt(app)


@app.route("/")
def home():
    return render_template("index.html")

app.config['MQTT_BROKER_URL'] = 'test.mosquitto.org'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = ''  # Set this item when you need to verify username and password
app.config['MQTT_PASSWORD'] = ''  # Set this item when you need to verify username and password
app.config['MQTT_KEEPALIVE'] = 5  # Set KeepAlive time in seconds
app.config['MQTT_TLS_ENABLED'] = False  # If your server supports TLS, set it True

#topicos de publish
topic = 'INTERFACE/SBC'
topic_led_on = 'Interface/LED/on'
topic_led_off = 'Interface/LED/off'
topic_analogic = 'Interface/Sensor/Analogic'
topic_digital = 'Interface/Sensor/Digital'
topic_state = 'Interface/State/State'
topic_network = 'Interface/State/Network'

subtopic = "SBCINTERFACE"

# @mqtt_client.on_connect()
# def handle_connect(client, userdata, flags, rc):
#    if rc == 0:
#        print('Connected successfully')
#        mqtt_client.subscribe(subtopic) # subscribe topic
#    else:
#        print('Bad connection. Code:', rc)

# #recebeu a mensagem
# @mqtt_client.on_message()
# def handle_mqtt_message(client, userdata, message):
#    data = dict(
#        topic=message.topic,
#        payload=message.payload.decode()
#   )
#    print('Received message on topic: {topic} with payload: {payload}'.format(**data))   


# @app.route('/publish', methods=['POST'])
# def publish_message():
#    request_data = request.get_json()
#    publish_result = mqtt_client.publish(request_data['topic'], request_data['msg'])
#    return jsonify({'code': publish_result[0]})

if __name__ == "__main__":
    io.run(app, debug=True)

