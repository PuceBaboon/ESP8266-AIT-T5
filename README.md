# AIT-T5
ESP8266 code (Arduino-ESP core) for the modified AI Thinker T5 board, implementing an MQTT-enabled DHT11 sensor.

The original AI-Thinker "Black board T5" has an on-board, 8051-based QFP processor, controlling the ESP8266
module via the TX/RX pins and (presumably) the AT command set.  Although the board is available quite
cheaply on the internet, there is virtually no information available on how to implement a workable
product, or how to program the 8051 to get access to the ESP8266.  After trying (and failing) to get
anything (with the notable exception of the annoying beeper) to work, I decided to rework the board to
allow direct access to the ESP8266.  The instructions for the rework process are available at:-

https://esp8266hints.wordpress.com/2015/12/07/reworking-the-ai-thinker-t5-board/

This repository contains the code to enable the ESP8266 on the T5 board to run standalone.

The user_config.h file contains (yes, you've guessed it!) configuration settings which need
to be updated to suit your particular network.  For instance, the SSID and password for your
WiFi access-point (to which the ESP8266 will connect), as well as IP addresses for the module
itself, default gateway (if you're connecting to the internet) and DNS server address.  You *must*
set these before compiling and loading the firmware to your ESP8266.

Do you need to use the AI Black-board T5?  Nope, you can basically use any ESP8266 module you
have to hand.  Of course, you'll need to connect a sensor, LED, etc., to your module to duplicate
the T5 functionality, but in all honesty, that's probably easier than reworking an actual T5.

**The DHT11_Test_Blinken.ino File**

This file contains the same, basic code as the original (ie:- it uses MQTT and deep-sleep to
create a stand-alone sensor module from the battery-powered T5 board).  However, it adds defines and
some extra code to implement the red and green LEDs (in addition to the previously used blue) to
display a "spinning wheel" type pattern at power on and subsequent deep-sleep wake-ups.

If you haven't modified your T5 board to add the wiring from GPIO12 and GPIO14 to the red and green
LEDs there's really no point in using this version; just stick with the original, non-Blinken
instead.
