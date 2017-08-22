
#include "bsp.h"

static uint8 AT2CXX_test_buffer[] = { "IIC WRITE TEST, PLEASE CHECK..." };

void  BSP_init( void )
{
    uint8   temp[sizeof(AT2CXX_test_buffer)];

    BSP_interrupt_handler_init();

    BSP_LED_init();

    BSP_USART1_init( 9600 );
    OS_Delay( 0, 0, 0, 100 );
    OS_printf( LIB_DEF_IO_OUT_USART1, "USART1 Check OK \n");

    BSP_USART2_init( 9600 );
    OS_Delay( 0, 0, 0, 100 );
    OS_printf( LIB_DEF_IO_OUT_USART1, "USART2 Check OK \n");

    AT24CXX_init();
#if 0
    while(1)
    {
        OS_printf( LIB_DEF_IO_OUT_USART1, "24C02 0xFF address value : %d\n", AT24CXX_read_one_byte( 255 ) );
        AT24CXX_write_one_byte( 255, 55 );

        OS_printf( LIB_DEF_IO_OUT_USART1, "24C02 0xFF address value : %d\n", AT24CXX_read_one_byte( 255 ) );
        AT24CXX_write_one_byte( 255, 100 );
        OS_Delay( 0, 0, 1, 0, &err );
    }
#endif
    OS_printf( LIB_DEF_IO_OUT_USART1, "24C02 Connect check ...\t\t");
    while( AT24CXX_check() ) // Testing if the EEPROM 24C02 connected
    {
    	uint8 is_print = 0;
    	if(!is_print)
    	{
    	    OS_printf( LIB_DEF_IO_OUT_USART1, "Failed, Please check !\n24C02 Connect check ...\t\t");
    	    is_print = 1;
    	}

        BSP_LED( BSP_LED_1 | BSP_LED_2, BSP_LED_STATUS_OPEN );
        OS_Delay( 0, 0, 0, 100 );
        BSP_LED( BSP_LED_1 | BSP_LED_2, BSP_LED_STATUS_CLOSE );
        OS_Delay( 0, 0, 0, 100 );
    }
    OS_printf( LIB_DEF_IO_OUT_USART1, "OK\n");

    OS_printf( LIB_DEF_IO_OUT_USART1, "24C02 Write test begin ...\t\t");
    AT24CXX_write( 0, AT2CXX_test_buffer, sizeof(AT2CXX_test_buffer) );
    OS_printf( LIB_DEF_IO_OUT_USART1, "done\n");

    AT24CXX_read( 0, temp, sizeof(AT2CXX_test_buffer) );
    OS_printf( LIB_DEF_IO_OUT_USART1, "24C02 Read test check value : %s\n", temp );
    OS_printf( LIB_DEF_IO_OUT_USART1, "24C02 All check OK \n");

    OS_printf( LIB_DEF_IO_OUT_USART1, "MPU6050 Connect check ...\t\t");
    while( !MPU6050_testConnection() ) //检查MPU6050设备是否正确连接
    {
        uint8 is_print = 0;
        if(!is_print)
        {
            OS_printf( LIB_DEF_IO_OUT_USART1, "Failed, Please check !\nMPU6050 Connect check ...\t\t");
            is_print = 1;
        }

        BSP_LED( BSP_LED_1 | BSP_LED_2, BSP_LED_STATUS_OPEN );
        OS_Delay( 0, 0, 0, 100 );
        BSP_LED( BSP_LED_1 | BSP_LED_2, BSP_LED_STATUS_CLOSE );
        OS_Delay( 0, 0, 0, 100 );
    }
    OS_printf( LIB_DEF_IO_OUT_USART1, "OK\n");

    MPU6050_initialize();
    OS_printf( LIB_DEF_IO_OUT_USART1, "MPU6050 All check OK \n");

    OS_printf( LIB_DEF_IO_OUT_USART1, "TIM3 Initialize check ...\t\t");
    TIM3_PWM_Init(8999, 0);  // PWM初始化  TIM3-CH2 and TIM3-CH3  72000KHz / 9000 = 8KHz
    TIM_SetCompare1(TIM3, 0);
    TIM_SetCompare2(TIM3, 0);
    OS_printf( LIB_DEF_IO_OUT_USART1, "OK\n");

#if 0
    while( ! MPU6050_testConnection() ) //检查MPU6050设备是否正确连接
    {
        OS_printf( LIB_DEF_IO_OUT_USART1, "MPU6050 fail ! \n");
        delay_us(100000);
    }
/*
    BSP_LED( BSP_LED_1, BSP_LED_STATUS_OPEN );
    OS_Delay( 0, 0, 1, 0, &err );
    BSP_LED( BSP_LED_1, BSP_LED_STATUS_CLOSE );
    OS_Delay( 0, 0, 1, 0, &err );
*/
    MPU6050_initialize();
    OS_printf( LIB_DEF_IO_OUT_USART1, "MPU6050 OK \n");

    MPU6050_InitGyro_Offset();//自动校准
    OS_printf( LIB_DEF_IO_OUT_USART1, "MPU6050 calibration OK \n");
#endif

}


