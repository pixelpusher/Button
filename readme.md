# Arduino Button Library

The Arduino Button library makes it easy to do some very common but rather 
tedious tasks, including detecting clicks, long presses, and debouncing.

## Example

	Button button(12);

	void onPress(Button& b){
		Serial.print("onPress: ");
		Serial.println(b.pin);
		// will print out "onPress: 12"
	}

	void setup(){
	  Serial.begin(9600);
	  // Assign callback function
	  button.pressHandler(onPress);
	}

	void loop(){
	  // update the buttons' internals
	  button.process();
	}

## Features

* Supports both pull-up and poll down buttons.

	`Button myButton(11, BUTTON_PULLUP_INTERNAL);`

* Simple yet powerful queries for press, held, and release. All
  the queries are debounce-filtered.

* Callback model. Probably the easiest way to use the Button functionality.

	`button.pressHandler(onPress)`

	There are 4 callbacks you can (optionally) use: press, release,
	click, and hold. 'press' and 'release' are sent for every button.
	'click' and 'hold' are exclusive. 

	- 'hold' will be sent for a long press, before 'release'
	- 'click' will be sent for a short press, after 'release'

* Built-in debouncing. The library has a reasonable default time, or
  you can set your own.

## Installing


