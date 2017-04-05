# Hardware

At this folder you'll find all the hardware parts, what to buy and how to connect things to build a tapFi.

## Case

[Download Here](https://github.com/joaopedrovbs/tapFi/tree/master/hardware/stl-files) the files to 3D Print a case.

## Parts 

#### The list for all the parts used in the prototype and their links can be found [here](https://docs.google.com/spreadsheets/d/1pxn552UrJLPFjxMJo8Y3OLqV407wsjz8_O9QdrBkuIY/edit?usp=sharing).

This Google Spreadsheet is for building the project from Ground Up, the tools and shipping costs to Louxembourg are also included. The parts from TagConnect are the tools that we had to use to flash the ARM microcontroller as it had a proprietary connector. 

## Fitting inside the case

![Inside Case 3d](Fit link here after commit)

![Inside Real Case](Fit link here after commit)

Every component was hot glued to the case for better use while prototyping. 

## Connections

The connections are laid out according to the following pattern:
- `(MPU9250 Board) **SDA** == **PIN 21** (Beacon Board)`
- `(MPU9250 Board) **SCL** == **PIN 24** (Beacon Board)`
- `(MPU9250 Board) **VDD** == **VCC** (Beacon Board)`
- `(MPU9250 Board) **GND** == **GND** (Beacon Board)`
- `(Rumble Motor) **RED WIRE** == **PIN 20** (Beacon Board)`
- `(Rumble Motor) **BLUE WIRE** == **GND** (MPU 9250 Board)`

## Battery Situation

As the **LOW POWER** functions from the microcontroller are not implemented the CR1632 batteries usually power the system for arround `3 Hours` so it's good to remove the battery every time you finish testing.
