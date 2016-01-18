#!/bin/sh

##
## $Id: m_epoch.sh,v 1.1 2016/01/18 01:50:25 anoncvs Exp $
##
## Publish Unix "epoch" format timestamps using the MQTT 
## broker and mosquitto_pub.  Intended for use by ESP8266
## embedded systems.
##

##
##------------------<CONFIGURE>-----------------------
##
## Set the time to sleep (in seconds) between successive
## publications, eg:-
##		"20"
## (sleeps for 20 seconds before publishing next data).
##
SLP_TM=10;
##
##---
##
## Set TOPIC to a valid topic string, such as:-
##		"TEST/output"
## (use the command "man mosquitto_pub" for details).
## Note that it is recommended -not- to start a topic
## string with a leading "/" character, as it introduces
## a leading null topic specifier, which can have 
## unexpected results.
##
TOPIC="TIME/Epoch";
##
##---
##
## Set "retain" to have the server use the last value
## published as a retained message to send to newly
## subscribing (or re-subscribing) clients.  This is
## useful to have the most recent timestamp sent to
## battery-operated sensors immediately on wake-up.
##
RETAIN="-r"			## Set to "" to disable.
##
##---
##
## Set OP_CMD to be the (full path) of the command
## whose output you want to publish, eg:-
##		"/bin/df -Plht ext4"
## (to publish local disk filesystem capacities)
##		"/bin/date +%H:%M:%S"
## (to publish the current time in HH:MM:SS format)
##
OP_CMD="/bin/date +%s";
##
##------------------<END CONFIG>-----------------------
##
PATH=/usr/bin;
MPUB=/usr/local/bin/mosquitto_pub;
MSUB=/usr/local/bin/mosquitto_sub;
SLEEP=/bin/sleep;
EGREP=/usr/bin/egrep;

## Syslog messages.
Logger(){
	logger -s "${0}: ${*}";
}

## Send info to the console (if there is one) and syslog.
Totty(){
	[ `tty -s` ] && printf "\n\t${*}\n\n";
	Logger "${*}";
}

## Oh dear!  We are outa here...
Fatal(){
	Totty "FATAL: ${*}";
	exit 1;
}

[ ! -x ${MPUB} ] && Fatal "${MPUB} not executable.";
[ ! -x ${SLEEP} ] && Fatal "${SLEEP} not executable.";
[ ! -x ${EGREP} ] && Fatal "${EGREP} not executable.";

[ -z "${TOPIC}" ] && Fatal "A TOPIC is required (not optional).";
[ -z "${OP_CMD}" ] && Fatal "Publishing requires OP_CMD data (not optional).";
[ -z "${SLP_TM}" ] && Fatal "Need a numeric value for "SLP_TM" (not optional).";
echo "${SLP_TM}" | ${EGREP} -qsv "[0-9]" >/dev/null 2>&1;
SLP_RV=${?};
[ "${SLP_RV}" -ne 1 ] && Fatal "Config SLP_TM must be a numeric value (ie:- \"20\").";
[ ${SLP_TM} -le 0 ] && Fatal "Need a valid SLP_TM value (eg:- SLP_TM=10;)";

##
## Endless loop.
##
while [ true ]; do
	${MPUB} -t "${TOPIC}" ${RETAIN} -m `${OP_CMD}`;
	${SLEEP} ${SLP_TM};
done
