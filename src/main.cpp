#include <Arduino.h>
#include <GameControllers.h>
#include <BleGamepad.h> // https://github.com/lemmingDev/ESP32-BLE-Gamepad

#define DPAD_BITMAP_FLAG_UP (1 << 0)
#define DPAD_BITMAP_FLAG_RIGHT (1 << 1)
#define DPAD_BITMAP_FLAG_DOWN (1 << 2)
#define DPAD_BITMAP_FLAG_LEFT (1 << 3)
#define DPAD_INVALID DPAD_CENTERED //Do not register invalid dpad combos (such as up and down)

const int DpadBitmapTrans[] =
{
  DPAD_CENTERED, // 0000
  DPAD_UP, // 0001
  DPAD_RIGHT, // 0010
  DPAD_UP_RIGHT, // 0011
  DPAD_DOWN, // 0100
  DPAD_INVALID, // 0101
  DPAD_DOWN_RIGHT, // 0110
  DPAD_INVALID, // 0111
  DPAD_LEFT, // 1000
  DPAD_UP_LEFT, //1001
  DPAD_INVALID, // 1010
  DPAD_INVALID, // 1011
  DPAD_DOWN_LEFT, // 1100
  DPAD_INVALID, // 1101
  DPAD_INVALID, // 1110
  DPAD_INVALID, // 1111
};

const int LATCH_PIN = 2;
const int CLOCK_PIN = 4;
const int DATA_PIN_0 = 15; 

int dPad_behaveAsStick = false;

GameControllers controllers;
BleGamepad bleGamepad("ESP32 SNES Gamepad", "Espressif", 100);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE gamepad");

  controllers.init(LATCH_PIN, CLOCK_PIN); 
  controllers.setController(0, GameControllers::SNES, DATA_PIN_0);

  BleGamepadConfiguration config;
  config.setButtonCount(10);
  config.setHatSwitchCount(1);
  config.setWhichAxes(true, true, false, false, false, false, false, false);
  config.setAxesMin(0);
  config.setAxesMax(2);

  bleGamepad.begin(&config);
}

void handleButtonPress(int controllerIndex, GameControllers::Button button, int bleButton) {
  if (controllers.down(controllerIndex, button)) {
    bleGamepad.press(bleButton);
  } else {
    bleGamepad.release(bleButton);
  }
}

int getDpadBitMap(void)
{
  int bitMap = 0;
  if (controllers.down(0, GameControllers::UP)) 
  {
    bitMap |= DPAD_BITMAP_FLAG_UP;
  }
  if (controllers.down(0, GameControllers::DOWN))
  {
    bitMap |= DPAD_BITMAP_FLAG_DOWN;
  }
  if (controllers.down(0, GameControllers::LEFT))
  {
    bitMap |= DPAD_BITMAP_FLAG_LEFT;
  }
  if (controllers.down(0, GameControllers::RIGHT)) 
  {
    bitMap |= DPAD_BITMAP_FLAG_RIGHT;
  }
  return bitMap; 
}

void handleDPadPress(int bitMap) 
{
  bleGamepad.setHat1(DpadBitmapTrans[bitMap]);
}

void loop() {
  controllers.poll(); //read all controllers at once

  if (!bleGamepad.isConnected())
    {
      Serial.println("BLE gamepad not connected");
      delay(500);
      return;
    }

  // A/B/X/Y buttons
  handleButtonPress(0, GameControllers::A, BUTTON_1);
  handleButtonPress(0, GameControllers::B, BUTTON_2);
  handleButtonPress(0, GameControllers::X, BUTTON_3);
  handleButtonPress(0, GameControllers::Y, BUTTON_4);

  // L1/R1 buttons
  handleButtonPress(0, GameControllers::L, BUTTON_5);
  handleButtonPress(0, GameControllers::R, BUTTON_6);

  // Start/Select buttons
  handleButtonPress(0, GameControllers::START, BUTTON_9);
  handleButtonPress(0, GameControllers::SELECT, BUTTON_10);

  // DPAD
  handleDPadPress(getDpadBitMap());
  bleGamepad.setLeftThumb(1, 1);
}