import json, base64
import urllib.request
from random import choice
import time
import requests
from random import choice  


randlist = [i for i in range(0, 100)]

while 1:
    try:
        payload = {'chave': choice(randlist), 'chave2': choice(randlist)}
        requests.post('http://127.0.0.1:5000/', payload)
        time.sleep(2)
    except:
        print("Site Offiline")
        time.sleep(2)