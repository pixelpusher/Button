#include <Arduino.h>
#include "Button.h"

// bit positions in the state byte
#define CURRENT 0
#define PREVIOUS 1
#define CHANGED 2

//#define DEBUG_SERIAL

Button::Button(uint8_t buttonPin, uint8_t buttonMode, uint32_t _debounceDuration)
{
	myPin=buttonPin;
  pinMode(myPin,INPUT);
  
  debounceDuration = _debounceDuration;
  debounceStartTime = millis();

	if (buttonMode == BUTTON_PULLDOWN)
    pulldown();
  else
    pullup(buttonMode);

  state = 0;
  bitWrite(state,CURRENT,!mode);
  
  cb_onPress = 0;
  cb_onRelease = 0;
  cb_onClick = 0;
  cb_onHold = 0;
  
  numberOfPresses = 0;
  triggeredHoldEvent = true;
  holdEventThreshold = DEFAULT_HOLD_TIME;

#ifdef DEBUG_SERIAL
  Serial.print("Button init:");
  Serial.println(buttonPin);
#endif
}


void Button::pullup(uint8_t buttonMode)
{
	mode=BUTTON_PULLUP;
}


void Button::pulldown(void)
{
	mode=BUTTON_PULLDOWN;
}


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

    if(interval < debounceDuration){
      // not enough time has passed; ignore
      return;
    }
    debounceStartTime = currentMillis;

    //the state changed to PRESSED
    if (bitRead(state,CURRENT)) 
    {
      #ifdef DEBUG_SERIAL
      Serial.println("Button press.");
      #endif

      numberOfPresses++;
      if (cb_onPress) { cb_onPress(*this); }   //fire the onPress event
      pressedStartTime = currentMillis;        //start timing
      triggeredHoldEvent = false;
    } 
    else //the state changed to RELEASED
    {
      #ifdef DEBUG_SERIAL
      Serial.println("Button release.");
      #endif

      if (cb_onRelease) { cb_onRelease(*this); } //fire the onRelease event
      // Don't fire both hold and click.
      if (!triggeredHoldEvent) {
        #ifdef DEBUG_SERIAL
        Serial.println("Button click.");
        #endif
        if (cb_onClick) { cb_onClick(*this); }   //fire the onClick event AFTER the onRelease
      }
      //reset states (for timing and for event triggering)
      pressedStartTime = uint32_t(-1);
    }
    //note that the state changed
    bitWrite(state,CHANGED,true);
  }
  else
  {
    //note that the state did not change
    bitWrite(state,CHANGED,false);
    //should we trigger an onHold event?
    if (pressedStartTime != uint32_t(-1) && !triggeredHoldEvent && bitRead(state, CURRENT)) 
    {
      if (currentMillis - pressedStartTime > holdEventThreshold) 
      { 
        #ifdef DEBUG_SERIAL
        Serial.println("Button hold.");
        #endif
        if (cb_onHold) cb_onHold(*this);
        triggeredHoldEvent = true;
      }
    }
  }
}


bool Button::pressed() const
{
	return bitRead(state,CURRENT);
}


bool Button::stateChanged() const
{
  return bitRead(state,CHANGED);
}


bool Button::uniquePress() const
{
  return (pressed() && stateChanged());
}


bool Button::held() const
{
  return (pressedStartTime != uint32_t(-1)) && triggeredHoldEvent; 
}


uint32_t Button::holdTime() const
{
  if (!pressed())
    return 0;
  return millis() - pressedStartTime;
}


void Button::setHoldThreshold(uint32_t holdTime) 
{ 
  holdEventThreshold = holdTime; 
}


void Button::pressHandler(buttonEventHandler handler)
{
  cb_onPress = handler;
}


void Button::releaseHandler(buttonEventHandler handler)
{
  cb_onRelease = handler;
}


void Button::clickHandler(buttonEventHandler handler)
{
  cb_onClick = handler;
}


void Button::holdHandler(buttonEventHandler handler)
{
  cb_onHold = handler;
}
