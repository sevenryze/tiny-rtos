/*
 * OS_Time.c
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#include "OS_Time.h"

void  OS_Delay_us
( OS_TYPE_Tick  p_Num )
{
    OS_Assert( p_Num >= 0 );

    CPU_ATOMIC_REGISTER_ALLOCATE();

    uint8   fac_us = 9;
    uint32  t_ticks;
    uint32  t_old,t_now,t_cnt = 0;
    uint32  reload = SysTick->LOAD;

    t_ticks = p_Num * fac_us;

    CPU_ATOMIC_ENTER(); //阻止ucos调度，防止打断us延时
    t_old = SysTick->VAL; //刚进入时的计数器值
    while(1)
    {
        t_now = SysTick->VAL;
        if( t_now != t_old )
        {
            if( t_now < t_old )
                t_cnt += t_old - t_now; //累加消耗掉的SysTick节拍
            else
                t_cnt += reload - t_now + t_old;

            t_old = t_now;

            if( t_cnt >= t_ticks )
                break; //时间到，退出.
        }
    }
    CPU_ATOMIC_EXIT();
}

void  OS_Delay_tick
( OS_TYPE_Tick  p_Tick )
{
    OS_Assert( p_Tick >=0 );

    CPU_ATOMIC_REGISTER_ALLOCATE();

    if( OS_Counter_ISRNest > 0 )
    {
        return;
    }

    CPU_ATOMIC_ENTER();
    OS_TDB_Current->TaskStatus = OS_ENUM_TaskStatus_Delay;

    OS_TDB_Current->TaskTick_ExceptTick = OS_SystemTick_StandardValue + p_Tick;
    OS_TDB_Current->TaskTick_RemainTick = p_Tick;

    OS_TickList_Insert( OS_TDB_Current );

    OS_ReadyList_Remove( OS_TDB_Current );
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}

void  OS_Delay
( uint8     p_hour,
  uint8     p_minute,
  uint8     p_second,
  uint16    p_milli )
{
    OS_Assert( p_hour >= 0 && p_hour <= 99 );
    OS_Assert( p_minute >= 0 && p_minute <= 59 );
    OS_Assert( p_second >= 0 && p_second <= 59 );
    OS_Assert( p_milli >= 0 && p_milli <= 999 );

    CPU_ATOMIC_REGISTER_ALLOCATE();

    OS_TYPE_Tick        t_Tick;

    if( OS_Counter_ISRNest > 0 )
    {
        return;
    }

    /* Have a tick if a milli-second number over half of 1/OS_CONFIG_SystemTick_HZ */
    t_Tick = ( p_hour * 3600 + p_minute * 60 + p_second ) * OS_CONFIG_SystemTick_HZ
              + ( OS_CONFIG_SystemTick_HZ * (p_milli +  500/OS_CONFIG_SystemTick_HZ) ) / 1000;

    CPU_ATOMIC_ENTER();
    OS_TDB_Current->TaskStatus = OS_ENUM_TaskStatus_Delay;

    OS_TDB_Current->TaskTick_ExceptTick = OS_SystemTick_StandardValue + t_Tick;
    OS_TDB_Current->TaskTick_RemainTick = t_Tick;

    OS_TickList_Insert( OS_TDB_Current );

    OS_ReadyList_Remove( OS_TDB_Current );
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}

