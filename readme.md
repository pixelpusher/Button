# Button Library

The Button library for Arduino makes common button tasks simple, filters
out mechanical noise, and adds some handy extra features. Solid, dependable
button handling is surprisingly difficult, so let this library help!

## Features

* De-bouncing. Mechanical buttons physically vibrate - bounce - when
  they are first pressed or released. This creates spurious state changes
  that need to be filtered or "de-bounced".
* Support for pull-up or pull-down configurations.
	
	Button button(12, Button::PULL_DOWN);

* Queries for press, down, and held. Queries are checked after the `process()`
  method is called.

	* 'press()' is true when a button is first pressed down.
	* 'isDown()' returns true if the button is down. (When a button is initially
	   pressed both 'press' and 'isDown' will return true. Subsequent queries
	   to a down button result in 'press' false and `isDown` true.)
	* 'held()' is true if the button has hit the holdThreshold in this loop.

* Callacks for press, hold, release, and click. Callbacks are called 
  from the `process()` method. Callbacks are generally easier for more
  complex button handling (doing something different on hold vs click, for
  example.)

	* 'press' is called when a button is first pressed.
	* 'click' is called when a button is released, if it wasn't held. 'click'
	  is called after 'release'.
	* 'hold' is called if a button is held down.
	* 'release' is called when the button is released.

## Example

### Basic Usage

	Button button(12);

	void setup() {
		Serial.begin(19200);
	}

	void loop() {
		button.process();
		if (button.uniquePress()) {
			Serial.println("Button pressed.");
		}
	}

### Callback Usage

	ButtonCB button(12);

	void onPress(const Button& b){
		Serial.println("Button pressed.");
	}

	void setup(){
	  Serial.begin(19200);
	  button.pressHandler(onPress);
	}

	void loop(){
	  button.process();
	}

## Installing

FIXME

