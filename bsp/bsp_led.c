
#include "bsp_led.h"


void  BSP_LED_init( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE ); //enable the clock of PB and PE ports

	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_5; //LED0-->PB.5
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP; //poll-pull output
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz; //ensure the IO speed is 50MHz

	GPIO_Init( GPIOB, &GPIO_InitStructure ); //the actual initializing job for GPIOB.5
	GPIO_SetBits( GPIOB, GPIO_Pin_5 ); //PB.5 reset output is HIGH

	GPIO_Init( GPIOE, &GPIO_InitStructure ); //the actual initializing job for GPIOE.5
	GPIO_SetBits( GPIOE, GPIO_Pin_5 ); //PE.5 reset output is HIGH

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1; //LED0-->PB.5
    GPIO_Init( GPIOB, &GPIO_InitStructure ); //the actual initializing job for GPIOE.5
    GPIO_ResetBits( GPIOB, GPIO_Pin_1 ); //PA.0 reset output is HIGH
}

void  BSP_LED( uint8    p_LED_number, uint8   p_state )
{
    OS_Assert( IS_LED_ID( p_LED_number ) );
    OS_Assert( p_state == BSP_LED_STATUS_OPEN || p_state == BSP_LED_STATUS_CLOSE);

    switch( p_LED_number )
	{
        case BSP_LED_1:
            if( p_state == BSP_LED_STATUS_OPEN )
            {
                GPIO_ResetBits( GPIOB, GPIO_Pin_5);
            }
            else
            {
                GPIO_SetBits( GPIOB, GPIO_Pin_5);
            }
            break;

        case BSP_LED_2:
            if( p_state == BSP_LED_STATUS_OPEN )
            {
                GPIO_ResetBits( GPIOE, GPIO_Pin_5 );
            }
            else
            {
                GPIO_SetBits( GPIOE, GPIO_Pin_5 );
            }
            break;

        case BSP_LED_1 | BSP_LED_2:
            if( p_state == BSP_LED_STATUS_OPEN )
            {
                GPIO_ResetBits( GPIOB, GPIO_Pin_5);
                GPIO_ResetBits( GPIOE, GPIO_Pin_5 );
            }
            else
            {
                GPIO_SetBits( GPIOB, GPIO_Pin_5);
                GPIO_SetBits( GPIOE, GPIO_Pin_5 );
            }
            break;

        default:
            while(1);
	}
}

void BSP_LED_reverse(uint8 p_LED_num)
{
    OS_Assert( IS_LED_ID( p_LED_num ) );

    switch( p_LED_num )
    {
        case BSP_LED_1:
            if( GPIO_ReadOutputDataBit( GPIOB, GPIO_Pin_5) )
            {
                GPIO_ResetBits( GPIOB, GPIO_Pin_5);
            }
            else
            {
                GPIO_SetBits( GPIOB, GPIO_Pin_5);
            }
            break;

        case BSP_LED_2:
            if( GPIO_ReadOutputDataBit( GPIOE, GPIO_Pin_5) )
            {
                GPIO_ResetBits( GPIOE, GPIO_Pin_5 );
            }
            else
            {
                GPIO_SetBits( GPIOE, GPIO_Pin_5 );
            }
            break;

        case BSP_LED_3:
            if( GPIO_ReadOutputDataBit( GPIOB, GPIO_Pin_5) )
            {
                GPIO_ResetBits( GPIOB, GPIO_Pin_2 );
            }
            else
            {
                GPIO_SetBits( GPIOB, GPIO_Pin_2 );
            }
            break;

        default:
            while(1);
    }
}

