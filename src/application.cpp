#include <user_config.h>
#include <SmingCore/SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
    #define WIFI_SSID "ABCam" // Put you SSID and Password here
    #define WIFI_PWD "12345678"
#endif

Timer procTimer;
int sensorValue = 0;
HttpClient sjCam;

void onDataSent(HttpClient& client, bool successful)
{
    if (successful)
        Serial.println("Success sent");
    else
        Serial.println("Failed");

    String response = client.getResponseString();
    Serial.println("Server response: '" + response + "'");
    if (response.length() > 0)
    {
        int intVal = response.toInt();

        if (intVal == 0)
            Serial.println("Sensor value wasn't accepted. May be we need to wait a little?");
    }
}

void sendData()
{
    static int flag = 0;
    if (sjCam.isProcessing()) return; // We need to wait while request processing was completed

    // Read our sensor value :)
    sensorValue++;

    if (flag == 0) {
        flag = 1;
        Serial.println("Go to photo mode");
        sjCam.downloadString("http://192.168.1.254/?custom=1&cmd=3001&par=0", onDataSent);
    }
    else {
        flag = 0;
        Serial.println("Go to video mode");
        sjCam.downloadString("http://192.168.1.254/?custom=1&cmd=3001&par=1", onDataSent);
    }

}

// Will be called when WiFi station was connected to AP
void connectOk()
{
    Serial.println("I'm CONNECTED");

    // Start send data loop
    procTimer.initializeMs(10 * 1000, sendData).start(); // every 10 seconds
}

// Will be called when WiFi station timeout was reached
void connectFail()
{
    Serial.println("I'm NOT CONNECTED. Need help :(");

    // Start soft access point
    WifiAccessPoint.enable(true);
    WifiAccessPoint.config("CONFIG ME PLEEEEASE...", "", AUTH_OPEN);

    // .. some you code for configuration ..
}

void init()
{
    spiffs_mount(); // Mount file system, in order to work with files

    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(false); // Disable debug output to serial

    WifiStation.config(WIFI_SSID, WIFI_PWD);
    WifiStation.enable(true);
    WifiAccessPoint.enable(false);

    // Run our method when station was connected to AP (or not connected)
    WifiStation.waitConnection(connectOk, 20, connectFail); // We recommend 20+ seconds for connection timeout at start
}
