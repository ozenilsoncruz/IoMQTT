from flask import Flask, render_template, jsonify, redirect, request
import json, base64
from random import choice
from datetime import datetime
import os, binascii
from flask_mqtt import Mqtt
from random import choice


app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def overview():
    #dados = dict(request.form)
    randlist = [i for i in range(0, 100)]
    dados = {'1': choice(randlist), '2': choice(randlist)}
    dados = json.dumps(dados)
    return render_template('overview.html', title='Overview', dados=dados)


if __name__ == "__main__":
    app.run(debug=True)