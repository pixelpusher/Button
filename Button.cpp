#include <Arduino.h>
#include "Button.h"

// bit positions in the state byte
#define CURRENT 0
#define PREVIOUS 1
#define CHANGED 2

//#define DEBUG_SERIAL

Button::Button(uint8_t buttonPin, uint8_t buttonMode, uint16_t _debounceDuration)
{
	myPin=buttonPin;
  pinMode(myPin,INPUT);
  handlers = 0;
  
  debounceDuration = _debounceDuration;
  debounceStartTime = millis();

	if (buttonMode == PULL_DOWN)
    pulldown();
  else
    pullup(buttonMode);

  state = 0;
  bitWrite(state,CURRENT,!mode);
  
  triggeredHoldEvent = true;
  holdEventThreshold = DEFAULT_HOLD_TIME;

#ifdef DEBUG_SERIAL
  Serial.print("Button init:");
  Serial.println(buttonPin);
#endif
}


void Button::pullup(uint8_t buttonMode)
{
	mode = PULL_UP;
}


void Button::pulldown(void)
{
	mode = PULL_DOWN;
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

    if(interval < uint32_t(debounceDuration)){
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

      if (handlers && handlers->cb_onPress) { 
        handlers->cb_onPress(*this); 
      }
      pressedStartTime = currentMillis;        //start timing
      triggeredHoldEvent = false;
    } 
    else //the state changed to RELEASED
    {
      #ifdef DEBUG_SERIAL
      Serial.println("Button release.");
      #endif

      if (handlers && handlers->cb_onRelease) { 
        handlers->cb_onRelease(*this); 
      }
      // Don't fire both hold and click.
      if (!triggeredHoldEvent) {
        #ifdef DEBUG_SERIAL
        Serial.println("Button click.");
        #endif
        if (handlers && handlers->cb_onClick) { 
          handlers->cb_onClick(*this);    //fire the onClick event AFTER the onRelease
        }
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
      if (currentMillis - pressedStartTime > uint32_t(holdEventThreshold)) 
      { 
        #ifdef DEBUG_SERIAL
        Serial.println("Button hold.");
        #endif
        if (handlers && handlers->cb_onHold) {
          handlers->cb_onHold(*this);
        } 
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


void ButtonCB::pressHandler(buttonEventHandler handler)
{
  handlerData.cb_onPress = handler;
}


void ButtonCB::releaseHandler(buttonEventHandler handler)
{
  handlerData.cb_onRelease = handler;
}


void ButtonCB::clickHandler(buttonEventHandler handler)
{
  handlerData.cb_onClick = handler;
}


void ButtonCB::holdHandler(buttonEventHandler handler)
{
  handlerData.cb_onHold = handler;
}
