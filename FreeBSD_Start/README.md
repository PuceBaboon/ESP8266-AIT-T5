
$Id: README.md,v 1.1 2016/01/18 01:47:19 anoncvs Exp $

#Example start environment for mosquitto local daemon under FreeBSD

This directory contains three scripts to enable starting of a
mosquitto support daemon under FreeBSD.  It provides stop, start,
status, restart and rcvar functionality when used with the
FreeBSD "service" command and, of course, stop and start
functionality in normal system boot/halt/reboot situations.


##The Daemon
The daemon implemented in this example is a very simple
timestamp publisher.  All it does is publish a regular timestamp
in Unix "epoch" format to the topic "TIME/Epoch".  The delay
between publications is settable (the default is 10 seconds).

The timestamp is published using the "retain" flag, so
that any client coming back on-line (for instance, from a
deep-sleep) will receive the most recent timestamp immediately
after (re-)subscribing to the topic, instead of having to wait
until the next scheduled publication time.  What this means is
that ESP8266 embedded sensors, which don't need millisecond
accuracy, can quickly get a timestamp from the MQTT server
without the overhead of an NTP client routine and embed it
directly into the sensor data which they then publish back to the
server. 


##Daemon Configuration
It demonstrates a couple of useful functions of the more recent FreeBSD releases' rc structure:-.

- Uses /usr/local/etc/rc.d for daemons which are not system standard.

- Implements the "REQUIRE" dependency to ensure that the mosquitto parent process is started before the daemon.

- Implements the daemon process as a "double-forked", detached child process.

- Demonstrates the use of the "command_interpreter" option to ensure that the (shell script) daemon process is recognized correctly when checking the process table.

- Uses a non-root UID to run the daemon process (in this case, "nobody").


###NOTES

####Install under /usr/local
- The files in the sub-directories must be installed in the /usr/local tree, -not- at root level.

####Chown and chmod
- The files should be chowned to "nobody:nobody" (the default UID for this daemon) and chmoded to 0550.


