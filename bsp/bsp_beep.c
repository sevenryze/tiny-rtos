
#include "bsp_beep.h"


void  BSP_beep_init( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE ); //enable the clock of PB ports

	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8; //BEEP-->PB.8
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP; //poll-pull output
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz; //ensure the IO speed is 50MHz
	GPIO_Init( GPIOB, &GPIO_InitStructure ); //the actual initializing job for GPIOB.8
	GPIO_ResetBits( GPIOB, GPIO_Pin_8 ); //PB.8 output is HIGH
}

void  BSP_beep( uint8  p_status )
{
    OS_Assert( p_status == BSP_BEEP_STATUS_OPEN || p_status == BSP_BEEP_STATUS_CLOSE );

    switch( p_status )
    {
        case BSP_BEEP_STATUS_OPEN:
            GPIO_SetBits( GPIOB, GPIO_Pin_8 );
            break;

        case BSP_BEEP_STATUS_CLOSE:
            GPIO_ResetBits( GPIOB, GPIO_Pin_8 );
            break;

        default:
            while(1);
    }
}


