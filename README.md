# Client for OpenWeatherMap API

Student project for **IPK** at **VUT FIT**. Main goal is to create client capable of acquiring weather information using HTTP from OpenWeatherMap API. 
Program will display basic weather info: weather description, temperature, humidity, pressure and wind speed/degree.

## Solution design

Client application is written in Python programming language. 
Program first checks weather correct arguments were entered and then attempts to connect to OpenWeatherMap server to get weather data.
In case HTTP response is OK (code 200), program then decodes data in json format and outputs desired weather info.

## Prerequisites

In order to run the application, Python compiler is needed to be present. Version 3.6 or newer is required and can be downloaded at its [official website](https://www.python.org/downloads/). 

Furthermore, because this program uses OpenWeatherMap API to get weather info, user needs to already have his own account there and API key.
In case user has no yet his own account and API key, it can be acquired [here](https://home.openweathermap.org/users/sign_up).

Connection to the Internet is required.

## Installing and compiling application

There is no need to compile application before executing, due to the fact that program is written in Python.

## Running the application

Make sure both *Makefile* and *client.py* are present. Open terminal window in folder, where they stored and type following command:
```
make run api_key=<API_KEY> city=<city_name>
```
where `<API_KEY>` represents your personal API key and `<city_name>` name of city. 

In case city name contains only one word, program can be run in this manner:
```
make run api_key=1234 city=Brno
```

However, if city name contains more than one word, ""  are required. For example:
```
make run api_key=1234 city="Ivanka pri Nitre"
```

## Limitations and extensions

It is advised to limit number of calls to OpenWeatherMap API. Free account limits this to 60 calls per minute.

To display help on how to use program while in terminal window, type following:
```
make help
```
or without using Makefile, type directly:
```
python3.6 client.py --help
```

## Author

* **Natália Holková** (xholko02)