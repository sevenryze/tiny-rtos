/*
 * OS_Port_TaskStack_Init.c
 *
 *  Created on: 2014年3月22日
 *      Author: OH
 */

#include "OS_Port.h"


static void  OS_TaskReturn( void )
{
    while(1);
}

OS_TYPE_StackUnit  *OS_TaskStack_Init
( OS_TYPE_TaskEntry    p_TaskEntry,
  OS_TYPE_StackUnit    *p_StackBase,
  OS_TYPE_StackSize    p_StackSize )
{
    OS_TYPE_StackUnit   *t_SP;
    OS_TYPE_StackSize   i;

    /* Clear the task's stack */
    t_SP = p_StackBase;
    for( i = 0; i < p_StackSize; i++ ) /* Stack grows from HIGH to LOW memory */
    {
        *t_SP = 0;
        t_SP -- ;
    }

    /* Initialize the stack frame of the task, which we used for the first content switch */
    t_SP = p_StackBase;

                                /* Registers stacked as if auto-saved on exception */
    *--t_SP = (OS_TYPE_StackUnit)0x01000000u;       /* xPSR */
    *--t_SP = (OS_TYPE_StackUnit)p_TaskEntry;       /* Entry Point */
    *--t_SP = (OS_TYPE_StackUnit)OS_TaskReturn;     /* R14 (LR) */
    *--t_SP = (OS_TYPE_StackUnit)0x12121212u;       /* R12 */
    *--t_SP = (OS_TYPE_StackUnit)0x03030303u;       /* R3 */
    *--t_SP = (OS_TYPE_StackUnit)0x02020202u;       /* R2 */
    *--t_SP = (OS_TYPE_StackUnit)p_StackBase;       /* R1 */
    *--t_SP = (OS_TYPE_StackUnit)0;                 /* R0 : argument */

                                /* Remaining registers saved on task stack */
    *--t_SP = (OS_TYPE_StackUnit)0x11111111u;       /* R11 */
    *--t_SP = (OS_TYPE_StackUnit)0x10101010u;       /* R10 */
    *--t_SP = (OS_TYPE_StackUnit)0x09090909u;       /* R9 */
    *--t_SP = (OS_TYPE_StackUnit)0x08080808u;       /* R8 */
    *--t_SP = (OS_TYPE_StackUnit)0x07070707u;       /* R7 */
    *--t_SP = (OS_TYPE_StackUnit)0x06060606u;       /* R6 */
    *--t_SP = (OS_TYPE_StackUnit)0x05050505u;       /* R5 */
    *--t_SP = (OS_TYPE_StackUnit)0x04040404u;       /* R4 */

    return (t_SP);
}



