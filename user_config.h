/*
 * $Id: user_config.h,v 1.6 2016/01/01 03:45:08 anoncvs Exp $
 *
 * Configuration for local set-up (ie:- Access-point ID & PWD).
 */

/*
 * GENERAL SETUP
 */
#define DEBUG		1		// Verbose output. "#undef" to disable.
#define BLINKEN		1		// Enable "blinkenlights" option.
#define CLKWISE		0		// LED "spin" direction: Clockwise.
#define CCLWISE		1		// Counter-clockwise.
#define LDELAY		10 * 1000	// Loop delay (10 seconds).

/*
 * ESP8266 SETUP
 */
#define DHT_PIN		2		// DHT11 sensor on ESP pin GPIO2.
#define K2_PIN		4		// Switch K2 on ESP pin GPIO4.
#define	BLED_PIN	5		// Blue-LED on ESP pin GPIO5.
#define	GLED_PIN	14		// Green-LED on ESP pin GPIO14.
#define	RLED_PIN	12		// Red-LED on ESP pin GPIO12.
#define DSLEEP_SECS	600		// Deep-sleep time, in seconds (600 = 10-mins).

/*
 * WIFI SETUP
 *
 * Number of times to test WiFi connection status before 
 * giving up and going back into deep-sleep (this is to
 * prevent the battery being flattened by an unavailable
 * access-point, or other, external problems.
 */
#define WIFI_RETRIES	20

/* Assign a static IP address to your ESP8266 */
#define WIFI_IPADDR	{ 192, 168, 1, 200 }
#define WIFI_NETMSK	{ 255, 255, 255, 0 }
#define WIFI_GATEWY	{ 192, 168, 1, 1 }
#define WIFI_DNSSRV	{ 192, 168, 1, 2 }
#define WIFI_CHANNEL	6		// !!You MUST change this!! -- Restart might be unreliable without this set.

#define STA_SSID	"Zippy"		// !!You MUST change this!!
#define STA_PASS	"Geoffrey"	// !!You MUST change this!!
#define STA_TYPE	AUTH_WPA2_PSK


/*
 * MQTT SETUP
 */
// #define PROTOCOL_NAMEv31      /* MQTT version 3.1 compatible with v0.15 */
#define MQTT_HOST	"mymqttserver.mydomain.com"
#define MQTT_PORT	1883
#define MQTT_BUF_SIZE	2048
#define MQTT_KEEPALIVE	120	/* In seconds. */
#define MQCONDEL	150	/* MQTT connection wait delay. */
#define MQ_TOPIC_SIZE	256	/* Characters. */
#define	TEMPR_SIZE	8	/* Temperature string conv temp storage. */
#define STRBUFF_SIZE	15	/* String buffer size. */

#define MQTT_CLIENT_ID	"Puce_%08X"
#define MQTT_USER	"Puce_USER"
#define MQTT_PASS	"Puce_PASS"

#define TOPIC1		"TIME/Epoch"
#define TOPIC2		"tdata/raw"
#define TOPIC3		"temperature"

#define MQTT_RECONNECT_TIMEOUT 	5	/* In Seconds. */
