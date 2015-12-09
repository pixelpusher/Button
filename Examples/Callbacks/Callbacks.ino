/** 
 *  Demonstration of how to use the button in callback mode.
 */

#include <Button.h>

#define PIN 8
#define PULL Button::PULL_UP

ButtonCB button(PIN, PULL);

void doPressHandler(const Button& button) {
  Serial.print("LOG: doPressHandler ");
  Serial.println(button.pin());
}

void doReleaseHandler(const Button& button) {
  Serial.print("LOG: doReleaseHandler ");
  Serial.println(button.pin());
}

void doClickHandler(const Button& button) {
  Serial.print("LOG: doClickHandler ");
  Serial.println(button.pin());
}

void doHoldHandler(const Button& button) {
  Serial.print("LOG: doHoldHandler ");
  Serial.println(button.pin());
}

void setup() {
  Serial.begin(19200);
  Serial.println("Button Callback Demo");
  pinMode(13, OUTPUT);

  button.pressHandler(doPressHandler);
  button.releaseHandler(doReleaseHandler);
  button.clickHandler(doClickHandler);
  button.holdHandler(doHoldHandler);
}

void loop() {
  button.process();
  digitalWrite(13, button.isDown());
}
 
