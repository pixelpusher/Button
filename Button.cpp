/* $Id$
||
|| @file 		       Button.cpp
|| @author 		     Alexander Brevig              <alexanderbrevig@gmail.com>        
|| @url            http://alexanderbrevig.com
||
|| @description
|| | This is a Hardware Abstraction Library for Buttons
|| | It provides an easy way of handling buttons
|| #
||
|| @license LICENSE_REPLACE
||
*/

#include <Arduino.h>
#include "Button.h"

// bit positions in the state byte
#define CURRENT 0
#define PREVIOUS 1
#define CHANGED 2

/*
|| @constructor
|| | Set the initial state of this button
|| #
|| 
|| @parameter buttonPin  sets the pin that this switch is connected to
|| @parameter buttonMode indicates BUTTON_PULLUP or BUTTON_PULLDOWN resistor
*/
Button::Button(uint8_t buttonPin, uint8_t buttonMode, bool _debounceMode, int _debounceDuration){
	myPin=buttonPin;
  pinMode(myPin,INPUT);
  
  debounceMode = _debounceMode;
  debounceDuration = _debounceDuration;
  debounceStartTime = millis();

	buttonMode==BUTTON_PULLDOWN ? pulldown() : pullup(buttonMode);
  state = 0;
  bitWrite(state,CURRENT,!mode);
  
  cb_onPress = 0;
  cb_onRelease = 0;
  cb_onClick = 0;
  cb_onHold = 0;
  
  numberOfPresses = 0;
  triggeredHoldEvent = true;
  holdEventThreshold = 0;
}

/*
|| @description
|| | Prepare logic for a pullup button.
|| | If mode is internal set pin HIGH as default
|| #
*/
void Button::pullup(uint8_t buttonMode)
{
	mode=BUTTON_PULLUP;
  if (buttonMode == BUTTON_PULLUP_INTERNAL) 
  {
	  digitalWrite(myPin,HIGH);
  }
}

/*
|| @description
|| | Set pin LOW as default
|| #
*/
void Button::pulldown(void)
{
	mode=BUTTON_PULLDOWN;
}

/*
|| @description
|| | Read and write states; issue callbacks
|| #
|| 
|| @return true if button is pressed
*/
void Button::process(void)
{
  //save the previous value
  bitWrite(state, PREVIOUS, bitRead(state, CURRENT));
  
  //get the current status of the pin
  bitWrite(state, CURRENT, (digitalRead(myPin) == mode));

  uint32_t currentMillis = millis();

  //handle state changes
  if (bitRead(state,CURRENT) != bitRead(state,PREVIOUS))
  {
    uint32_t interval = currentMillis - debounceStartTime;

    if(debounceMode && interval < debounceDuration){
      // not enough time has passed; ignore
      return;
    }
    debounceStartTime = currentMillis;

    //the state changed to PRESSED
    if (bitRead(state,CURRENT)) 
    {
      numberOfPresses++;
      if (cb_onPress) { cb_onPress(*this); }   //fire the onPress event
      pressedStartTime = currentMillis;        //start timing
      triggeredHoldEvent = false;
    } 
    else //the state changed to RELEASED
    {
      if (cb_onRelease) { cb_onRelease(*this); } //fire the onRelease event
      // Don't fire both hold and click.
      if (!triggeredHoldEvent) {
        if (cb_onClick) { cb_onClick(*this); }   //fire the onClick event AFTER the onRelease
      }
      //reset states (for timing and for event triggering)
      pressedStartTime = -1;
    }
    //note that the state changed
    bitWrite(state,CHANGED,true);
  }
  else
  {
    //note that the state did not change
    bitWrite(state,CHANGED,false);
    //should we trigger an onHold event?
    if (pressedStartTime != -1 && !triggeredHoldEvent && bitRead(state, CURRENT)) 
    {
      if (currentMillis - pressedStartTime > holdEventThreshold) 
      { 
        if (cb_onHold) cb_onHold(*this);
        triggeredHoldEvent = true;
      }
    }
  }
}

/*
|| @description
|| | Return the bitRead(state,CURRENT) of the switch
|| #
|| 
|| @return true if button is pressed
*/
bool Button::isPressed(bool proc)
{
  if(proc) process();
	return bitRead(state,CURRENT);
}

/*
|| @description
|| | Return true if the button has been pressed
|| #
*/
bool Button::wasPressed(bool proc)
{
  if(proc) process();
  return bitRead(state,CURRENT);
}

/*
|| @description
|| | Return true if state has been changed
|| #
*/
bool Button::stateChanged(bool proc)
{
  if(proc) process();
  return bitRead(state,CHANGED);
}

/*
|| @description
|| | Return true if the button is pressed, and was not pressed before
|| #
*/
bool Button::uniquePress()
{
  process();
  return (isPressed(false) && stateChanged(false));
}

/*
|| @description
|| | onHold polling model
|| | Check to see if the button has been pressed for time ms
|| | This will clear the counter for next iteration and thus return true once
|| #
*/
bool Button::held(uint32_t time /*=0*/) 
{
  process();
  unsigned int threshold = time ? time : holdEventThreshold; //use holdEventThreshold if time == 0
	//should we trigger a onHold event?
  if (pressedStartTime!=-1 && !triggeredHoldEvent) 
  {
    if (millis()-pressedStartTime > threshold) 
    { 
      triggeredHoldEvent = true;
      return true;
    }
  }
	return false;
}

/*
|| @description
|| | Polling model for holding, this is true every check after hold time
|| | Check to see if the button has been pressed for time ms
|| #
*/
bool Button::heldFor(uint32_t time) 
{
	if (isPressed()) 
  {
		if (millis()-pressedStartTime > time) { return true; }
	}
	return false;
}

/*
|| @description
|| | Set the hold event time threshold
|| #
*/
void Button::setHoldThreshold(uint32_t holdTime) 
{ 
  holdEventThreshold = holdTime; 
}

/*
|| @description
|| | Register a handler for presses on this button
|| #
||
|| @parameter handler The function to call when this button is pressed
*/
void Button::pressHandler(buttonEventHandler handler)
{
  cb_onPress = handler;
}

/*
|| @description
|| | Register a handler for releases on this button
|| #
||
|| @parameter handler The function to call when this button is released
*/
void Button::releaseHandler(buttonEventHandler handler)
{
  cb_onRelease = handler;
}

/*
|| @description
|| | Register a handler for clicks on this button
|| #
||
|| @parameter handler The function to call when this button is clicked
*/
void Button::clickHandler(buttonEventHandler handler)
{
  cb_onClick = handler;
}

/*
|| @description
|| | Register a handler for when this button is held
|| #
||
|| @parameter handler The function to call when this button is held
*/
void Button::holdHandler(buttonEventHandler handler, uint32_t holdTime)
{
  if (holdTime>0) { setHoldThreshold(holdTime); }
  cb_onHold = handler;
}

/*
|| @description
|| | Get the time this button has been held
|| #
||
|| @return The time this button has been held
*/
uint32_t Button::holdTime() const { 
  if (pressedStartTime!=-1) { 
    return millis()-pressedStartTime;
  } 
  else {
    return 0; 
  }
}
  