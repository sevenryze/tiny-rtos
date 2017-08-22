
#include "bsp_IIC.h"


void  IIC_init( void )
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP ; //push-pull output
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_SetBits( GPIOB, GPIO_Pin_10 | GPIO_Pin_11 ); //When the bus is idle, both SDA and SCL are HIGH
}


/*
 * Generate the START signal of IIC bus
 */
void  IIC_start( void )
{
    SDA_OUT(); // be sure the SDA is output mode

    IIC_SDA = 1;
    IIC_SCL = 1;
    OS_Delay_us(5);

    IIC_SDA = 0; // START signal : SDA change form HIGH to LOW, When SCL is HIGH
    OS_Delay_us(5);

    IIC_SCL = 0; // Vise the IIC bus casue we is preparing to send or receive data
}

/*
 * Generate the STOP signal of IIC bus
 */
void  IIC_stop( void )
{
    SDA_OUT(); // Be sure the SDA is output mode

    IIC_SDA = 0;
    IIC_SCL = 1; // STOP signal : SDA change form LOW to HIGH, When SCL is HIGH
    OS_Delay_us(5);

    IIC_SDA = 1;
    OS_Delay_us(5);
}

/*
 * Send a ACK signal
 */
void  IIC_send_ACK( void )
{
    SDA_OUT();

    IIC_SDA = 0;
    IIC_SCL = 1;
    OS_Delay_us(5);

    IIC_SCL = 0;
}

/*
 * Do send a nACK signal
 */
void  IIC_send_nACK( void )
{
    SDA_OUT();

    IIC_SDA = 1;
    IIC_SCL = 1;
    OS_Delay_us(5);

    IIC_SCL = 0;
}

/*
 * Get the ACK signal from slave to master
 *      return 0 : Get a ACK signal
 *             1 : Overtime when we wait
 */
uint8  IIC_wait_ACK( void )
{
    uint8   ucErrTime = 0;

    SDA_IN();

    IIC_SDA = 1;
    IIC_SCL = 1;

    while( READ_SDA )
    {
        ucErrTime++;
        if( ucErrTime > 250 )
        {
            IIC_stop();
            return 1;
        }
    }

    IIC_SCL = 0;
    return 0;
}

/*
 * Send a single byte
 */
void  IIC_send_byte( uint8  p_data )
{
    uint8   i;

    SDA_OUT();

    for( i = 0; i < 8; i++ )
    {
        if( (p_data & 0x80) >> 7 ) // We send the MSB firstly, and SDA must be holding steady when SCL is HIGH
            IIC_SDA = 1;           // We can read the steady data during the SCL is HIGH
        else
            IIC_SDA = 0;
        p_data <<= 1;

        IIC_SCL = 1; // A period is 10 us ,that is 100KHz
        OS_Delay_us(5);

        IIC_SCL = 0;
    }
}


/*
 * Read a single byte
 *      @param need_ACK : If we need a ACK signal
 *                        0 means nACK and 1 means ACK
 */
uint8  IIC_read_byte( uint8 need_ACK )
{
    uint8   i;
    uint8   t_receive = 0;

    SDA_IN(); // Be sure the SDA is input mode

    for( i = 0; i < 8; i++ )
    {
        IIC_SCL = 0;
        OS_Delay_us(5); // We can get a steady data during the SCL is HIGH

        IIC_SCL = 1;
        t_receive <<= 1;
        t_receive |= READ_SDA;

        IIC_SCL = 0;
    }

    if( need_ACK )
    {
        IIC_send_ACK();
    }
    else
    {
        IIC_send_nACK();
    }

    return t_receive;
}


