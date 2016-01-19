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

####The DHT11_Test_Blinken.ino File
This file contains the same, basic code as the original (ie:- it uses MQTT and deep-sleep to
create a stand-alone sensor module from the battery-powered T5 board).  However, it adds defines and
some extra code to implement the red and green LEDs (in addition to the previously used blue) to
display a "spinning wheel" type pattern at power on and subsequent deep-sleep wake-ups.

If you haven't modified your T5 board to add the wiring from GPIO12 and GPIO14 to the red and green
LEDs there's really no point in using this version; just stick with the original, non-Blinken
instead.

####Blinken_K2.ino
This version adds support for the K2 "mode" switch.  At power-on, the ESP waits for 3-seconds
before starting the main program.  If switch K2 is pressed during that time the program
switches into "Blinkenlights" mode.  At the end of the light show, the program will automatically
drop back into the normal operating mode.  Additional differences are:-

* The power-on indicator has changed to two, very brief flashes of the green LED.
* The DHT11 sensor-read indicator has changed to a brief flash of the red LED.
    Note that the red LED may flash multiple times if the ESP sees *bad* data from the DHT11 and re-tries the read operation.

####Blinken_K2_Epoch.ino
This version changes the TOPICs (set in user_config.h) to drop the leading "/" character (apparently leading slashes are frowned upon in the MQTT world) and to change TOPIC1 to "TIME/Epoch".  This allows the ESP8266 to pick up a Unix "epoch" format timestamp from the (NTP controlled) MQTT server and add it to the sensor data before publishing.  This method gives reasonable accuracy without needing the extra overhead of an NTP client process on the ESP8266 itself.  The epoch time process on the MQTT server only publishes at 10-second intervals, but it runs with the "retain" flag set, which means any ESP module subscribing to the TIME/Epoch topic will receive the last published timestamp immediately, without having to wait for the next, scheduled publication.

The data published by the ESP8266 now looks like this:-

<code>1453168978, Puce_0006969A, 15.00C, 53.00%, 3.42V</CODE>

...with the fields being
* Epoch-format timestamp.
* ESP8266 unique client-ID.
* Temperature (Celsius).
* Relative Humidity.
* VCC.

**NOTE** The humidity readings from the DHT11 are all over the shop and everyone seems to agree that an upgrade to the slightly more expensive DHT22 is well worth the effort.


###Quiet mode
To disable all of the output to the console (with the exception of the "blinken" prompt) and all
of the flashing LEDs, simply change the line in user_config.h which starts with
<code>#define DEBUG</code>
to start with
<code>#undef DEBUG</code>
instead.
