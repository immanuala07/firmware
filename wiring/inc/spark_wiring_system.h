/**
 ******************************************************************************
 * @file    spark_wiring_system.h
 * @author  Satish Nair, Zachary Crockett, Matthew McGowan
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

#ifndef SPARK_WIRING_SYSTEM_H
#define	SPARK_WIRING_SYSTEM_H
#include "spark_wiring_ticks.h"
#include "spark_wiring_string.h"
#include "system_mode.h"
#include "system_update.h"
#include "system_sleep.h"
#include "system_cloud.h"
#include "system_event.h"
#include "core_hal.h"
#include "interrupts_hal.h"
#include "core_hal.h"
#include "system_user.h"
#ifdef SPARK_PLATFORM
#include "hw_ticks.h"
#if PLATFORM_ID<3
#include "stm32f10x.h"
#else
#include "stm32f2xx.h"
#endif
#endif

class Stream;

class SystemClass {
public:

    SystemClass(System_Mode_TypeDef mode = DEFAULT) {
        set_system_mode(mode);
    }

    static System_Mode_TypeDef mode(void) {
        return system_mode();
    }

    static bool firmwareUpdate(Stream *serialObj) {
        return system_firmwareUpdate(serialObj);
    }

    static void factoryReset(void);
    static void dfu(bool persist=false);
    static void reset(void);

    static void enterSafeMode(void) {
        HAL_Core_Enter_Safe_Mode(NULL);
    }

#ifdef SPARK_PLATFORM
    static inline uint32_t ticksPerMicrosecond()
    {
        return SYSTEM_US_TICKS;
    }

    static inline uint32_t ticks()
    {
        return DWT->CYCCNT;
    }

    static inline void ticksDelay(uint32_t duration)
    {
        uint32_t start = ticks();
        while ((ticks()-start)<duration) {}
    }
#endif

    static void sleep(Spark_Sleep_TypeDef sleepMode, long seconds=0);
    static void sleep(long seconds) { sleep(SLEEP_MODE_WLAN, seconds); }
    static void sleep(uint16_t wakeUpPin, InterruptMode edgeTriggerMode, long seconds=0);
    static String deviceID(void) { return spark_deviceID(); }

    static uint16_t buttonPushed(uint8_t button=0) {
        return system_button_pushed_duration(button, NULL);
    }

    static bool on(system_event_t events, void(*handler)(system_event_t, uint32_t,void*)) {
        return !system_subscribe_event(events, handler, nullptr);
    }

    /* Contemplating allowing the callback to be a subset of the parameters
    static bool on(system_event_t events, void(*handler)(system_event_t, uint32_t)) {
        return system_subscribe_event(events, (system_event_handler_t*)handler, NULL);
    }

    static bool on(system_event_t events, void(*handler)(system_event_t)) {
        return system_subscribe_event(events, (system_event_handler_t*)handler, NULL);
    }

    static bool on(system_event_t events, void(*handler)()) {
        return system_subscribe_event(events, (system_event_handler_t*)handler, NULL);
    }
    */

    static void off(void(*handler)(system_event_t, uint32_t,void*)) {
        system_unsubscribe_event(all_events, handler, nullptr);
    }

    static uint32_t freeMemory();

    template<typename Condition, typename While> static bool waitConditionWhile(Condition _condition, While _while) {
        while (_while() && !_condition()) {
            spark_process();
        }
        return _condition();
    }

    template<typename Condition> static bool waitCondition(Condition _condition) {
        return waitConditionWhile(_condition, []{ return true; });
    }

    template<typename Condition> static bool waitCondition(Condition _condition, system_tick_t timeout) {
        const system_tick_t start = millis();
        return waitConditionWhile(_condition, [=]{ return (millis()-start)<timeout; });
    }

    bool set(hal_system_config_t config_type, const void* data, unsigned length)
    {
        return HAL_Set_System_Config(config_type, data, length)>=0;
    }

    bool set(hal_system_config_t config_type, const char* data)
    {
        return set(config_type, data, strlen(data));
    }


    inline bool featureEnabled(HAL_Feature feature)
    {
        return HAL_Feature_Get(feature);
    }

    inline int enableFeature(HAL_Feature feature)
    {
        int result = HAL_Feature_Set(feature, true);
        if (feature==FEATURE_RETAINED_MEMORY && !HAL_Feature_Get(FEATURE_WARM_START)) {
            system_initialize_user_backup_ram();
        }
        return result;
    }

    inline int disableFeature(HAL_Feature feature)
    {
        return HAL_Feature_Set(feature, false);
    }

};

extern SystemClass System;

#define SYSTEM_MODE(mode)  SystemClass SystemMode(mode);


#define waitFor(condition, timeout) System.waitCondition([]{ return (condition)(); }, (timeout))
#define waitUntil(condition) System.waitCondition([]{ return (condition)(); })

#endif	/* SPARK_WIRING_SYSTEM_H */

