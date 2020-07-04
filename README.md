#  ESP32 Weather Station

This project uses the weather station kit available from [Spark Fun](https://www.sparkfun.com/products/15901) connected to an ESP32 module from [Adafruit](https://www.adafruit.com/product/3269).
The goal is to connect this to the sensors, battery, and solar panel inside of a weather proof box and transmit over MQTT to a LAN server. 

Use esp-idf. Follow instructions here: https://github.com/espressif/esp-idf

## Build

Create a `credentials.h` file in the `main/` directory with the appropriate wifi credentials.

`idf.py build`

## Flash

`idf.py -p <port> flash`

Port should be the device path of the usb device. e.g. `/dev/ttyUSB0`

## Monitor

`idf.py -p <port> monitor`
