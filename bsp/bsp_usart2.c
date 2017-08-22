
#include "bsp_usart.h"


extern OS_STRUCT_TDB    USART2_task_TDB;

static void  USART2_interrupt_handler( void );

uint8   BSP_USART2_buffer[BSP_DEF_CONFIG_USART2_RECEIVE_BUFFER_SIZE];
uint16  BSP_USART2_char_counter = 0;

int8  BSP_usart2_fputc( uint8     ch )
{
    USART1->SR;
    USART_SendData( USART2, ch );
    while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET )
    {
        ;
    }
    return ch;
}

/**
 * @brief  We use this function to initialize the uart 2
 *          and related IO PA.2 and PA.3 .
 *
 * @note   None
 * @param  uint32 p_bound_rate: The baud rate we need
 * @retval int : if success ,return 0.
 */
int8  BSP_USART2_init( uint32   p_bound_rate )
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

    USART_DeInit( USART2 ); //reset the usart 2

    //USART2_TX ---> PA.2
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP; //multiplexing push-pull
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //USART2_RX ---> PA.3
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING; //floating output
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //make sure the parameters of USART2
    USART_InitStructure.USART_BaudRate              = p_bound_rate; //general we use 9600
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;
    USART_InitStructure.USART_Parity                = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USART2, &USART_InitStructure );

    //Usart2 NVIC initializing
    NVIC_InitStructure.NVIC_IRQChannel                      = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //enable the interrupt
    USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );

    BSP_interrupt_handler_register( BSP_INT_ID_USART2, USART2_interrupt_handler );

    //enable the uart 1
    USART_Cmd( USART2, ENABLE );

    return 0;
}

/**
 * @brief  This function handles uart 2 interrupt request.
 * @param  None
 * @retval None
 */
static void  USART2_interrupt_handler( void )
{
    uint8           ch;

    //the interrupt of receive and the data received must be end of 0x0D 0x0A
    if( USART_GetITStatus(USART2, USART_IT_RXNE) == SET )
    {
        ch = USART_ReceiveData( USART2 ); //UART2->DR

        if( BSP_USART2_char_counter >= BSP_DEF_CONFIG_USART2_RECEIVE_BUFFER_SIZE )
        {
            return;
        }
        BSP_USART2_buffer[BSP_USART2_char_counter] = ch;
        BSP_USART2_char_counter++;

        if( BSP_USART2_buffer[BSP_USART2_char_counter-1] == '\n' || BSP_USART2_buffer[BSP_USART2_char_counter-1] == '\r' )
        {
            BSP_USART2_buffer[--BSP_USART2_char_counter] = 0;
            OS_TaskMessage_Post( &USART2_task_TDB, BSP_USART2_buffer, BSP_USART2_char_counter );
            BSP_USART2_char_counter = 0;
        }

        /*Our program have a overrun error,that is, we do not use
         the data in RDR register and a next data have been go in.*/
        if( USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET )
        {
            USART_ClearFlag( USART2, USART_FLAG_ORE ); //读SR
            (void) USART_ReceiveData( USART2 ); //读DR
        }

//      If we need show back our char
//        BSP_fputc( ch );
    }
}
