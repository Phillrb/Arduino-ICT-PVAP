# Arduino-ICT-PVAP
Support for some games missing from Paul Swan's Arduino Mega ICT PVAP

Original / base code available at:
https://github.com/prswan/arduino-mega-ict

Currently adding support for:
- Asteroids
- Lunar Lander
- Pacland

# Sainsmart LCD shield support
The [DFR_Key.cpp](/libraries/DFR_Key/DFR_Key.cpp) file is configured for my own LCD shield as the threshold values for the buttons on the shield differs from that supported in the original Paul Swan repo. You will probably need to adjust the values for your own LCD shield.

The values were generated using the [LCD_buttons_discovery.ino](/extras/LCD_buttons_discovery.ino) project courtesy of https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
