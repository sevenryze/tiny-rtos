/*
 *
 */

#include "OS.h"

/*
************************************************************************************************************************
*                           Priority table
************************************************************************************************************************
*/
#define OS_DEF_PriorityTable_UnitBit    ( sizeof(OS_TYPE_CPUDataWidth) * 8 )
#define OS_DEF_PriorityTable_Size       ( (OS_CONFIG_MaxPriority - 1)/(OS_DEF_PriorityTable_UnitBit) + 1 )

static OS_TYPE_CPUDataWidth     OS_Priority_Table[OS_DEF_PriorityTable_Size];

static void  OS_Priority_Init()
{
    OS_TYPE_CPUDataWidth  i;

    for( i = 0; i < OS_DEF_PriorityTable_Size; i++ )
    {
        OS_Priority_Table[i] = 0;
    }
}

static void  OS_Priority_Insert
( OS_TYPE_Priority  p_Priority )
{
    OS_TYPE_CPUDataWidth    bit;
    OS_TYPE_CPUDataWidth    bit_nbr;
    OS_TYPE_Priority        ix;

    ix             = p_Priority / OS_DEF_PriorityTable_UnitBit;
    bit_nbr        = (OS_TYPE_CPUDataWidth)p_Priority & (OS_DEF_PriorityTable_UnitBit - 1u);
    bit            = 1u;
    bit          <<= (OS_DEF_PriorityTable_UnitBit - 1u) - bit_nbr;
    OS_Priority_Table[ix] |= bit;
}

static void  OS_Priority_Remove
( OS_TYPE_Priority  p_Priority )
{
    OS_TYPE_CPUDataWidth    bit;
    OS_TYPE_CPUDataWidth    bit_nbr;
    OS_TYPE_Priority        ix;

    ix             = p_Priority / OS_DEF_PriorityTable_UnitBit;
    bit_nbr        = (OS_TYPE_CPUDataWidth)p_Priority & (OS_DEF_PriorityTable_UnitBit - 1u);
    bit            = 1u;
    bit          <<= (OS_DEF_PriorityTable_UnitBit - 1u) - bit_nbr;
    OS_Priority_Table[ix] &= ~bit;
}

OS_TYPE_Priority  OS_Priority_GetReady()
{
    OS_TYPE_CPUDataWidth    *t_table;
    OS_TYPE_Priority        t_prio;

    t_prio  = 0;
    t_table = &OS_Priority_Table[0];

    while( *t_table == 0 )
    {
        t_prio += OS_DEF_PriorityTable_UnitBit;
        t_table ++ ;
    }
    t_prio += CPU_count_lead_zeros( *t_table ); /* Find the position of the first bit set at the entry */
    return (t_prio);
}

/*
************************************************************************************************************************
*                           Ready list
************************************************************************************************************************
*/
void  OS_ReadyList_Init()
{
    OS_TYPE_Priority    i;
    OS_STRUCT_List      *t_List;

    OS_Priority_Init(); /* Initialize the priority table */

    for( i = 0; i < OS_DEF_RealMaxPriority; i++ ) /* Initialize the ready list */
    {
        t_List = &OS_Ready_List[i];
        t_List->Type = OS_ENUM_ListType_ReadyList;
        OS_List_Init( t_List );
    }
}

void  OS_ReadyList_Insert
( OS_STRUCT_TDB     *p_TDB )
{
    OS_Assert( p_TDB != 0 );

    OS_Priority_Insert( p_TDB->TaskPriority );

    OS_List_Insert( p_TDB, &OS_Ready_List[p_TDB->TaskPriority] );
}

void  OS_ReadyList_Remove
( OS_STRUCT_TDB     *p_TDB )
{
    OS_Assert( p_TDB != 0 );

	if( OS_Ready_List[p_TDB->TaskPriority].EntryNum_Current == 1 )
	{
	    OS_Priority_Remove( p_TDB->TaskPriority );
	}

	OS_List_Remove( p_TDB, &OS_Ready_List[p_TDB->TaskPriority] );
}

