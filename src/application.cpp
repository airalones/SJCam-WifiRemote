#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "Bounce.h"

#define WIFI_CONN_TIMEOUT   20

#ifndef WIFI_SSID
    #define WIFI_SSID "ABCam"
    #define WIFI_PWD "12345678"
#endif

#define SELECT_MODE_PIN     4
#define RELEASE_PIN         5
#define LEDSTS_PIN          2

#define DEBOUNCE_TIME       5


Timer procTimer;
Timer procTimer2;
Timer procTimer3;
Timer ledProcess;

Bounce modeBtnBouncer    = Bounce(SELECT_MODE_PIN, DEBOUNCE_TIME);
Bounce releaseBtnBouncer = Bounce(RELEASE_PIN, DEBOUNCE_TIME);

typedef enum {
    MODE_VIDEO = 0,
    MODE_PHOTO = 1
} SJCamMode_t;

typedef enum {
    CMD_GOTO_VIDEO_MODE = 0,
    CMD_GOTO_PHOTO_MODE = 1,
    CMD_START_REC_VIDEO = 2,
    CMD_STOP_REC_VIDEO  = 3,
    CMD_TAKE_A_PHOTO    = 4,
} SJCamCommand_t;

typedef struct {
    SJCamCommand_t cmd;
    String cmdNumber;
    String param;
} SJCamCmdMap_t;

static const SJCamCmdMap_t SJCamCmdMapping[] {
    { CMD_GOTO_VIDEO_MODE, "3001", "1" },
    { CMD_GOTO_PHOTO_MODE, "3001", "0" },
    { CMD_START_REC_VIDEO, "2001", "1" },
    { CMD_STOP_REC_VIDEO,  "2001", "0" },
    { CMD_TAKE_A_PHOTO,    "1001", "0" },
};

class SJCamRemote {
public :
    SJCamRemote()
    {
        currentMode = MODE_VIDEO;
    }
    ~SJCamRemote()
    {
        currentMode = MODE_VIDEO;
    }

    SJCamMode_t getCurrentMode () { return currentMode; }
    void setCurrentMode (SJCamMode_t mode) { currentMode = mode; }
    bool isVideoRecording () { return videoRecording; }

    void sendCommand (SJCamCommand_t cmd)
    {
        if (cmd == CMD_START_REC_VIDEO)
            videoRecording = true;
        else
            videoRecording = false;

        if (client.isProcessing())
            return;

        client.downloadString("http://192.168.1.254/?custom=1&cmd=" + SJCamCmdMapping[cmd].cmdNumber +
                                                            "&par=" + SJCamCmdMapping[cmd].param,
                              onSentResponse);
    }

private :
    HttpClient client;
    SJCamMode_t currentMode = MODE_VIDEO;
    bool videoRecording = false;

    static void onSentResponse(HttpClient& client, bool successful)
    {
        if (successful)
            Serial.println("Success sent command");
        else
            Serial.println("Failed command");
    }
};

SJCamRemote sjcam;

void processModeBtn()
{
    Serial.println(__func__);
    if (sjcam.getCurrentMode() == MODE_PHOTO) {
        Serial.println(CMD_GOTO_VIDEO_MODE);
        sjcam.sendCommand (CMD_GOTO_VIDEO_MODE);
        sjcam.setCurrentMode (MODE_VIDEO);
    }
    else if (sjcam.getCurrentMode() == MODE_VIDEO) {
        if (sjcam.isVideoRecording()) {
            sjcam.sendCommand (CMD_STOP_REC_VIDEO);
        }

        Serial.println(CMD_GOTO_PHOTO_MODE);
        sjcam.sendCommand (CMD_GOTO_PHOTO_MODE);
        sjcam.setCurrentMode (MODE_PHOTO);
    }
}

void processReleaseBtn()
{
    Serial.println(__func__);
    if (sjcam.getCurrentMode() == MODE_PHOTO) {
        Serial.println("CMD_TAKE_A_PHOTO");
        digitalWrite(LEDSTS_PIN, 0);
        sjcam.sendCommand (CMD_TAKE_A_PHOTO);
    }
    else if (sjcam.getCurrentMode() == MODE_VIDEO) {
        if (!sjcam.isVideoRecording()) {
            Serial.println("CMD_START_REC_VIDEO");
            sjcam.sendCommand (CMD_START_REC_VIDEO);
        }
        else {
            Serial.println("CMD_STOP_REC_VIDEO");
            sjcam.sendCommand (CMD_STOP_REC_VIDEO);
        }
    }
}

void processButton()
{
    if (modeBtnBouncer.update()) {
        if (modeBtnBouncer.read() == HIGH) {
            procTimer.initializeMs(1, processModeBtn).start(false);
        }
    }

    if (releaseBtnBouncer.update()) {
        if (releaseBtnBouncer.read() == HIGH) {
            procTimer2.initializeMs(1, processReleaseBtn).start(false);
        }
    }
}

void ledStsProcess()
{
	if (!WifiStation.isConnected()) {
        digitalWrite(LEDSTS_PIN, 0);
		return;
	}

    if (sjcam.getCurrentMode() == MODE_PHOTO) {
        digitalWrite(LEDSTS_PIN, 1);
    }
    else if (sjcam.getCurrentMode() == MODE_VIDEO) {
        if (sjcam.isVideoRecording()) {
            static bool ledSts = false;
            digitalWrite(LEDSTS_PIN, ledSts);
            ledSts = !ledSts;
        }
        else {
            digitalWrite(LEDSTS_PIN, 1);
        }
    }
}

void connectOk()
{
    Serial.println("CONNECTED");
    procTimer3.initializeMs(50, processButton).start();
}

void connectFail()
{
    Serial.println("CANNOT CONNECT WIFI.");
}

void init()
{
    //spiffs_mount(); // Mount file system, in order to work with files

    /* Uart init; 115200 and no system debug */
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.systemDebugOutput(false);

    /* Button init */
    pinMode(SELECT_MODE_PIN,INPUT);
    pinMode(RELEASE_PIN,INPUT);

    /* LED init */
    pinMode(LEDSTS_PIN,OUTPUT);
    digitalWrite(LEDSTS_PIN, 0);

    /* Start Wifi */
    WifiStation.config(WIFI_SSID, WIFI_PWD);
    WifiStation.enable(true);
    WifiAccessPoint.enable(false);

    WifiStation.waitConnection(connectOk, WIFI_CONN_TIMEOUT, connectFail);
    ledProcess.initializeMs(500, ledStsProcess).start();
}
