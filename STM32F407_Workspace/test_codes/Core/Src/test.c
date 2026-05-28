/*
 * test.c
 *
 *  Created on: May 1, 2026
 *      Author: SHAJIV BALAN S
 */
#include "test.h"

// --- BLDC Control ---
void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse) {
    if ((pulse < 1000) || (pulse > 2000)) return;
    __HAL_TIM_SET_COMPARE(timer, channel, pulse);
}
