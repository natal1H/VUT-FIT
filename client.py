#!/usr/bin/env python3

# VUT FIT - IPK - Projekt č. 1 - varianta 2
# Vypracovala: Natália Holková (xholko02)

# Iba základná funkčnosť bez akýchkoľvek ošetrení

import socket
import sys
import json

api_key = 'ec0149a4edef7e80607a2ea39e5c58f6' # TODO - prerobiť na získanie z argumentu pri volaní programu
city = 'Ivanka pri Nitre' # TODO - prerobiť na získanie z argumentu pri volaní programu


buffer_size = 4096

target_host = 'api.openweathermap.org'

target_port = 80  # create a socket object


#client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    # connect the client
    client.connect((target_host, target_port))

    # send some data
    data_path = '/data/2.5/weather?q=' + city + '&APPID=' + api_key
    request = "GET " + data_path + " HTTP/1.1\r\nHost: " + target_host + "\r\nConnection: close\r\n\r\n"
    client.sendall(request.encode())

    # receive some data
    data = client.recv(buffer_size)

    data_str = data.decode("utf-8")
    #print("DATA_STR:")
    #print(data_str)
    #print('- ' * 20)

    # Získanie substring s json
    start = data_str.index('{')
    #print(data_str[start:])
    #print('- ' * 20)

    data_json = json.loads(data_str[start:])

    #s = json.dumps(data_json, indent=4, sort_keys=True)
    #print(s)

    # Výpis počasia
    print(city) # Mesto TODO - vypísať reťazec vrátený jsonom
    # TODO: dať pozor, ak je viacero stavov počasí (prší a vietor zároveň - dorobiť - prejsť cez všetky a vypísať
    print(data_json["weather"][0]["description"]) # Popis počasia
    print("temp: %s K" % data_json["main"]["temp"]) # Teplota TODO - previesť predtým z K na Celsius
    print("humidity: %s%%" % data_json["main"]["humidity"]) # Vlhkosť
    print("pressure: %s hPa" % data_json["main"]["pressure"]) # Tlak
    print("wind-speed: %s km/h" % data_json["wind"]["speed"]) # Rýchlosť vetra TODO - zistiť v akých jednotkách vrátená
    print("wind-deg: %s" % data_json["wind"]["deg"]) # Smer vetra TODO - ošetriť ak nevráti smer vetru
