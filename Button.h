#ifndef BUTTON_LIBRARY_INCLUDED
#define BUTTON_LIBRARY_INCLUDED

#include <inttypes.h>

#define BUTTON_PULLUP           HIGH
#define BUTTON_PULLDOWN         LOW

class Button;
typedef void (*buttonEventHandler)(const Button&);

class Button {
  public:

    enum {
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
    Button(uint8_t buttonPin, uint8_t buttonMode = BUTTON_PULLUP, uint32_t debounceDuration = DEFAULT_BOUNCE_DURATION);
    
    int  pin() const {return myPin; }

    void process();

    /// true if pressed
    bool pressed() const;
    /// true if changed at last process() 
    bool stateChanged() const;
    /// true if the press was at last process()
    bool uniquePress() const;
    /// true if the button is held to the trigger time or longer
    bool held() const;
    /// return the time the button held down
    uint32_t holdTime() const;
    
    void setHoldThreshold(uint32_t holdTime);
    
    void pressHandler(buttonEventHandler handler);
    void releaseHandler(buttonEventHandler handler);
    void clickHandler(buttonEventHandler handler);
    void holdHandler(buttonEventHandler handler, uint32_t holdTime=DEFAULT_HOLD_TIME);
  
  private: 
    void pullup(uint8_t buttonMode);
    void pulldown();

    uint8_t             myPin;
    uint8_t             mode;
    uint8_t             state;
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

#endif // BUTTON_LIBRARY_INCLUDED
