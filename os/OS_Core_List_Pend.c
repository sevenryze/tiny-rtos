/*
 *
 */

#include "OS.h"


void  OS_PendList_Init
( OS_STRUCT_List    *t_List )
{
    OS_Assert( t_List != 0 );

    t_List->Type = OS_ENUM_ListType_PendList;
    OS_List_Init( t_List );
}

void  OS_PendList_Insert
( OS_STRUCT_TDB     *p_TDB )
{
    OS_Assert( p_TDB != 0 );

    OS_List_Insert( p_TDB, &p_TDB->TaskPend_Tool->PendList );
}

void  OS_PendList_Remove
( OS_STRUCT_TDB     *p_TDB )
{
    OS_Assert( p_TDB != 0 );

    OS_List_Remove( p_TDB, &p_TDB->TaskPend_Tool->PendList );
}
