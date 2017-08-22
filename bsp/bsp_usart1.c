
#include "bsp_usart.h"


extern OS_STRUCT_TDB    USART1_task_TDB;

static void  USART1_interrupt_handler( void );

uint8   BSP_USART1_buffer[BSP_DEF_CONFIG_USART1_RECEIVE_BUFFER_SIZE];
uint16  BSP_USART1_char_counter = 0;

int8  BSP_usart1_fputc( uint8     ch )
{
    USART1->SR;
    USART_SendData( USART1, ch );
    while( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET )
    {
        ;
    }
    return ch;
}

/**
 * @brief  We use this function to initialize the uart 1
 *      and related IO PA.9 and PA.10 .
 *
 * @note   None
 * @param  uint32_t p_bound_rate: The baud rate we need
 * @retval int : if success ,return 0.
 */
int8  BSP_USART1_init( uint32    p_bound_rate )
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );

    USART_DeInit( USART1 ); //reset the uart 1

    //USART1_TX ---> PA.9
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP; //multiplexing push-pull
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //USART1_RX ---> PA.10
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING; //floating output
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //make sure the parameters of USART1
    USART_InitStructure.USART_BaudRate              = p_bound_rate; //general we use 9600
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;
    USART_InitStructure.USART_Parity                = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USART1, &USART_InitStructure );

    //Usart1 NVIC initializing
    NVIC_InitStructure.NVIC_IRQChannel                      = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //enable the interrupt
    USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );

    BSP_interrupt_handler_register( BSP_INT_ID_USART1, USART1_interrupt_handler );

    //enable the uart 1
    USART_Cmd( USART1, ENABLE );

    return 0;
}

/**
 * @brief  This function handles uart 1 interrupt request.
 * @param  None
 * @retval None
 */
static void  USART1_interrupt_handler( void )
{
    uint8           ch;

    //the interrupt of receive and the data received must be end of 0x0D 0x0A
    if( USART_GetITStatus(USART1, USART_IT_RXNE) == SET )
    {
        ch = USART_ReceiveData( USART1 ); //UART1->DR

        if( BSP_USART1_char_counter >= BSP_DEF_CONFIG_USART1_RECEIVE_BUFFER_SIZE )
        {
            return;
        }
        BSP_USART1_buffer[BSP_USART1_char_counter] = ch;
        BSP_USART1_char_counter++;

        if( BSP_USART1_buffer[BSP_USART1_char_counter-1] == '\n' || BSP_USART1_buffer[BSP_USART1_char_counter-1] == '\r' )
        {
            BSP_USART1_buffer[--BSP_USART1_char_counter] = 0;
            OS_TaskMessage_Post( &USART1_task_TDB, BSP_USART1_buffer, BSP_USART1_char_counter );
            BSP_USART1_char_counter = 0;
        }

        /*Our program have a overrun error,that is, we do not use
         the data in RDR register and a next data have been go in.*/
        if( USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET )
        {
            USART_ClearFlag( USART1, USART_FLAG_ORE ); //读SR
            (void) USART_ReceiveData( USART1 ); //读DR
        }

//      If we need show back our char
//        BSP_fputc( ch );
    }
}
