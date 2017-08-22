/*
 * OS_SystemTick.c
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#include "OS_Port.h"

void  OS_SystemTick_BSP()
{
    RCC_ClocksTypeDef  rcc_clocks;

    OS_SystemTick_StandardValue = 0;

    RCC_GetClocksFreq( &rcc_clocks );

    SysTick_Config( SystemCoreClock / OS_CONFIG_SystemTick_HZ );
}
