
#include "OS.h"

OS_STRUCT_TDB        show_run_task_TDB;
OS_TYPE_StackUnit   show_run_task_stack[250];
void show_run_task();

OS_STRUCT_TDB        USART1_task_TDB;
OS_TYPE_StackUnit   USART1_task_stack[128];
void  USART1_task();

OS_STRUCT_TDB        USART2_task_TDB;
OS_TYPE_StackUnit   USART2_task_stack[128];
void  USART2_task();

OS_STRUCT_TDB        MOVE_task_TDB;
OS_TYPE_StackUnit   MOVE_task_stack[128];
void  MOVE_task();

OS_STRUCT_TDB        MPU6050_task_TDB;
OS_TYPE_StackUnit   MPU6050_task_stack[128];
void  MPU6050_task();

OS_STRUCT_Semaphore     USART2_sem;

int main()
{
    OS_Init();

    OS_Semaphore_Create( &USART2_sem, 1 );

    OS_Task_Create( &show_run_task_TDB,
    				show_run_task,
    				3,
    				&show_run_task_stack[0],
    				250,
    				10 );

    OS_Start();

    /* Should never get here! */
    while(1);
    return 0;
}

void show_run_task()
{
    BSP_init();

    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 ); //We have 4 pre-emption priorities and 4 subpriorities

    OS_Task_Create( &USART1_task_TDB,
                    USART1_task,
                    4,
                    &USART1_task_stack[0],
                    128,
                    10);

    OS_Task_Create( &USART2_task_TDB,
                    USART2_task,
                    5,
                    &USART2_task_stack[0],
                    128,
                    10);

    OS_Task_Create( &MOVE_task_TDB,
                    MOVE_task,
                    6,
                    &MOVE_task_stack[0],
                    128,
                    10);

    OS_Task_Create( &MPU6050_task_TDB,
                    MPU6050_task,
                    7,
                    &MPU6050_task_stack[0],
                    128,
                    0 );

    while(1)
    {
        BSP_LED( BSP_LED_1, BSP_LED_STATUS_OPEN );
        OS_Delay( 0, 0, 1, 0 );
        BSP_LED( BSP_LED_1, BSP_LED_STATUS_CLOSE );
        OS_Delay( 0, 0, 1, 0 );
    }
}

void  USART1_task()
{
    uint32  t_message_size;
    uint8   *t_void;

    while(1)
    {
        t_void = OS_TaskMessage_Pend( &t_message_size);

        OS_printf( LIB_DEF_IO_OUT_USART1, "USART1 strings : %s ( %d )\n", t_void, t_message_size );
    }
}

void  USART2_task()
{
    uint32  t_message_size;
    uint8   *t_void;

    while(1)
    {
        t_void = OS_TaskMessage_Pend( &t_message_size );

        if( *t_void == 'M' && *(t_void+1) == ':' )
        {
            OS_TaskMessage_Post( &MOVE_task_TDB, t_void + 2, 1 );
        }
        else if( *t_void == 'G' && *(t_void+1) == '~' )
        {
            OS_TaskSemaphore_Post( &MPU6050_task_TDB );
        }
        else
        {
            OS_Semaphore_Pend( &USART2_sem );
            OS_printf( LIB_DEF_IO_OUT_USART2, "USART2 strings : %s\n", t_void );
            OS_Semaphore_Post( &USART2_sem );
        }
    }
}

void  MOVE_task()
{
    uint32  t_message_size;
    uint8   *t_void;

    uint32  l_speed = 0;
    uint32  r_speed = 0;

    uint8   is_single = 0;

    while(1)
    {
        t_void = OS_TaskMessage_Pend( &t_message_size );

        if(is_single == 0)
        {
            GPIO_SetBits( GPIOB, GPIO_Pin_1 ); //PA.0
            is_single = 1;
        }
        else
        {
            GPIO_ResetBits( GPIOB, GPIO_Pin_1 ); //PA.0
            is_single = 0;
        }

        if( t_message_size != 1 )
        {
            continue;
        }
        else
        {
            switch( *t_void )
            {
                case 'W':
                    if( l_speed <= 8000 )
                    {
                        l_speed += 500;
                        TIM_SetCompare1(TIM3, l_speed);
                    }
                    if( r_speed <= 8000 )
                    {
                        r_speed += 500;
                        TIM_SetCompare2(TIM3, r_speed);
                    }
                    break;

                case 'S':
                    if( l_speed >= 500 )
                    {
                        l_speed -= 500;
                        TIM_SetCompare1(TIM3, l_speed);
                    }
                    if( r_speed >= 500 )
                    {
                        r_speed -= 500;
                        TIM_SetCompare2(TIM3, r_speed);
                    }
                    break;

                case 'Q':
                    if( l_speed <= 8000 )
                    {
                        l_speed += 500;
                        TIM_SetCompare1(TIM3, l_speed);
                    }
                    break;

                case 'E':
                    if( r_speed <= 8000 )
                    {
                        r_speed += 500;
                        TIM_SetCompare2(TIM3, r_speed);
                    }
                    break;

                case 'A':
                    if( l_speed >= 500 )
                    {
                        l_speed -= 500;
                        TIM_SetCompare1(TIM3, l_speed);
                    }
                    break;

                case 'D':
                    if( r_speed >= 500 )
                    {
                        r_speed -= 500;
                        TIM_SetCompare2(TIM3, r_speed);
                    }
                    break;

                default:
                    break;
            }
            OS_Semaphore_Pend( &USART2_sem );
            OS_printf( LIB_DEF_IO_OUT_USART2, "M:%d %d\n", l_speed, r_speed );
            OS_Semaphore_Post( &USART2_sem );
        }
    }
}

int16 ax, ay, az;
int16 gx, gy, gz;

int32 aax,aay,aaz; //单位 g(9.8m/s^2)
int32 faax,faay,faaz;
int32 ggx,ggy,ggz; //单位 °/S

void  MPU6050_task()
{
    while(1)
    {
        OS_Delay( 0, 0, 0, 500 );

        MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
/*
        aax = ax/16384;
        aay = ay/16384;
        aaz = az/16384;

        faax = ax % 16384;
        faay = ay % 16384;
        faaz = az % 16384;
        aax: %d  aay: %d  aaz: %d  faax: %d  faay: %d  faaz: %d  aax, aay, aaz, faax, faay, faaz,
*/
        ggx = gx/131;
        ggy = gy/131;
        ggz = gz/131;

        OS_Semaphore_Pend( &USART2_sem );
        OS_printf( LIB_DEF_IO_OUT_USART2 , "G:%d %d %d\n", ggx, ggy, ggz );
        OS_Semaphore_Post( &USART2_sem );
    }
}


