#ifndef BUTTON_LIBRARY_INCLUDED
#define BUTTON_LIBRARY_INCLUDED

#include <Arduino.h>
#include <inttypes.h>

// - #defines inside class
// - fix up test case
// - fix up readme
// - license
// - post 

class Button;
typedef void (*buttonEventHandler)(const Button&);

struct ButtonCBHandlers {
    ButtonCBHandlers() : cb_onPress(0), cb_onRelease(0), cb_onClick(0), cb_onHold(0) {}
    buttonEventHandler  cb_onPress;
    buttonEventHandler  cb_onRelease;
    buttonEventHandler  cb_onClick;
    buttonEventHandler  cb_onHold;
};

/**
  * A simple button class that configures the Arduino, filters bounces, and
  * detects clicks and holds. Use `ButtoCB` if you would like to add callback handlers.
  */
class Button {
  public:

    enum {
        PULL_UP = HIGH,
        PULL_DOWN = LOW,
        DEFAULT_HOLD_TIME = 500,
        DEFAULT_BOUNCE_DURATION = 20
    };    
  
    /**
     *  Construct a button.
     *  'buttonPin' is the pin for this button to use.
     *  'buttonMode' is the wiring of the button. (Referring to the resistor.)
     *      - BUTTON_PULLDOWN is 'buttonPin' connected (through a resistor) to ground. When pressed, the button goes high.
     *      - BUTTON_PULLUP is 'buttonPin' connected (through a resistor) to high. When pressed, the button goes low.
     *  'debounceDuration' is how long it takes the button to settle, mechanically, when pressed.
     */
    Button(uint8_t buttonPin, uint8_t buttonMode = PULL_UP, uint16_t debounceDuration = DEFAULT_BOUNCE_DURATION);
    
    int  pin() const {return myPin; }

    /** Process the button state change. Should be called from loop().
     *  Will call the callbacks, if needed.
     */
    void process();

    /// true if pressed on this loop
    bool uniquePress() const;
    /// true if the the button is down
    bool pressed() const;
    /// true if changed at last process() 
    bool stateChanged() const;
    /// true if the button is held to the trigger time or longer
    bool held() const;
    /// return the time the button held down
    uint32_t holdTime() const;
    
    void setHoldThreshold(uint32_t holdTime);

  private: 
    void pullup(uint8_t buttonMode);
    void pulldown();

    bool                triggeredHoldEvent;
    uint8_t             myPin;
    uint8_t             mode;
    uint8_t             state;
    uint16_t            holdEventThreshold;
    uint16_t            debounceDuration;
    uint32_t            pressedStartTime;
    uint32_t            debounceStartTime;

protected:    
    const ButtonCBHandlers* handlers;
};


/**
 * Creates a button that supports Callbacks for Press, Release, Click, and Hold.
 * Uses a bit more memory (24 bytes) so the functionality is opt-in.
 */
class ButtonCB : public Button {
public:
    ButtonCB(uint8_t buttonPin, uint8_t buttonMode = PULL_UP, uint16_t debounceDuration = DEFAULT_BOUNCE_DURATION) :
        Button(buttonPin, buttonMode, debounceDuration)
    {
        handlers = &handlerData;   
    }

    /** Every button press generates 'press' and 'release'.
      * It may also generate a 'click' or a 'hold'.
      */
    void pressHandler(buttonEventHandler handler);
    void releaseHandler(buttonEventHandler handler);
    void clickHandler(buttonEventHandler handler);
    void holdHandler(buttonEventHandler handler);

private:
    ButtonCBHandlers handlerData;
};

#endif // BUTTON_LIBRARY_INCLUDED
