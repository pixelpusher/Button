/* $Id$
||
|| @file 		       Button.cpp
|| @author 		     Alexander Brevig              <alexanderbrevig@gmail.com>        
|| @url            http://alexanderbrevig.com
||
|| @description
|| | This is a Hardware Abstraction Library for Buttons
|| | It providea an easy way of handling buttons
|| #
||
|| @license LICENSE_REPLACE
||
*/

#ifndef Button_h
#define Button_h

#include <inttypes.h>

#define BUTTON_PULLUP           HIGH
#define BUTTON_PULLUP_INTERNAL  2
#define BUTTON_PULLDOWN         LOW

class Button;
typedef void (*buttonEventHandler)(const Button&);

class Button {
  public:

    enum {
        DEFAULT_HOLD_TIME = 500
    };    
  
    Button(uint8_t buttonPin, uint8_t buttonMode=BUTTON_PULLUP_INTERNAL, bool _debounceMode=true, int _debounceDuration=20);
    
    int  pin() const {return myPin; }
    void pullup(uint8_t buttonMode);
    void pulldown();
    void process();

    bool pressed() const;
    bool stateChanged() const;
    bool uniquePress() const;
    bool held() const;
    uint32_t holdTime() const;
    
    void setHoldThreshold(uint32_t holdTime);
    
    void pressHandler(buttonEventHandler handler);
    void releaseHandler(buttonEventHandler handler);
    void clickHandler(buttonEventHandler handler);
    void holdHandler(buttonEventHandler handler, uint32_t holdTime=DEFAULT_HOLD_TIME);
  
    inline uint16_t presses() const { return numberOfPresses; }
    
  private: 
    void init(uint8_t buttonPin, uint8_t buttonMode=BUTTON_PULLUP_INTERNAL, bool _debounceMode=true, int _debounceDuration=20);

    uint8_t             myPin;
    uint8_t             mode;
    uint8_t             state;
    bool                debounceMode;
    uint32_t            pressedStartTime;
    uint32_t            holdEventThreshold;
    uint32_t            debounceStartTime;
    uint32_t            debounceDuration;
    buttonEventHandler  cb_onPress;
    buttonEventHandler  cb_onRelease;
    buttonEventHandler  cb_onClick;
    buttonEventHandler  cb_onHold;
    uint16_t            numberOfPresses;
    bool                triggeredHoldEvent;
};

#endif