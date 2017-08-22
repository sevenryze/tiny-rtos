
#include "bsp_EEPROM.h"


//初始化IIC接口
void  AT24CXX_init( void )
{
    IIC_init();
}


//在AT24CXX指定地址读出一个数据
//p_read_address:开始读数的地址
//返回值  :读到的数据
uint8  AT24CXX_read_one_byte( uint16    p_read_address )
{
    uint8   temp;

    IIC_start();

    IIC_send_byte( 0xA0 );   //发送器件地址0xA0,写数据

    IIC_wait_ACK();

    IIC_send_byte( p_read_address );   //发送地址

    IIC_wait_ACK();

    IIC_start();

    IIC_send_byte( 0xA1 ); //进入接收模式

    IIC_wait_ACK();

    temp = IIC_read_byte( 1 );

    IIC_stop();//产生一个停止条件

    return temp;
}

//在AT24CXX指定地址写入一个数据
//p_write_address  :写入数据的目的地址
//p_data    : 要写入的数据
void  AT24CXX_write_one_byte( uint16 p_write_address, uint8 p_data )
{
    IIC_start();

    IIC_send_byte( 0xA0 );   //发送器件地址0XA0,写数据

    IIC_wait_ACK();

    IIC_send_byte( p_write_address );   //发送低地址

    IIC_wait_ACK();

    IIC_send_byte( p_data );     //发送字节

    IIC_wait_ACK();

    IIC_stop();//产生一个停止条件

    OS_Delay_us( 20000 );
}


//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
uint8  AT24CXX_check( void )
{
    uint8   temp;

    temp = AT24CXX_read_one_byte( 255 ); //避免每次开机都写AT24CXX

    if( temp == 0x5A )
        return 0;
    else //排除第一次初始化的情况
    {
        AT24CXX_write_one_byte( 255, 0x5A );
        temp = AT24CXX_read_one_byte( 255 );
        if( temp == 0x5A )
            return 0;
    }

    return 1;
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//p_write_address  :开始写入的地址
//p_data:数据
//Len        :要写入数据的长度2,4 bytes
void  AT24CXX_write_len_byte( uint16    p_write_address, uint32     p_data, uint8   Len )
{
    uint8   i;

    for( i = 0; i < Len; i++ )
    {
        AT24CXX_write_one_byte( p_write_address + i, ( p_data >> (8*i) ) & 0xFF );
    }
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//p_read_address   :开始读出的地址
//返回值 :数据
//Len   :要读出数据的长度2,4
uint32  AT24CXX_read_len_byte( uint16   p_read_address, uint8   Len )
{
    uint8   i;
    uint32  temp = 0;

    for( i = 0; i < Len; i++ )
    {
        temp <<= 8;
        temp += AT24CXX_read_one_byte( (p_read_address + Len - 1) - i );
    }
    return temp;
}


//在AT24CXX里面的指定地址开始读出指定个数的数据
//p_read_address :开始读出的地址 对24c02为0~255
//p_buffer  :数据数组首地址
//p_read_num:要读出数据的个数
void  AT24CXX_read( uint16   p_read_address, uint8   *p_buffer, uint16    p_read_num )
{
    while( p_read_num-- )
    {
        *p_buffer++ = AT24CXX_read_one_byte( p_read_address++ );
    }
}


//在AT24CXX里面的指定地址开始写入指定个数的数据
//p_write_address :开始写入的地址 对24c02为0~255
//p_buffer   :数据数组首地址
//p_write_num:要写入数据的个数
void  AT24CXX_write( uint16     p_write_address, uint8  *p_buffer, uint16   p_write_num )
{
    while( p_write_num-- )
    {
        AT24CXX_write_one_byte( p_write_address++, *p_buffer++ );
    }
}





