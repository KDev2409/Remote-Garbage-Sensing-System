# Remote Garbage Sensing System

## Description

We implemented a remote garbage sensing system, with the aim to alert a user when a bin is full. The system is designed to be fitted to almost any garbage system, be it an office trash can or a dumpster in the street, it is portable only being the size of a palm, and can be battery-powered or hard-wired. The sensor system made use of two ultrasonic sensors in different locations, both posting the data through the use of an ESP32 development board to our created database hosted in the cloud. We made use of the Firebase cloud storage to host our data in a database, and Asksensors cloud analytics API to visualise the data retrieved from the database. We performed edge analytics and calculated the retrieved data’s average, mean, standard deviation and variance. The last task was to activate an actuator based on the data retrieved from the database, for this we implemented a green and red LED to display if the bin is full or empty.


## Getting Started


### Develop IOT System

Two garbage sensor devices were installed at two different locations which return two separate readings. To create the garbage sensor device we made use of an ultrasonic sensor module, HC-SR04 (Figure 1), an ESP32 development board (Figure 2), running Arduino code. We chose to use the ESP32 module, instead of the provided Arduino Uno, because of the Wi-Fi capabilities of the ESP32, not needing an extra Wi-Fi module, as we would have with the Uno, means the system can be smaller and more portable.

The sensor system detects the rubbish by using the HC-SR04 sensor which emits a sound wave, called the trigger signal, above human hearing frequencies, and then the module listens for the return (bounced back) sound, called the ‘echo’ signal.
This sensing system makes use of two garbage sensors in different countries, writing to a database hosted in the cloud. This data is then retrieved from the cloud in order to complete Task 3 data analysis and visualisation. Kain handled the sensor design and setup, Dev handled the cloud connectivity data management and handled the actuator activation.


### Useful commands

Download and setup Arduino UNO IDE and use the code provided in the project to get a working system.

## Authors

Devaesh Kaggdas
kdevaesh@gmail.com

Burgemeister Kain
100662073@student.swin.edu.au


