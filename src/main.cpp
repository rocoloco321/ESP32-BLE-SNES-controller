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
int keyComboCooldown = 0;

GameControllers controllers;
BleGamepad bleGamepad("ESP32 SNES Gamepad", "Espressif", 100);

void setup() {
  //Serial.begin(115200);
  //Serial.println("Starting BLE gamepad");

  controllers.init(LATCH_PIN, CLOCK_PIN); 
  controllers.setController(0, GameControllers::SNES, DATA_PIN_0);

  BleGamepadConfiguration config;
  //config.setButtonCount(10);
  config.setAutoReport(false);
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

void handleModeSwitch()
{
  if(keyComboCooldown > 0) //Add a cooldown to the modeswitch keycombo
  {
    keyComboCooldown--;
    return;
  }

  if(controllers.down(0, GameControllers::L) 
    && controllers.down(0, GameControllers::DOWN)
    && controllers.down(0, GameControllers::SELECT)) //Check for L + DOWN + SELECT to switch DPad Modes
    {
      dPad_behaveAsStick = (dPad_behaveAsStick + 1) % 2;
      keyComboCooldown = 60;
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

void handleLeftStick(int bitMap)
{
  int x = 1;
  int y = 1;
  if(bitMap & DPAD_BITMAP_FLAG_LEFT)
  {
    x = 0;
  }
  else if(bitMap & DPAD_BITMAP_FLAG_RIGHT)
  {
    x = 2;
  } 
  if(bitMap & DPAD_BITMAP_FLAG_DOWN)
  {
    y = 2;
  }
  else if(bitMap & DPAD_BITMAP_FLAG_UP)
  {
    y = 0;
  }
  bleGamepad.setLeftThumb(x, y);
}

void loop() {
  controllers.poll(); //read all controllers at once

  if (!bleGamepad.isConnected())
    {
      //Serial.println("BLE gamepad not connected");
      delay(500);
      return;
    }

  // A/B/X/Y buttons
  handleButtonPress(0, GameControllers::A, BUTTON_1);
  handleButtonPress(0, GameControllers::B, BUTTON_2);
  handleButtonPress(0, GameControllers::X, BUTTON_4);
  handleButtonPress(0, GameControllers::Y, BUTTON_5);

  // L1/R1 buttons
  handleButtonPress(0, GameControllers::L, BUTTON_7);
  handleButtonPress(0, GameControllers::R, BUTTON_8);

  // Start/Select buttons
  handleButtonPress(0, GameControllers::START, BUTTON_12);
  handleButtonPress(0, GameControllers::SELECT, BUTTON_11);

  // DPAD
  handleModeSwitch();
  int bitMap = getDpadBitMap();
  if(!dPad_behaveAsStick)
  {
    handleDPadPress(bitMap);
    bleGamepad.setLeftThumb(1, 1);
  }
  else
  {
    bleGamepad.setHat1(DPAD_CENTERED);
    handleLeftStick(bitMap);
  }
  bleGamepad.sendReport();
//Add a small delay to prevent an issue where the left stick would randomly go to the top left and every button would be released for a split second
  delay(10); 
}