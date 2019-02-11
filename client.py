#!/usr/bin/env python3

# VUT FIT - IPK - Project No. 1 - variant 2
# Vypracovala: Natália Holková (xholko02)

# Iba základná funkčnosť bez akýchkoľvek ošetrení

# TODO - zjednotiť jazyk komentárov - celé po anglicky!!!
# TODO - kontrola kódu http response

import socket
import sys
import json

def check_arguments():
    return len(sys.argv) == 3

def process_arguments():
    return sys.argv[1], sys.argv[2]

def calculate_speed(speed):
    return speed * 3.6

def output_weather(data):
    print("%s, %s" % (data["name"], data["sys"]["country"]))  # Mesto, skratka krajiny

    weather_states = len(data["weather"])
    for index in range(weather_states-1):
        print("%s," % data["weather"][index]["description"], end=" ")
    print(data["weather"][weather_states-1]["description"])

    print("temp:\t\t%.1f \u00B0C" % data["main"]["temp"])  # Temperatu in celsius

    print("humidity:\t%s%%" % data["main"]["humidity"])  # Vlhkosť
    print("pressure:\t%s hPa" % data["main"]["pressure"])  # Tlak
    print(
        "wind-speed:\t%.1f km/h" % calculate_speed(data["wind"]["speed"]))  # Rýchlosť vetra

    if "deg" in data["wind"].keys():
        print("wind-deg:\t%s" % data["wind"]["deg"])  # Smer vetra
    else:
        print("wind-deg:\t-")  # Nie je známy smer vetra

### Hlavná časť programu ###

#api_key = 'ec0149a4edef7e80607a2ea39e5c58f6'
#city = 'Brno'

if check_arguments():
    api_key, city = process_arguments()
else:
    exit(1)

buffer_size = 4096

target_host = 'api.openweathermap.org'

target_port = 80  # create a socket object

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    # connect the client
    client.connect((target_host, target_port))

    # send some data
    data_path = '/data/2.5/weather?q=' + city + '&APPID=' + api_key + "&units=metric"
    request = "GET " + data_path + " HTTP/1.1\r\nHost: " + target_host + "\r\nConnection: close\r\n\r\n"
    client.sendall(request.encode())

    # receive some data
    data = client.recv(buffer_size)

# decode data to string
data_str = data.decode("utf-8")

# Získanie substring s json
start = data_str.index('{')
data_json = json.loads(data_str[start:])

#s = json.dumps(data_json, indent=4, sort_keys=True)
#print(s)

output_weather(data_json)
