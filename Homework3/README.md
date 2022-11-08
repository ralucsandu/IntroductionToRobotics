# Controlling a 7-segments display using a joystick #

## Components 

* 1 7-segments display
* 1 joystick
* wires
* resistors
* breadboard

## Task

You will use the joystick to control the position of the segment and "draw" on the display. The movement between segments should be natural (meaning they should jump from the current position
only to neighbors, but without passing through "walls". The system has the following states:

* State 1 (default, but also initiated after a button press in State 2): Current position blinking. Can use the joystick to move from one position to neighbors. Short pressing the button toggles state
2. Long pressing the button in state 1 resets the entire display by turning all the segments OFF and moving the current position to the decimal point.

* State 2 (initiated after a button press in State 1): The current segment stops blinking, adopting the state of the segment before selection (ON or OFF). Toggling the X (or Y, you chose) axis should
change the segment state from ON to OFF or from OFF to ON. Clicking the joystick should save the segment state and exit back to state 1.

## Setup Pictures
![alt text](https://github.com/ralucsandu/IntroductionToRobotics/blob/main/Homework3/setup-picture.jpeg?raw=true)
![alt text](https://github.com/ralucsandu/IntroductionToRobotics/blob/main/Homework3/setup-picture-2.jpeg?raw=true)

## Video Demo
TODO
