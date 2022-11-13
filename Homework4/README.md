# Controlling a 4 digit 7-segments display using a joystick #

## Components 

* a 4 digit 7-segments display
* 1 joystick
* 1 74hc595 shift register
* wires
* breadboard

## Task

Use the joystick to move through the 4 digit 7-segments displays digits, press the button to lock in on the current digit and use the other axis to increment or decrement the number. Keep the button pressed to reset all the digit values and the current position to the first digit in the first state. The system has the following states:

* State 1:  You can use a joystick axis to cycle through the 4 digits; using the other axis does nothing. A blinking decimal point shows the current digit position. When pressing the button, you lock in on the selected digit and enter the second state.

* State 2: In this state, the decimal point stays always on, no longer blinking and you can no longer use the axis to cycle through the 4 digits. Instead, using the other axis, you can increment or decrement the number on the current digit IN HEX (aka from 0 to F). Pressing the button again returns you to the previous state. Also, keep in mind that when changing the number, you must increment it for each joystick movement - it should not work continuosly increment if you keep the joystick in one position(aka with joyMoved). 

* Reset: Toggled by long pressing the button only in the first state. When resetting, all the digits go back to 0 and the current position is set to the first (rightmost) digit, in the first state.

## Setup Pictures
![alt text](https://github.com/ralucsandu/IntroductionToRobotics/blob/main/Homework4/setup-picture.jpeg?raw=true)

## Video Demo
TODO
