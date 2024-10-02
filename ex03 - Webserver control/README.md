# Webserver control

## Introduction

A web server is a software application responsible for hosting and delivering
content over the internet. It receives HTTP requests from clients, such as web
browsers, and responds with the requested content, such as HTML pages,
images, CSS, and JavaScript files. Web servers play a crucial role in enabling
users worldwide to access websites and web services, facilitating the exchange
of information between the server and clients.

## Hardware components

The following components are required to complete the project:

- ESP8266 Wi-Fi microcontroller
- W104 Sound sensor
- A breadboard
- 8x8 Ws2812b LED Matrix

## Project Objective

### Mandatory Part

Create a website on the ESP that will control the LED matrix.
Make buttons (one for each LED) to turn the LEDs on and off and then create an
animated pattern of your choice.

### Bonus Part

Using the sound sensor make the LEDs light up with diffrent colors depending
on the brightness of the environmental light.

## Circuit Diagram

- W104 Sound Sensor AO → A0 (ESP8266): For analog signal.
- W104 Sound Sensor DO → D1 (or any digital pin on ESP8266): For digital signal.
- W104 Sound Sensor VCC → 3.3V (ESP8266).
- W104 Sound Sensor GND → GND (ESP8266).
- ESP8266 Microcontroller V- → GND (8x8 LED MATRIX)
- ESP8266 Microcontroller 1N → D2 (8x8 LED MATRIX)
- ESP8266 Microcontroller V+ → 3v3 (8x8 LED MATRIX)

## Software and Code

The code for mandatory part and bonus part are at the same code.
You can find the code by clicking on the relevant link below:

- [Mandatory part + Bonus Part Code](webserver.ino)
