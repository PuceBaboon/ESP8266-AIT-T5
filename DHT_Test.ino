/*
 * $Id: DHT_Test.ino,v 1.15 2016/01/01 03:43:35 anoncvs Exp $
 *
 * Test program for the AI Thinker "Black board T5", after
 * modification to remove the 8051-based QFP micro and wire
 * the DHT11 sensor and blue LED directly to the ESP8266.
 *
 * Spends most of the time with the ESP8266 in deep-sleep;
 * waking every 10-minutes to read the DHT11 sensor and
 * publish the data to MQTT.
 *
 * NOTE - All user-specific settings (SSID/Passwd, etc)
 *        are in user_config.h.  These settings -MUST-
 *        be customized for -your- network.
 */

#include <ESP8266WiFi.h>
#include <pgmspace.h>
#include <DHT.h>
#include <MQTT.h>
#include "user_config.h"

/* Globals (Terribly bad style, old boy!) */
boolean flgIsConnected = false;	// MQTT connected-to-server flag.
boolean flgIsPublished = false;	// MQTT publish-completed flag.
int BLED = BLED_PIN;		// WiFi debug LED (set in user_config.h).
char buff[MQ_TOPIC_SIZE];
char clidbuff[15], vbuff[TEMPR_SIZE], tbuff[TEMPR_SIZE], hbuff[TEMPR_SIZE];
float temperature = 0;
float humidity = 0;

/* The following line is required for proper operation of VCC measurements. */
ADC_MODE(ADC_VCC);

/* Initialize the DHT sensor type to DHT11 (pin configured in usr_config.h). */
DHT dht(DHT_PIN, DHT11, 15);

/* Create WiFi client object. */
WiFiClient wclient;

/* NOTE:- All of these values are configured in user_config.h.  */
const byte ip[] = WIFI_IPADDR;
const byte gateway[] = WIFI_GATEWY;
const byte netmask[] = WIFI_NETMSK;
const byte dnssrv[] = WIFI_DNSSRV;


/*
 * Turn on the blue WiFi debug LED for a short
 * flash. Note that on the T5 board the LED is
 * driven by a low-side switch.
 */
void BFlash() {
    digitalWrite(BLED, LOW);
    delay(40);
    digitalWrite(BLED, HIGH);
}


/*
 * Show the world we're alive. A short, rapid flash cycle
 * for the blue LED at power-up.
 */
void OnFlash() {
    for (int i = 0; i < 3; i++) {
	BFlash();
	delay(60);
    }
}


/*
 * Try to get a valid reading from the DHT11 sensor
 * (not always easy).
 */
void getSensorData() {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    while ((isnan(temperature) || isnan(humidity))
	   || (temperature == 0 && humidity == 0)) {
#ifdef DEBUG
	Serial.print(F("!"));
#endif
	delay(500);
	temperature = dht.readTemperature();
	humidity = dht.readHumidity();
    }
#ifdef DEBUG
    Serial.println("");
#endif
}


/*
 *  Create callbacks for the MQTT functions.
 */
void dsConnectedCb() {
#ifdef DEBUG
    Serial.println(F("MQTT connected."));
#endif
    flgIsConnected = true;
}


void dsDisconnectedCb() {
#ifdef DEBUG
    Serial.println(F("MQTT disconnected."));
#endif
    flgIsConnected = false;
    flgIsPublished = false;
}


void dsPublishedCb() {
#ifdef DEBUG
    Serial.println(F("Published."));
#endif
    flgIsPublished = true;
}


void dsDataCb(String & topic, String & data) {
#ifdef DEBUG
    Serial.print(topic);
    Serial.print(F(": "));
    Serial.println(data);
#endif
}


/*
 * Call deep-sleep.  The DSLEEP_SECS seconds count is set in
 * user_config.h.
 * Note that "NOTREACHED" should never be displayed if
 * deep-sleep is working correctly, as the processor will do
 * a full reset when it wakes up.
 */
void DSleep() {
    /*
     * Close down our WiFi connections cleanly before sleeping.
     */
    WiFi.disconnect(true);	// Switch-off WiFi = "true".
    yield();
    delay(500);

#ifdef DEBUG
    Serial.println(F("Zzzz..."));
#endif
    // ESP.deepSleep(DSLEEP_SECS * 1000 * 1000, WAKE_RF_DISABLED);
    ESP.deepSleep(DSLEEP_SECS * 1000 * 1000);

    /*
     * Pause everything to give the system time to set up deep-sleep
     * and shut itself down before restarting the default loop.
     */
    yield();
    delay(5000);
    Serial.println("/*NOTREACHED*/");	/* Lint on the ESP?!? :-) */
}


void setup() {
    int conn_tries = 0;

    Serial.begin(115200);
    delay(500);
    Serial.println();

    pinMode(BLED, OUTPUT);	// Ensure our blue LED driver pin is an output.    
#ifdef DEBUG
    OnFlash();			// We're alive!
#endif

    dht.begin();		// Initialize the DHT11 interface.

    /* Get a unique client-ID for this ESP8266. */
    sprintf(clidbuff, MQTT_CLIENT_ID, ESP.getChipId());
    float vdd = ESP.getVcc() / 1000.0;	// Grab the current battery voltage.
    dtostrf(vdd, 4, 2, vbuff);	// Convert to ASCII and store.

    /* This delay is for use when testing without deep-sleep. */
//    delay(LDELAY);    // Disable this loop delay if using deep-sleep.

    getSensorData();
#ifdef DEBUG
    BFlash();			// A quick flash to show we've accessed the DHT11.
#endif
    dtostrf(temperature, 4, 2, tbuff);	// Convert and store.
    dtostrf(humidity, 4, 2, hbuff);	// Convert and store.
    snprintf(buff, MQ_TOPIC_SIZE, "%s, %sC, %s%% %sV", clidbuff, tbuff,
	     hbuff, vbuff);
#ifdef DEBUG
    Serial.println(buff);
#endif

#ifdef DEBUG
    Serial.print(F("   ClientID: "));
    Serial.println(clidbuff);
    Serial.print(F("Temperature: "));
    Serial.print(tbuff);
    Serial.println(F("C"));
    Serial.print(F("   Humidity: "));
    Serial.print(hbuff);
    Serial.println(F("%"));
    Serial.print(F("    Voltage:  "));
    Serial.print(vbuff);
    Serial.println(F("V"));
#endif

    /*
     * Now we have the temperature, humidity and voltage readings, so
     * we're ready to bring up the network and talk to the outside
     * world.
     */
#ifdef DEBUG
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(STA_SSID);
#endif

    /*
     * An open-ended loop  here will flatten the battery on
     * the sensor if, for instance, the access point is 
     * down, so limit our connection attempts.
     */
    Serial.print("WiFi Stat: ");
    Serial.println(WiFi.status());		// Meant to fix slow-start problem.
    WiFi.mode(WIFI_STA);
    WiFi.config(IPAddress(ip), IPAddress(gateway),
		IPAddress(netmask), IPAddress(dnssrv));
//    wifi_station_set_auto_connect(false);
    WiFi.begin(STA_SSID, STA_PASS, WIFI_CHANNEL, NULL);
    yield();

    while ((WiFi.status() != WL_CONNECTED)
	   && (conn_tries++ < WIFI_RETRIES)) {
	delay(500);
	Serial.print(".");
    }
    if (conn_tries == WIFI_RETRIES) {
#ifdef DEBUG
	Serial.println(F("No WiFi connection ...sleeping."));
#endif
	DSleep();
    }
#ifdef DEBUG
    Serial.println();
    WiFi.printDiag(Serial);
    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP());
#endif

    /*
     * Now that we have a WiFi link, publish our data to the MQTT
     * server.  As with the WiFi connection, the MQTT publish
     * attempts are in a limited loop to prevent a failed connection
     * from holding the ESP8266 on and draining the battery.
     */
    MQTT mqttClient(clidbuff, MQTT_HOST, MQTT_PORT);

    /* Configure MQTT callbacks.  */
    mqttClient.onConnected(dsConnectedCb);
    mqttClient.onDisconnected(dsDisconnectedCb);
    mqttClient.onPublished(dsPublishedCb);
    mqttClient.onData(dsDataCb);

    /*
     * MQTT connect/publish loop.
     */
    // mqttClient.connect();
    yield();
    conn_tries = 0;		// Reset tries counter to zero.
    while (!flgIsPublished && (conn_tries++ < WIFI_RETRIES)) {
	delay(MQCONDEL);
#ifdef DEBUG
	Serial.print(F("Content: "));
	Serial.println(buff);
#endif
	if (flgIsConnected) {
	    mqttClient.publish(TOPIC3, buff, strlen(buff), 0, 0);	// Topics set in user_config.h.
	} else {
#ifdef DEBUG
	    Serial.print(F("Connecting to MQTT server: "));
	    Serial.println(MQTT_HOST);
#endif
	    mqttClient.connect();
	}
    }

    /*
     * If we didn't manage to publish, for whatever reason, we're
     * going to give up and go back to sleep to save battery 
     * power, otherwise do a clean disconnect from the MQTT server.
     */
    if (!flgIsPublished) {
#ifdef DEBUG
	Serial.println(F("No MQTT ...sleeping."));
#endif
    }
    mqttClient.disconnect();
    delay(500);

    /*
     * Put the unit into deep sleep (requires
     * GPIO16 to be connected to RST for
     * wake-up).
     */
    DSleep();
}


void loop() {
    // Nuthin' to see here.  Move along now!
    ESP.restart();
}
