#ifndef __ESP32_WATCHDOG_H__
#define __ESP32_WATCHDOG_H__

#include "Arduino.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "soc/rtc_wdt.h"
#ifdef __cplusplus
extern "C" {
#endif

void IRAM_ATTR wm_resetModule();

void IRAM_ATTR wm_feedWdt(uint8_t core);

void IRAM_ATTR wm_feedWdtVKL();

void IRAM_ATTR wm_setupWdt();

#ifdef __cplusplus
}
#endif

#endif