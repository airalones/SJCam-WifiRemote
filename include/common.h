/*
 * common.h
 *
 *  Created on: Oct 24, 2015
 *      Author: Airalone
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <SmingCore/SmingCore.h>

#include <devdbg.h>
#include <peripheral_pin.h>
#include <ret_code.h>

#define WIFI_SSID 					"Airalone-Wifi2"
#define WIFI_PWD 					"helloworld789"

#define NTP_SERVER					NTP_DEFAULT_SERVER
#define NTP_UPDATE_INTERVAL			NTP_DEFAULT_QUERY_INTERVAL_SECONDS

#define PROC_SENSOR_READY_INTERVAL  (2000)
#define PROC_DISPLAY_INTERVAL       (500)

#endif /* __COMMON_H__ */
