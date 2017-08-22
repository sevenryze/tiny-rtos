/*
 * OS_Core_List.c
 *
 *  Created on: 2014年3月26日
 *      Author: OH
 */

#include "OS.h"

void  OS_List_Init
( OS_STRUCT_List    *t_List )
{
    OS_Assert( t_List != 0 );

    t_List->HeadTDB = 0;
    t_List->TailTDB = 0;
    t_List->EntryNum_Current    = 0;
    t_List->EntryNum_Peak       = 0;
}

void  OS_List_Insert
( OS_STRUCT_TDB     *p_TDB,
  OS_STRUCT_List    *p_List )
{
    OS_Assert( p_TDB != 0 );
    OS_Assert( p_List != 0 );
    OS_Assert( p_List->Type >=0 && p_List->Type < 3 );

    OS_STRUCT_TDB   *t_TDB;

    if( p_List->EntryNum_Current == 0 ) /* First entry in the list */
    {
        p_TDB->TaskListLinker[p_List->Type].PrevTDB = 0;
        p_TDB->TaskListLinker[p_List->Type].NextTDB = 0;

        p_List->HeadTDB = p_TDB;
        p_List->TailTDB = p_TDB;
    }
    else /* More than one entries in the list */
    {
        t_TDB = p_List->HeadTDB;

        switch( p_List->Type ) /* Find the position where to insert */
        {
        case OS_ENUM_ListType_PendList:
            while( t_TDB != 0 && t_TDB->TaskPriority < p_TDB->TaskPriority )
            {
                t_TDB = t_TDB->TaskListLinker[p_List->Type].NextTDB;
            }
            break;
        case OS_ENUM_ListType_TickList:
            while( t_TDB != 0 && t_TDB->TaskTick_ExceptTick < p_TDB->TaskTick_RemainTick )
            {
                t_TDB = t_TDB->TaskListLinker[p_List->Type].NextTDB;
            }
            break;
        case OS_ENUM_ListType_ReadyList:
            t_TDB = 0;
            break;
        default:
            break;
        }

        if( t_TDB == 0 ) /* Insert at the tail */
        {
            t_TDB = p_List->TailTDB;

            p_TDB->TaskListLinker[p_List->Type].NextTDB = 0;
            p_TDB->TaskListLinker[p_List->Type].PrevTDB = t_TDB;

            t_TDB->TaskListLinker[p_List->Type].NextTDB = p_TDB;
            p_List->TailTDB                             = p_TDB;
        }
        else
        {
            if( t_TDB->TaskListLinker[p_List->Type].PrevTDB == 0 ) /* Insert at the head */
            {
                t_TDB = p_List->HeadTDB;

                p_TDB->TaskListLinker[p_List->Type].NextTDB = t_TDB;
                p_TDB->TaskListLinker[p_List->Type].PrevTDB = 0;

                t_TDB->TaskListLinker[p_List->Type].PrevTDB = p_TDB;
                p_List->HeadTDB                             = p_TDB;
            }
            else /* Insert in between two entries */
            {
                p_TDB->TaskListLinker[p_List->Type].PrevTDB = t_TDB->TaskListLinker[p_List->Type].PrevTDB;
                p_TDB->TaskListLinker[p_List->Type].NextTDB = t_TDB;
                t_TDB->TaskListLinker[p_List->Type].PrevTDB->TaskListLinker[p_List->Type].NextTDB = p_TDB;
                t_TDB->TaskListLinker[p_List->Type].PrevTDB = p_TDB;
            }
        }
    }
    p_List->EntryNum_Current++;

    if( p_List->EntryNum_Peak < p_List->EntryNum_Current )
    {
        p_List->EntryNum_Peak = p_List->EntryNum_Current;
    }
}

void  OS_List_Remove
( OS_STRUCT_TDB     *p_TDB,
  OS_STRUCT_List    *p_List )
{
    OS_Assert( p_TDB != 0 );
    OS_Assert( p_List != 0 );
    OS_Assert( p_List->Type >=0 && p_List->Type < 3 );

    if( p_List->EntryNum_Current == 1 ) /* Only one entry in the list */
    {
        p_List->HeadTDB = 0;
        p_List->TailTDB = 0;
    }
    else if( p_TDB->TaskListLinker[p_List->Type].PrevTDB == 0 ) /* at the head of the list */
    {
        p_TDB->TaskListLinker[p_List->Type].NextTDB->TaskListLinker[p_List->Type].PrevTDB = 0;
        p_List->HeadTDB = p_TDB->TaskListLinker[p_List->Type].NextTDB;
    }
    else if( p_TDB->TaskListLinker[p_List->Type].NextTDB == 0 ) /* at the tail of the list */
    {
        p_TDB->TaskListLinker[p_List->Type].PrevTDB->TaskListLinker[p_List->Type].NextTDB = 0;
        p_List->TailTDB = p_TDB->TaskListLinker[p_List->Type].PrevTDB;
    }
    else /* Remove from inside the list */
    {
        p_TDB->TaskListLinker[p_List->Type].PrevTDB->TaskListLinker[p_List->Type].NextTDB = p_TDB->TaskListLinker[p_List->Type].NextTDB;
        p_TDB->TaskListLinker[p_List->Type].NextTDB->TaskListLinker[p_List->Type].PrevTDB = p_TDB->TaskListLinker[p_List->Type].PrevTDB;
    }
    p_List->EntryNum_Current--;

    p_TDB->TaskListLinker[p_List->Type].PrevTDB = 0;
    p_TDB->TaskListLinker[p_List->Type].NextTDB = 0;
}



