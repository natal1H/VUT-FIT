# VUT FIT - IPK - Project No. 1 - variant 2
# Author: Natália Holková (xholko02)

import socket
import sys
import json

# Constants:
buffer_size = 4096
target_host = 'api.openweathermap.org'
target_port = 80  # create a socket object


def check_arguments():
    """Function to check if correct number of program arguments was entered."""
    return len(sys.argv) == 3


def check_help_arg():
    """Function to check if user entered argument for displaying help"""
    return len(sys.argv) == 2 and sys.argv[1] == "--help"


def display_help():
    """Function to display program help."""
    help_message = """Client for acquiring current weather information about desired city.
    
Usage:
    - call this program with your OpenWeatherMap API key and inquired city as arguments
    - call with single argument --help to display, well, what you are reading right now
In case of success, program will output weather info in following format:
    City, country
    weather description
    temperature
    humidity
    pressure
    wind
Common problems:
    Program will return error message if any of following occurres:
        - no connection to the Internet
        - wrong API key
        - wrong city name
        - wrong number of arguments or arguments in wrong order
        
Enjoy!"""
    print(help_message)


def process_arguments():
    """Function to return API key and city from program arguments. Assumes function check_arguments() returned true."""
    return sys.argv[1], sys.argv[2]


def calculate_speed(speed):
    """Function to convert speed from metres per second to kilometers per hour"""
    return speed * 3.6


def output_weather(data):
    """Function to output weather info from json data"""
    print("%s, %s" % (data["name"], data["sys"]["country"]))  # City, country

    weather_states = len(data["weather"])
    for index in range(weather_states-1):
        print("%s," % data["weather"][index]["description"], end=" ")
    print(data["weather"][weather_states-1]["description"])

    print("temp:\t\t%.1f \u00B0C" % data["main"]["temp"])  # Temperature in celsius

    print("humidity:\t%s%%" % data["main"]["humidity"])  # Air humidity
    print("pressure:\t%s hPa" % data["main"]["pressure"])  # Pressure
    print("wind-speed:\t%.1f km/h" % calculate_speed(data["wind"]["speed"]))  # Wind speed

    if "deg" in data["wind"].keys():
        print("wind-deg:\t%s" % data["wind"]["deg"])  # Wind degree
    else:
        print("wind-deg:\t-")  # Wind degree unknown


def get_data_from_api(api_key, city):
    """Function to get data from API using provided key."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
        # connect the client
        try:
            client.connect((target_host, target_port))
        except:
            # Error with connecting to server
            exit_with_message("Could not connect to OpenWeatherMap server. Exiting program...")

        # send some data
        data_path = '/data/2.5/weather?q=' + city + '&APPID=' + api_key + "&units=metric"
        request = "GET " + data_path + " HTTP/1.1\r\nHost: " + target_host + "\r\nConnection: close\r\n\r\n"
        client.sendall(request.encode())

        # receive some data
        data = client.recv(buffer_size)

    return data


def exit_with_message(message):
    """Function to display error message and exit program with error code 1."""
    print("Error: " + message)
    sys.exit(1)

# Main #


if check_arguments():
    api_key, city = process_arguments()
elif check_help_arg():
    display_help()
    exit(0)
else:
    exit_with_message("wrong arguments. Exiting program...")

# Get weather info from API
data = get_data_from_api(api_key, city)

# decode data to string
data_str = data.decode("utf-8")

# Checking http status - get first line, split, check response number
first_line = data_str[:data_str.index('\r\n')].split(" ", 2)

if first_line[1] != "200":
    # Error occurred - wrong HTTP response code
    error_code = first_line[1]
    error_msg = first_line[2]
    exit_with_message("HTTP response code %s - %s. Exiting program..." % (error_code, error_msg))
else:
    # Acquire json substring
    start = data_str.index('{')  # Find start of json data
    data_json = json.loads(data_str[start:])  # Decode string and get json data

    output_weather(data_json)
