#include <Arduino.h>
#include "Button.h"

#define BIT_CURRENT         0
#define BIT_PREVIOUS        1
#define BIT_CHANGED         2
#define BIT_HOLD_TRIGGERED  3
#define BIT_HOLD_NOW        4

//#define DEBUG_SERIAL

Button::Button(uint8_t buttonPin, uint8_t buttonMode, uint16_t _debounceDuration)
{
	myPin = buttonPin;
  mode = buttonMode;
  state = 0;
  holdEventThreshold = DEFAULT_HOLD_TIME;
  debounceDuration = _debounceDuration;
  debounceStartTime = 0;
  pressedStartTime = 0;
  handlers = 0;

  pinMode(myPin, INPUT);
  bitWrite(state, BIT_CURRENT, (digitalRead(myPin) == mode));

#ifdef DEBUG_SERIAL
  Serial.print("Button init:");
  Serial.println(buttonPin);
#endif
}

void Button::process(void)
{
  // save the previous value
  bitWrite(state, BIT_PREVIOUS, bitRead(state, BIT_CURRENT));
  
  // get the current status of the pin
  bitWrite(state, BIT_CURRENT, (digitalRead(myPin) == mode));

  // clear the hold, if it was set.
  bitWrite(state, BIT_HOLD_NOW, false);

  uint32_t currentMillis = millis();

  if (bitRead(state, BIT_CURRENT) != bitRead(state, BIT_PREVIOUS)) {
    uint32_t interval = currentMillis - debounceStartTime;

    if(interval < uint32_t(debounceDuration)) {
      // not enough time has passed; ignore
      return;
    }
    debounceStartTime = currentMillis;

    if (bitRead(state, BIT_CURRENT)) {
      // Pressed.
      #ifdef DEBUG_SERIAL
      Serial.println("Button press.");
      #endif

      if (handlers && handlers->cb_onPress) { 
        handlers->cb_onPress(*this); 
      }
      pressedStartTime = currentMillis;        //start timing
      bitWrite(state, BIT_HOLD_TRIGGERED, false);
      bitWrite(state, BIT_HOLD_NOW, false);
    } 
    else {
      // Released.
      #ifdef DEBUG_SERIAL
      Serial.println("Button release.");
      #endif

      if (handlers && handlers->cb_onRelease) { 
        handlers->cb_onRelease(*this); 
      }
      // Don't fire both hold and click.
      if (!bitRead(state, BIT_HOLD_TRIGGERED)) {
        #ifdef DEBUG_SERIAL
        Serial.println("Button click.");
        #endif
        if (handlers && handlers->cb_onClick) { 
          handlers->cb_onClick(*this);    //fire the onClick event AFTER the onRelease
        }
      }
      //reset states (for timing and for event triggering)
      pressedStartTime = 0;
    }
    bitWrite(state, BIT_CHANGED, true);
  }
  else {
    // State did NOT change.
    bitWrite(state, BIT_CHANGED, false);

    // should we trigger an onHold event? If so - only trigger one!
    if (isDown() && !bitRead(state, BIT_HOLD_TRIGGERED)) 
    {
      if (pressedStartTime && (currentMillis - pressedStartTime > uint32_t(holdEventThreshold))) 
      { 
        #ifdef DEBUG_SERIAL
        Serial.print("Button hold. startTime=");
        Serial.print(pressedStartTime);
        Serial.print(" currentTime=");
        Serial.println(currentMillis);
        #endif
        bitWrite(state, BIT_HOLD_TRIGGERED, true);
        bitWrite(state, BIT_HOLD_NOW, true);
        if (handlers && handlers->cb_onHold) {
          handlers->cb_onHold(*this);
        } 
      }
    }
  }
}


bool Button::isDown() const
{
	return bitRead(state, BIT_CURRENT);
}


bool Button::stateChanged() const
{
  return bitRead(state, BIT_CHANGED);
}


bool Button::press() const
{
  return (isDown() && stateChanged());
}


bool Button::held() const
{
  return isDown() && bitRead(state, BIT_HOLD_NOW); 
}


uint32_t Button::holdTime() const
{
  if (!isDown())
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
