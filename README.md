# ESP32-BLE-SNES-controller 🎮🎮

Project for converting an SNES controller into a Bluetooth gamepad using the ESP32 microcontroller.

## Controller:
- **SFC clear controller (Super potato japan)**
  - Model: 16Bit SFC CC-SFCCR
  - Product Code: CLBCC-SFCCR-BL/BK
  - JAN code: 4582286321407

<p float="left">
  <img src="/Image/Controller_internal.jpg" width="45%" />
  <img src="/Image/Controller_back.jpg" width="45%" /> 
</p>
<p float="left">
  <img src="/Image/Controller_front.jpg" width="45%" />
  <img src="/Image/Controller_usb.jpg" width="45%" /> 
</p>
<img src="/Image/Btn_mapping.png" width="50%" />

## PCB
- Layers: 2
- PCB Thickness: 0.8mm
- Base Material: FR-4

<img src="/Image/PCB.png" width="80%" />
<img src="/Image/Schematic.png" width="80%" />

## GPIO Connection
| ESP32 pin | SNES Controller | Wire color |
|---:|:-------:|:--------:|
| 2  |   Latch |   Yellow | 
| 4  |   Clock |   Blue   | 
| 15 |   Data  |   Red    | 
|    |   GND   |   Black  | 
|    | VCC 3.3v|   White  | 

## Libraries
- [ESP32-BLE-Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad)
- [ArduinoGameController](https://github.com/bitluni/ArduinoGameController)

## Parts List
- ESP32-S
- 18650 1A Lipo Battery Charging Board Charger Module TP4056 Type-C
- 300mAh 601235 Rechargeable Lithium Polymer Battery
- LM1117MP-3.3/TR 3.3v Voltage Regulator
- 47 µF capacitor D6.3xL11mm
- Switch SK12D07VG3
- Tactile Tact push button 3x6x2.5MM G73
- 10k resistor
- 2.54mm pin header

