#ifdef ESP32
#include "wm_esp32_watchdog.h"
#include "esp_attr.h"

static hw_timer_t* wdtTimer = NULL;
static uint8_t feedWdtLastCore = 0;
static uint8_t feedWdtCounter = 0;

static void watchdog_task (void* pv) {
	while (1) {
		wm_feedWdt(0);
		
		vTaskDelay(pdMS_TO_TICKS(100)); // 100ms
	}
}

void IRAM_ATTR wm_resetModule() {
	esp_restart();
}

void IRAM_ATTR wm_feedWdtVKL() {
	rtc_wdt_feed(); // reset RTC watchdog
	esp_task_wdt_reset();
	if (wdtTimer != NULL) 
		timerWrite(wdtTimer, 0); //reset timer (feed watchdog)
}
void IRAM_ATTR wm_feedWdt(uint8_t core) {
	// if (feedWdtLastCore != core) {
	// 	feedWdtCounter++;
	// }
	
	// if (feedWdtCounter >= 3) {
	// 	wm_feedWdtVKL();
	// 	feedWdtCounter = 0;
	// }
	
	// feedWdtLastCore = core;
	wm_feedWdtVKL();
}

void IRAM_ATTR wm_setupWdt() {
	uint32_t ms = 20000UL; // 10 seconds
	
	// setup RTC reset (system, not just CPU !important)
	rtc_wdt_protect_off();
    rtc_wdt_disable();
    rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_3_2us);
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_SYSTEM);
    rtc_wdt_set_time(RTC_WDT_STAGE0, ms + 5000); // in milli seconds
    rtc_wdt_enable();
    rtc_wdt_protect_on();
	
	// init interrupt wdt
	wdtTimer = timerBegin(0, 80, true); //timer 0, div 80
	timerAttachInterrupt(wdtTimer, &wm_resetModule, true);
	timerAlarmWrite(wdtTimer, ms*1000UL, false); //set time in us

	// https://github.com/espressif/arduino-esp32/issues/1313
	// if the following yield() is removed, the timer will not
	// be enabled the second time
	yield();
	timerAlarmEnable(wdtTimer); //enable interrupt
	
	enableLoopWDT();
	
	// if (xTaskCreatePinnedToCore(watchdog_task, "watchdog_task", 1024, NULL, 1, NULL, 0) != pdTRUE) {
	// 	delay(3000);
	// 	esp_restart();
	// }
	
	// init task wdt
	// please no call: esp_task_wdt_init
}

#endif
