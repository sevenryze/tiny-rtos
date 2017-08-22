/*
 * bsp_IIC.h
 *
 *  Created on: 2014年3月5日
 *      Author: Rare
 */

#ifndef BSP_IIC_H_
#define BSP_IIC_H_

#include "bsp.h"


//Set the SDA's IO directon
#define SDA_IN()    { GPIOB->CRH &= 0XFFFF0FFF;   GPIOB->CRH |= 8<<12; }
#define SDA_OUT()   { GPIOB->CRH &= 0XFFFF0FFF;   GPIOB->CRH |= 3<<12; }


//Operate the IO
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA
#define READ_SDA   PBin(11)  //输入SDA

void  IIC_init( void );
void  IIC_start( void );
void  IIC_stop( void );
uint8  IIC_wait_ACK( void );
void  IIC_send_ACK( void );
void  IIC_send_nACK(void);
void  IIC_send_byte( uint8  p_data );
uint8  IIC_read_byte( uint8 need_ACK );









#endif /* BSP_IIC_H_ */
