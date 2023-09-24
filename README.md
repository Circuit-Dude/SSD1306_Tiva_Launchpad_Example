# SSD1306_Tiva_Launchpad_Example
sandbox / learning experience for talking to the SSD1306 OLED driver/controller via I2C with a Tiva Launchpad


A couple of things to note:
  - I am circuit dude, not computer dude. My code is very ugly.
  - The SSD1306 OLED is marketed pretty heavily towards arduino, thus most of the documentation and "tutorials" are terrible IMO.
  -   I bought these things from amazon (from the seller MakerFocus) against my better judgement. The seller's documentation isn't very good. I was able to find what seems to be a legitimate datasheet from adafruit: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
  -   The initialization sequence in the adafruit datasheet didn't work for me, I copied the sequence from this guy https://microcontrollerslab.com/oled-interfacing-with-tm4c123g-display-texts-and-graphics/
  -   I created my own functions after compiling and running that microcontrollerslab exmaple because I didn't really like how he handled the intialization. If you piece together the information from the I2C interface and command table sections of the adafruit datasheet, it looks like you aren't supposed to send the stop bit in the middle of sending command data.


The base of the code was copied from a previous project and I just left that stuff in there. It's useful because the blinking lights can tell you if your uC is hung up.

The code uses I2C interface 0 on the Tiva. I created a bit map of my profile picture and instantiated it as a 2D char array, which is what displays on the screen. During runtime you can hit the onboard switches to do a couple of things:
  - SW1 will display the logo and invert the "color scheme"
  - SW2 will clear the display so that you don't burn in an image during debug / being distracted.

Due to the previous project stuff I kept in the switches also reset the LED color and changing the freqeucny of the lights blinking.


The displaying of the bitmap on the OLED has some tomfoolery involved with it. This comes as a result of the OLED graphics data addressing. 
