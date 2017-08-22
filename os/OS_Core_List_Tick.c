/*
 *
 */

#include "OS.h"

static OS_STRUCT_List   OS_TickWheel[OS_CONFIG_TickWheelSize];

void  OS_TickList_Init()
{
    uint8   i;
    OS_STRUCT_List      *t_List;

    for( i = 0; i < OS_CONFIG_TickWheelSize; i++ )
    {
        t_List = &OS_TickWheel[i];
        t_List->Type = OS_ENUM_ListType_TickList;
        OS_List_Init( t_List );
    }
}

void  OS_TickList_Update()
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    uint8           t_IsDone = 0;
    OS_STRUCT_TDB   *t_TDB;
    OS_STRUCT_TDB   *t_TDB_next;

    CPU_ATOMIC_ENTER();
    OS_SystemTick_StandardValue++; /* Keep track of the number of system ticks */

    t_TDB = OS_TickWheel[OS_SystemTick_StandardValue % OS_CONFIG_TickWheelSize].HeadTDB;

    while( t_IsDone == 0 && t_TDB != 0 )
    {
        t_TDB_next = t_TDB->TaskListLinker[OS_ENUM_ListType_TickList].NextTDB;

        switch( t_TDB->TaskStatus )
        {
        case OS_ENUM_TaskStatus_Delay:
            t_TDB->TaskTick_RemainTick = t_TDB->TaskTick_ExceptTick - OS_SystemTick_StandardValue;

            if( OS_SystemTick_StandardValue >= t_TDB->TaskTick_ExceptTick ) /* Process each task that expires */
            {
                t_TDB->TaskStatus = OS_ENUM_TaskStatus_Ready;
                OS_TickList_Remove( t_TDB );
                OS_ReadyList_Insert( t_TDB );
            }
            else
            {
                t_IsDone = 1; /* Don't find a match, we're done */
            }
            break;

        default:
            while(1)
                ;
            break;
        }
        t_TDB = t_TDB_next;
    }
    CPU_ATOMIC_EXIT();
}

void  OS_TickList_Insert
( OS_STRUCT_TDB     *p_TDB )
{
    OS_Assert( p_TDB != 0 );

    OS_TDB_Current->TaskTick_Spoke = &OS_TickWheel[OS_TDB_Current->TaskTick_ExceptTick % OS_CONFIG_TickWheelSize];

    OS_List_Insert( p_TDB, OS_TDB_Current->TaskTick_Spoke );
}

void  OS_TickList_Remove
( OS_STRUCT_TDB     *p_TDB )
{
    OS_Assert( p_TDB != 0 );

    OS_List_Remove( p_TDB, p_TDB->TaskTick_Spoke );
}

