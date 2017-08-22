
#include "bsp_mpu6050.h"

u8 buffer[14];

s16 MPU6050_FIFO[6][11];
s16 Gx_offset=0,Gy_offset=0,Gz_offset=0;
float Acc1G_Values;

/**
  * @brief: 读取指定设备指定寄存器的一个值
    * @param: I2C_Addr: 目标设备地址
    * @param: add     : 寄存器地址
    * @retval: 读取到的值
  */
u8 MPU6050_ReadOneByte(u8 I2C_Addr,u8 addr)
{
    u8 res=0;

    IIC_start();
    IIC_send_byte(I2C_Addr);       //发送写命令
    res++;
    IIC_wait_ACK();
    IIC_send_byte(addr); res++;  //发送地址
    IIC_wait_ACK();
    //IIC_stop();//产生一个停止条件
    IIC_start();
    IIC_send_byte(I2C_Addr+1); res++;          //进入接收模式
    IIC_wait_ACK();
    res=IIC_read_byte(0);
    IIC_stop();//产生一个停止条件

    return res;
}

/**
  * @brief: 读取指定设备指定寄存器的 length个值
  * @param: dev: 目标设备地址
  * @param: reg: 目标寄存器地址
  * @param: length: 要读取的字节数
  * @param: *data: 读出来数据将存放的地址指针
  * @retval: 读取出的数据数目
  */
u8 MPU6050_readBytes(u8 dev, u8 reg, u8 length, u8 *data)
{
    u8 count = 0;

    IIC_start();
    IIC_send_byte(dev);     //发送写命令
    IIC_wait_ACK();
    IIC_send_byte(reg);     //发送地址
    IIC_wait_ACK();
    IIC_start();
    IIC_send_byte(dev+1);   //进入接收模式
    IIC_wait_ACK();

    for(count=0;count<length;count++)
    {
        if(count!=length-1)data[count]=IIC_read_byte(1);    //带ACK的读数据
        else  data[count]=IIC_read_byte(0);                             //最后一个字节NACK
    }
    IIC_stop();//产生一个停止条件
    return count;
}

/**
  * @brief: 将多个字节写入指定设备指定寄存器
  * @param:    dev: 目标设备地址
  * @param:    reg: 寄存器地址
  * @param: length: 要写的字节数
  * @param:  *data: 将要写的数据的首地址
  * @retval: 写入的字节数
  */
u8 MPU6050_writeBytes(u8 dev, u8 reg, u8 length, u8* data)
{
    u8 count = 0;
    IIC_start();
    IIC_send_byte(dev);    //发送写命令
    IIC_wait_ACK();
    IIC_send_byte(reg);   //发送地址
  IIC_wait_ACK();
    for(count=0;count<length;count++)
    {
        IIC_send_byte(data[count]);
        IIC_wait_ACK();
    }
    IIC_stop();//产生一个停止条件

    return count;
}

/**
  * @brief: 读取指定设备指定寄存器的一个值
  * @param:   dev: 目标设备地址
  * @param:   reg: 寄存器地址
  * @param: *data: 读出的数据将要存放的地址
  * @retval: 返回伪数值 1
  */
u8 MPU6050_readByte(u8 dev, u8 reg, u8 *data)
{
    *data=MPU6050_ReadOneByte(dev, reg);
    return 1;
}

/**
  * @brief: 写入指定设备指定寄存器一个字节
  * @param:  dev: 目标设备地址
  * @param:  reg: 寄存器地址
  * @param: data: 将要写入的字节
  * @retval: 写入数据的数目，这里始终是 1
  */
u8 MPU6050_writeByte(u8 dev, u8 reg, u8 data)
{
    return MPU6050_writeBytes(dev, reg, 1, &data);
}

/**
  * @brief: 读 修改 写 指定设备 指定寄存器一个字节 中的多个位
  * @param: dev: 目标设备地址
  * @param: reg: 寄存器地址
  * @param: bitStart: 目标字节的起始位
  * @param: length: 位长度
  * @param: data: 存放改变目标字节位的值
  * @retval: 1 - 位修改成功    0 - 修改失败
  */
u8 MPU6050_writeBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
{
    u8 b;
    if (MPU6050_readByte(dev, reg, &b) != 0)
    {
        u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return MPU6050_writeByte(dev, reg, b);
    }
    else
    {
        return 0;
    }
}

/**
  * @brief: 读 修改 写 指定设备 指定寄存器一个字节 中的1个位
  * @param: dev: 目标设备地址
  * @param: reg: 寄存器地址
  * @param: bitNum: 要修改目标字节的bitNum位
  * @param: data: 为0 时，目标位将被清0 否则将被置位
  * @retval: 1 - 修改成功    0 - 修改失败
  */
u8 MPU6050_writeBit(u8 dev, u8 reg, u8 bitNum, u8 data)
{
    u8 b;
    MPU6050_readByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return MPU6050_writeByte(dev, reg, b);
}

/*---------------------------------------------------------------------------------------------------*/

/**
  * @brief: 读取三个轴加速度的总和
  * @retval: 返回三个轴加速度的总和
  */
float MPU6050_1GValue(void)
{
    return Acc1G_Values;
}

/**
  * @brief: 将新的ADC数据更新到FIFO数组，进行滤波处理
  * @param: ax,ay,az,gx,gy,gz: 新的ADC数据
  */
void  MPU6050_newValues(s16 ax,s16 ay,s16 az,s16 gx,s16 gy,s16 gz)
{
    u8 i ;
    s32 sum=0;
    for(i=1;i<10;i++)
    {   //FIFO 操作
        MPU6050_FIFO[0][i-1]=MPU6050_FIFO[0][i];
        MPU6050_FIFO[1][i-1]=MPU6050_FIFO[1][i];
        MPU6050_FIFO[2][i-1]=MPU6050_FIFO[2][i];
        MPU6050_FIFO[3][i-1]=MPU6050_FIFO[3][i];
        MPU6050_FIFO[4][i-1]=MPU6050_FIFO[4][i];
        MPU6050_FIFO[5][i-1]=MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[0][9]=ax;//将新的数据放置到 数据的最后面
    MPU6050_FIFO[1][9]=ay;
    MPU6050_FIFO[2][9]=az;
    MPU6050_FIFO[3][9]=gx;
    MPU6050_FIFO[4][9]=gy;
    MPU6050_FIFO[5][9]=gz;

    sum=0;
    for(i=0;i<10;i++)
    {   //求当前数组的合，再取平均值
         sum+=MPU6050_FIFO[0][i];
    }
    MPU6050_FIFO[0][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[1][i];
    }
    MPU6050_FIFO[1][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[2][i];
    }
    MPU6050_FIFO[2][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[3][i];
    }
    MPU6050_FIFO[3][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[4][i];
    }
    MPU6050_FIFO[4][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[5][10]=sum/10;
}

/**
  * @brief: 设置MPU6050的时钟
  * @param: source: 时钟源
  * @note:
  * CLK_SEL | Clock Source
  * --------+--------------------------------------
  * 0       | Internal oscillator
  * 1       | PLL with X Gyro reference
  * 2       | PLL with Y Gyro reference
  * 3       | PLL with Z Gyro reference
  * 4       | PLL with external 32.768kHz reference
  * 5       | PLL with external 19.2MHz reference
  * 6       | Reserved
  * 7       | Stops the clock and keeps the timing generator in reset
  */
void MPU6050_setClockSource(u8 source)
{
    MPU6050_writeBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050_setFullScaleGyroRange(u8 range)
{
    MPU6050_writeBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

/**
  * @brief: 设置MPU6050加速度计的最大量程
  * @param: range: 最大两成
  */
void MPU6050_setFullScaleAccelRange(u8 range)
{
    MPU6050_writeBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/**
  * @brief: 设置MPU6050是否进入睡眠模式
    * @param: enabled =1   睡觉
    *                 =0   工作
  */
void MPU6050_setSleepEnabled(u8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**
  * @brief: 读取MPU6050 WHO_AM_I 标识
  * @retval: 0x68 - MPU6050 WHO_AM_I标识   其他 - 未正确读取到
  */
u8 MPU6050_getDeviceID(void)
{
    MPU6050_readBytes(devAddr, MPU6050_RA_WHO_AM_I, 1, buffer);
    return buffer[0];
}

/**
  * @brief: 检测MPU6050 是否已经连接
  * @retval: 0 - 未检测到MPU6050    1 - 检测到MPU6050
  */
u8 MPU6050_testConnection(void)
{
    if(MPU6050_getDeviceID() == 0x68)  //读取设备ID: 0b01101000;
        return 1;
    else
        return 0;
}

/**
  * @brief: 设置 MPU6050 是否为AUX I2C线的主机
  * @param: enable = 1 主机模式
  * @param:        = 0 非主机模式
  */
void MPU6050_setI2CMasterModeEnabled(u8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**
  * @brief: 设置 MPU6050 是否为AUX I2C线的BYPASS模式
  * @param: enable = 1 BYPASS模式
  *                = 0 非BYPASS模式
  */
void MPU6050_setI2CBypassEnabled(u8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

/**
  * @brief: 初始化MPU6050以进入可用状态。
  */
void MPU6050_initialize(void)
{
    s16 temp[6];
    u8 i;

    MPU6050_setClockSource(MPU6050_CLOCK_INTERNAL);         //设置时钟
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_250); //陀螺仪最大量程 +-250度每秒
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2); //加速度度最大量程 +-2G
    MPU6050_setSleepEnabled(0);                 //进入工作状态
    MPU6050_setI2CMasterModeEnabled(0); //不让MPU6050 控制AUXI2C
    MPU6050_setI2CBypassEnabled(1);         //主控制器的I2C与 MPU6050的AUXI2C  直通。控制器可以直接访问HMC5883L
    //配置MPU6050 的中断模式 和中断电平模式
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, 0);   //INT配置为高电平有效
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, 0);    //INT配置为推挽输出
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, 1);//INT pin持续为高电平知道清除
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, 1);//任意读取中断状态位，都清除中断
    //开数据转换完成中断
  MPU6050_writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, 1);  //打开测量数据准备好中断

    for(i=0;i<20;i++)
    {//更新FIFO数组
        OS_Delay_us(50);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
    }
    MPU6050_InitGyro_Offset(); //初始化陀螺仪的偏置，此时模块应该不要被移动
}

/**
  * @brief: 检查 MPU6050的中断引脚，测试是否完成转换
  * @retval: 1 - 转换完成
  *          0 - 数据寄存器还没有更新
  */
u8 MPU6050_is_DRY(void)
{
    u8 int_status = 0;

    MPU6050_readByte(devAddr,MPU6050_RA_INT_STATUS,&int_status);
    int_status &= (1<<MPU6050_INTERRUPT_DATA_RDY_BIT);

    if(int_status)
    {
        return 1;
    }
    else
        return 0;
}

s16 MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz,MPU6050_LastTemp,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz;
/**
  * @brief: 读取 MPU6050的当前测量值
  */
void MPU6050_getMotion6(s16* ax, s16* ay, s16* az, s16* gx, s16* gy, s16* gz)
{
    if(MPU6050_is_DRY())
    {
        //加速度测量值
        MPU6050_readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    MPU6050_Lastax=(((s16)buffer[0]) << 8) | buffer[1];
    MPU6050_Lastay=(((s16)buffer[2]) << 8) | buffer[3];
    MPU6050_Lastaz=(((s16)buffer[4]) << 8) | buffer[5];
        //温度测量值ADC
        MPU6050_LastTemp = (((s16)buffer[6]) << 8) | buffer[7];
        //陀螺仪测量值
    MPU6050_Lastgx=(((s16)buffer[8]) << 8)  | buffer[9];
    MPU6050_Lastgy=(((s16)buffer[10]) << 8) | buffer[11];
    MPU6050_Lastgz=(((s16)buffer[12]) << 8) | buffer[13];

        MPU6050_newValues(MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz
                                            ,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz);

        *ax  =MPU6050_FIFO[0][10];
        *ay  =MPU6050_FIFO[1][10];
        *az = MPU6050_FIFO[2][10];
        *gx  =MPU6050_FIFO[3][10]-Gx_offset;
        *gy = MPU6050_FIFO[4][10]-Gy_offset;
        *gz = MPU6050_FIFO[5][10]-Gz_offset;
    }
    else
    {
        *ax = MPU6050_FIFO[0][10];//=MPU6050_FIFO[0][10];
        *ay = MPU6050_FIFO[1][10];//=MPU6050_FIFO[1][10];
        *az = MPU6050_FIFO[2][10];//=MPU6050_FIFO[2][10];
        *gx = MPU6050_FIFO[3][10]-Gx_offset;//=MPU6050_FIFO[3][10];
        *gy = MPU6050_FIFO[4][10]-Gy_offset;//=MPU6050_FIFO[4][10];
        *gz = MPU6050_FIFO[5][10]-Gz_offset;//=MPU6050_FIFO[5][10];
    }
}

void MPU6050_getlastMotion6(s16* ax, s16* ay, s16* az, s16* gx, s16* gy, s16* gz)
{
    *ax  =MPU6050_FIFO[0][10];
    *ay  =MPU6050_FIFO[1][10];
    *az = MPU6050_FIFO[2][10];
    *gx  =MPU6050_FIFO[3][10]-Gx_offset;
    *gy = MPU6050_FIFO[4][10]-Gy_offset;
    *gz = MPU6050_FIFO[5][10]-Gz_offset;
}

/**
  * @brief: 读取 MPU6050的陀螺仪偏置
  * @note:
  *   - 此时模块应该被静止放置,以测试静止时的陀螺仪输出
  */
void MPU6050_InitGyro_Offset(void)
{
    u8 i;
    s16 temp[6];
    s32 tempgx=0,tempgy=0,tempgz=0;
    s32 tempax=0,tempay=0,tempaz=0;
    Gx_offset=0;
    Gy_offset=0;
    Gz_offset=0;
    for(i=0;i<50;i++)
    {
        OS_Delay_us(100);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
    }
    for(i=0;i<100;i++)
    {
        OS_Delay_us(200);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
        tempax+= temp[0];
        tempay+= temp[1];
        tempaz+= temp[2];
        tempgx+= temp[3];
        tempgy+= temp[4];
        tempgz+= temp[5];
    }

    Gx_offset=tempgx/100;//MPU6050_FIFO[3][10];
    Gy_offset=tempgy/100;//MPU6050_FIFO[4][10];
    Gz_offset=tempgz/100;//MPU6050_FIFO[5][10];
    tempax/=100;
    tempay/=100;
    tempaz/=100;
    Acc1G_Values= (float)(tempax+tempay+tempaz);
}












#if 0
uint8 buffer[14];

int16 MPU6050_FIFO[6][11];
int16 Gx_offset=0,Gy_offset=0,Gz_offset=0;
float Acc1G_Values;

#if 0
/*
 * 读取指定设备指定寄存器的一个值
 * @param p_device_address  : 目标设备地址
 * @param p_reg_address     : 寄存器地址
 * @retval                  读取到的值
 */
uint8  MPU6050_read_byte( uint8 p_device_address, uint8 p_reg_address )
{
    uint8   temp;

    IIC_start();

    IIC_send_byte( p_device_address ); //发送写命令

    IIC_wait_ACK();

    IIC_send_byte(p_reg_address); //发送地址

    IIC_wait_ACK();

    IIC_start();

    IIC_send_byte( p_device_address + 1 ); //进入接收模式

    IIC_wait_ACK();

    temp = IIC_read_byte( 1 );

    IIC_stop();

    return temp;
}
#endif


/**
  * @brief: 读取指定设备指定寄存器的 length个值
  * @param: dev: 目标设备地址
  * @param: reg: 目标寄存器地址
  * @param: length: 要读取的字节数
  * @param: *data: 读出来数据将存放的地址指针
  * @retval: 读取出的数据数目
  */
uint8  MPU6050_read_bytes( uint8 dev, uint8 reg, uint8 length, uint8 *data )
{
    uint8 count = 0;

    IIC_start();

    IIC_send_byte( dev );     //发送写命令

    IIC_wait_ACK();

    IIC_send_byte( reg );     //发送地址

    IIC_wait_ACK();

    IIC_start();

    IIC_send_byte( dev + 1 );   //进入接收模式

    IIC_wait_ACK();

    for( count = 0; count < length; count++ )
    {
        if( count != length - 1 )
            data[count] = IIC_read_byte(1); //带ACK的读数据
        else
            data[count] = IIC_read_byte(0); //最后一个字节NACK
    }

    IIC_stop(); //产生一个停止条件

    return count;
}
/**
  * @brief: 读取指定设备指定寄存器一个字节
  * @param:  dev: 目标设备地址
  * @param:  reg: 寄存器地址
  * @param: data: 读取字节的存放地址
  * @retval: 读取数据的数目，这里始终是 1
  */
uint8  MPU6050_read_byte( uint8 dev, uint8 reg, uint8 *data )
{
    return MPU6050_read_bytes( dev, reg, 1, data );
}


/**
  * @brief: 将多个字节写入指定设备指定寄存器
  * @param:    dev: 目标设备地址
  * @param:    reg: 寄存器地址
  * @param: length: 要写的字节数
  * @param:  *data: 将要写的数据的首地址
  * @retval: 写入的字节数
  */
uint8  MPU6050_write_bytes( uint8 dev, uint8 reg, uint8 length, uint8 *data )
{
    uint8   count;

    IIC_start();

    IIC_send_byte(dev);    //发送写命令

    IIC_wait_ACK();

    IIC_send_byte(reg);   //发送地址

    IIC_wait_ACK();

    for( count = 0; count < length; count++ )
    {
        IIC_send_byte( data[count] );
        IIC_wait_ACK();
    }

    IIC_stop();//产生一个停止条件

    return count;
}
/**
  * @brief: 写入指定设备指定寄存器一个字节
  * @param:  dev: 目标设备地址
  * @param:  reg: 寄存器地址
  * @param: data: 将要写入的字节
  * @retval: 写入数据的数目，这里始终是 1
  */
uint8  MPU6050_write_byte( uint8 dev, uint8 reg, uint8 data )
{
    return MPU6050_write_bytes( dev, reg, 1, &data );
}


/**
  * @brief: 读取MPU6050 WHO_AM_I 标识
  * @retval: 0x68 - MPU6050 WHO_AM_I 标识   其他 - 未正确读取到
  */
uint8  MPU6050_get_device_ID( void )
{
    uint8   t_ch;

    MPU6050_read_byte( devAddr, MPU6050_RA_WHO_AM_I, &t_ch );

    return t_ch;
}

/**
  * @brief: 检测MPU6050 是否已经连接
  * @retval: 0 - 未检测到MPU6050    1 - 检测到MPU6050
  */
uint8  MPU6050_test_connection( void )
{
    if( MPU6050_get_device_ID() == 0x68 )  //读取设备ID: 0b01101000;
        return 1;
    else
        return 0;
}

/**
  * @brief: 读 修改 写 指定设备 指定寄存器一个字节 中的多个位
  * @param: dev: 目标设备地址
  * @param: reg: 寄存器地址
  * @param: bitStart: 目标字节的起始位
  * @param: length: 位长度
  * @param: data: 存放改变目标字节位的值
  * @retval: 1 - 位修改成功    0 - 修改失败
  */
uint8  MPU6050_write_bits( uint8 dev, uint8 reg, uint8 bitStart, uint8 length, uint8 data )
{
    uint8   temp;

    if( MPU6050_read_byte(dev, reg, &temp) != 0 )
    {
        uint8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        temp &= mask;
        temp |= data;
        return MPU6050_write_byte(dev, reg, temp);
    }
    else
    {
        return 0;
    }
}

/**
  * @brief: 读 修改 写 指定设备 指定寄存器一个字节 中的1个位
  * @param: dev: 目标设备地址
  * @param: reg: 寄存器地址
  * @param: bitNum: 要修改目标字节的bitNum位
  * @param: data: 为0 时，目标位将被清0 否则将被置位
  * @retval: 1 - 修改成功    0 - 修改失败
  */
uint8 MPU6050_writeBit(uint8 dev, uint8 reg, uint8 bitNum, uint8 data)
{
    uint8 b;
    MPU6050_read_byte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return MPU6050_write_byte(dev, reg, b);
}

/**
  * @brief: 设置MPU6050的时钟
  * @param: source: 时钟源
  * @note:
  * CLK_SEL | Clock Source
  * --------+--------------------------------------
  * 0       | Internal oscillator
  * 1       | PLL with X Gyro reference
  * 2       | PLL with Y Gyro reference
  * 3       | PLL with Z Gyro reference
  * 4       | PLL with external 32.768kHz reference
  * 5       | PLL with external 19.2MHz reference
  * 6       | Reserved
  * 7       | Stops the clock and keeps the timing generator in reset
  */
void  MPU6050_set_clock_source( uint8 source )
{
    MPU6050_write_bits( devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source );
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050_setFullScaleGyroRange(uint8 range)
{
    MPU6050_write_bits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

/**
  * @brief: 设置MPU6050加速度计的最大量程
  * @param: range: 最大两成
  */
void MPU6050_setFullScaleAccelRange(uint8 range)
{
    MPU6050_write_bits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/**
  * @brief: 设置MPU6050是否进入睡眠模式
    * @param: enabled =1   睡觉
    *                 =0   工作
  */
void MPU6050_setSleepEnabled(uint8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**
  * @brief: 设置 MPU6050 是否为AUX I2C线的主机
  * @param: enable = 1 主机模式
  * @param:        = 0 非主机模式
  */
void MPU6050_setI2CMasterModeEnabled(uint8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**
  * @brief: 设置 MPU6050 是否为AUX I2C线的BYPASS模式
  * @param: enable = 1 BYPASS模式
  *                = 0 非BYPASS模式
  */
void MPU6050_setI2CBypassEnabled(uint8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}


/**
  * @brief: 初始化MPU6050以进入可用状态。
  */
void  MPU6050_init( void )
{
    int16 temp[6];
    uint8 i;

    MPU6050_set_clock_source( MPU6050_CLOCK_INTERNAL ); //设置时钟
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_250); //陀螺仪最大量程 +-250度每秒
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2); //加速度度最大量程 +-2G
    MPU6050_setSleepEnabled(0);                 //进入工作状态
    MPU6050_setI2CMasterModeEnabled(0); //不让MPU6050 控制AUXI2C
    MPU6050_setI2CBypassEnabled(1);         //主控制器的I2C与 MPU6050的AUXI2C  直通。控制器可以直接访问HMC5883L
    //配置MPU6050 的中断模式 和中断电平模式
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, 0);   //INT配置为高电平有效
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, 0);    //INT配置为推挽输出
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, 1);//INT pin持续为高电平知道清除
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, 1);//任意读取中断状态位，都清除中断
    //开数据转换完成中断
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, 1);  //打开测量数据准备好中断

    for( i=0; i<20; i++ )
    {   //更新FIFO数组
        OS_Delay_us(50);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
    }
    MPU6050_InitGyro_Offset(); //初始化陀螺仪的偏置，此时模块应该不要被移动
}


/**
  * @brief: 检查 MPU6050的中断引脚，测试是否完成转换
  * @retval: 1 - 转换完成
  *          0 - 数据寄存器还没有更新
  */
uint8 MPU6050_is_DRY(void)
{
    uint8 int_status = 0;

    MPU6050_read_byte(devAddr,MPU6050_RA_INT_STATUS,&int_status);
    int_status &= (1<<MPU6050_INTERRUPT_DATA_RDY_BIT);

    if(int_status)
    {
        return 1;
    }
    else
        return 0;
}

int16 MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz,MPU6050_LastTemp,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz;
/**
  * @brief: 读取 MPU6050的当前测量值
  */
void MPU6050_getMotion6(int16* ax, int16* ay, int16* az, int16* gx, int16* gy, int16* gz)
{
    if(MPU6050_is_DRY())
    {
        //加速度测量值
        MPU6050_read_bytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    MPU6050_Lastax=(((int16)buffer[0]) << 8) | buffer[1];
    MPU6050_Lastay=(((int16)buffer[2]) << 8) | buffer[3];
    MPU6050_Lastaz=(((int16)buffer[4]) << 8) | buffer[5];
        //温度测量值ADC
        MPU6050_LastTemp = (((int16)buffer[6]) << 8) | buffer[7];
        //陀螺仪测量值
    MPU6050_Lastgx=(((int16)buffer[8]) << 8)  | buffer[9];
    MPU6050_Lastgy=(((int16)buffer[10]) << 8) | buffer[11];
    MPU6050_Lastgz=(((int16)buffer[12]) << 8) | buffer[13];

        MPU6050_newValues(MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz
                                            ,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz);

        *ax  =MPU6050_FIFO[0][10];
        *ay  =MPU6050_FIFO[1][10];
        *az = MPU6050_FIFO[2][10];
        *gx  =MPU6050_FIFO[3][10]-Gx_offset;
        *gy = MPU6050_FIFO[4][10]-Gy_offset;
        *gz = MPU6050_FIFO[5][10]-Gz_offset;
    }
    else
    {
        *ax = MPU6050_FIFO[0][10];//=MPU6050_FIFO[0][10];
        *ay = MPU6050_FIFO[1][10];//=MPU6050_FIFO[1][10];
        *az = MPU6050_FIFO[2][10];//=MPU6050_FIFO[2][10];
        *gx = MPU6050_FIFO[3][10]-Gx_offset;//=MPU6050_FIFO[3][10];
        *gy = MPU6050_FIFO[4][10]-Gy_offset;//=MPU6050_FIFO[4][10];
        *gz = MPU6050_FIFO[5][10]-Gz_offset;//=MPU6050_FIFO[5][10];
    }
}

void MPU6050_getlastMotion6(int16* ax, int16* ay, int16* az, int16* gx, int16* gy, int16* gz)
{
    *ax  =MPU6050_FIFO[0][10];
    *ay  =MPU6050_FIFO[1][10];
    *az = MPU6050_FIFO[2][10];
    *gx  =MPU6050_FIFO[3][10]-Gx_offset;
    *gy = MPU6050_FIFO[4][10]-Gy_offset;
    *gz = MPU6050_FIFO[5][10]-Gz_offset;
}

/**
  * @brief: 读取 MPU6050的陀螺仪偏置
  * @note:
  *   - 此时模块应该被静止放置,以测试静止时的陀螺仪输出
  */
void MPU6050_InitGyro_Offset(void)
{
    uint8 i;
    int16 temp[6];
    s32 tempgx=0,tempgy=0,tempgz=0;
    s32 tempax=0,tempay=0,tempaz=0;
    Gx_offset=0;
    Gy_offset=0;
    Gz_offset=0;
    for(i=0;i<50;i++)
    {
        OS_Delay_us(100);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
//        LED1 = !LED1;
    }
    for(i=0;i<100;i++)
    {
        OS_Delay_us(200);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
        tempax+= temp[0];
        tempay+= temp[1];
        tempaz+= temp[2];
        tempgx+= temp[3];
        tempgy+= temp[4];
        tempgz+= temp[5];
//        LED1 = !LED1;
    }

    Gx_offset=tempgx/100;//MPU6050_FIFO[3][10];
    Gy_offset=tempgy/100;//MPU6050_FIFO[4][10];
    Gz_offset=tempgz/100;//MPU6050_FIFO[5][10];
    tempax/=100;
    tempay/=100;
    tempaz/=100;
    Acc1G_Values= (float)(tempax+tempay+tempaz);
}


/*---------------------------------------------------------------------------------------------------*/

/**
  * @brief: 读取三个轴加速度的总和
  * @retval: 返回三个轴加速度的总和
  */
float MPU6050_1GValue(void)
{
    return Acc1G_Values;
}

/**
  * @brief: 将新的ADC数据更新到FIFO数组，进行滤波处理
  * @param: ax,ay,az,gx,gy,gz: 新的ADC数据
  */
void  MPU6050_newValues(int16 ax,int16 ay,int16 az,int16 gx,int16 gy,int16 gz)
{
    uint8 i ;
    s32 sum=0;
    for(i=1;i<10;i++)
    {   //FIFO 操作
        MPU6050_FIFO[0][i-1]=MPU6050_FIFO[0][i];
        MPU6050_FIFO[1][i-1]=MPU6050_FIFO[1][i];
        MPU6050_FIFO[2][i-1]=MPU6050_FIFO[2][i];
        MPU6050_FIFO[3][i-1]=MPU6050_FIFO[3][i];
        MPU6050_FIFO[4][i-1]=MPU6050_FIFO[4][i];
        MPU6050_FIFO[5][i-1]=MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[0][9]=ax;//将新的数据放置到 数据的最后面
    MPU6050_FIFO[1][9]=ay;
    MPU6050_FIFO[2][9]=az;
    MPU6050_FIFO[3][9]=gx;
    MPU6050_FIFO[4][9]=gy;
    MPU6050_FIFO[5][9]=gz;

    sum=0;
    for(i=0;i<10;i++)
    {   //求当前数组的合，再取平均值
         sum+=MPU6050_FIFO[0][i];
    }
    MPU6050_FIFO[0][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[1][i];
    }
    MPU6050_FIFO[1][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[2][i];
    }
    MPU6050_FIFO[2][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[3][i];
    }
    MPU6050_FIFO[3][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[4][i];
    }
    MPU6050_FIFO[4][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[5][10]=sum/10;
}

#if 0

/**
  * @brief: 读 修改 写 指定设备 指定寄存器一个字节 中的多个位
  * @param: dev: 目标设备地址
  * @param: reg: 寄存器地址
  * @param: bitStart: 目标字节的起始位
  * @param: length: 位长度
  * @param: data: 存放改变目标字节位的值
  * @retval: 1 - 位修改成功    0 - 修改失败
  */
uint8  MPU6050_writeBits(uint8 dev,uint8 reg,uint8 bitStart,uint8 length,uint8 data)
{
    uint8 b;
    if (MPU6050_read_byte(dev, reg, &b) != 0)
    {
        uint8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return MPU6050_writeByte(dev, reg, b);
    }
    else
    {
        return 0;
    }
}







/**
  * @brief: 读 修改 写 指定设备 指定寄存器一个字节 中的1个位
  * @param: dev: 目标设备地址
  * @param: reg: 寄存器地址
  * @param: bitNum: 要修改目标字节的bitNum位
  * @param: data: 为0 时，目标位将被清0 否则将被置位
  * @retval: 1 - 修改成功    0 - 修改失败
  */
uint8 MPU6050_writeBit(uint8 dev, uint8 reg, uint8 bitNum, uint8 data)
{
    uint8 b;
    MPU6050_read_byte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return MPU6050_writeByte(dev, reg, b);
}

/*---------------------------------------------------------------------------------------------------*/

/**
  * @brief: 读取三个轴加速度的总和
  * @retval: 返回三个轴加速度的总和
  */
float MPU6050_1GValue(void)
{
    return Acc1G_Values;
}

/**
  * @brief: 将新的ADC数据更新到FIFO数组，进行滤波处理
  * @param: ax,ay,az,gx,gy,gz: 新的ADC数据
  */
void  MPU6050_newValues(int16 ax,int16 ay,int16 az,int16 gx,int16 gy,int16 gz)
{
    uint8 i ;
    s32 sum=0;
    for(i=1;i<10;i++)
    {   //FIFO 操作
        MPU6050_FIFO[0][i-1]=MPU6050_FIFO[0][i];
        MPU6050_FIFO[1][i-1]=MPU6050_FIFO[1][i];
        MPU6050_FIFO[2][i-1]=MPU6050_FIFO[2][i];
        MPU6050_FIFO[3][i-1]=MPU6050_FIFO[3][i];
        MPU6050_FIFO[4][i-1]=MPU6050_FIFO[4][i];
        MPU6050_FIFO[5][i-1]=MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[0][9]=ax;//将新的数据放置到 数据的最后面
    MPU6050_FIFO[1][9]=ay;
    MPU6050_FIFO[2][9]=az;
    MPU6050_FIFO[3][9]=gx;
    MPU6050_FIFO[4][9]=gy;
    MPU6050_FIFO[5][9]=gz;

    sum=0;
    for(i=0;i<10;i++)
    {   //求当前数组的合，再取平均值
         sum+=MPU6050_FIFO[0][i];
    }
    MPU6050_FIFO[0][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[1][i];
    }
    MPU6050_FIFO[1][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[2][i];
    }
    MPU6050_FIFO[2][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[3][i];
    }
    MPU6050_FIFO[3][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[4][i];
    }
    MPU6050_FIFO[4][10]=sum/10;

    sum=0;
    for(i=0;i<10;i++)
    {
         sum+=MPU6050_FIFO[5][i];
    }
    MPU6050_FIFO[5][10]=sum/10;
}

/**
  * @brief: 设置MPU6050的时钟
  * @param: source: 时钟源
  * @note:
  * CLK_SEL | Clock Source
  * --------+--------------------------------------
  * 0       | Internal oscillator
  * 1       | PLL with X Gyro reference
  * 2       | PLL with Y Gyro reference
  * 3       | PLL with Z Gyro reference
  * 4       | PLL with external 32.768kHz reference
  * 5       | PLL with external 19.2MHz reference
  * 6       | Reserved
  * 7       | Stops the clock and keeps the timing generator in reset
  */
void MPU6050_setClockSource(uint8 source)
{
    MPU6050_writeBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050_setFullScaleGyroRange(uint8 range)
{
    MPU6050_writeBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

/**
  * @brief: 设置MPU6050加速度计的最大量程
  * @param: range: 最大两成
  */
void MPU6050_setFullScaleAccelRange(uint8 range)
{
    MPU6050_writeBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/**
  * @brief: 设置MPU6050是否进入睡眠模式
    * @param: enabled =1   睡觉
    *                 =0   工作
  */
void MPU6050_setSleepEnabled(uint8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**
  * @brief: 读取MPU6050 WHO_AM_I 标识
  * @retval: 0x68 - MPU6050 WHO_AM_I标识   其他 - 未正确读取到
  */
uint8 MPU6050_getDeviceID(void)
{
    MPU6050_read_bytes(devAddr, MPU6050_RA_WHO_AM_I, 1, buffer);
    return buffer[0];
}

/**
  * @brief: 检测MPU6050 是否已经连接
  * @retval: 0 - 未检测到MPU6050    1 - 检测到MPU6050
  */
uint8 MPU6050_testConnection(void)
{
    if(MPU6050_getDeviceID() == 0x68)  //读取设备ID: 0b01101000;
        return 1;
    else
        return 0;
}

/**
  * @brief: 设置 MPU6050 是否为AUX I2C线的主机
  * @param: enable = 1 主机模式
  * @param:        = 0 非主机模式
  */
void MPU6050_setI2CMasterModeEnabled(uint8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**
  * @brief: 设置 MPU6050 是否为AUX I2C线的BYPASS模式
  * @param: enable = 1 BYPASS模式
  *                = 0 非BYPASS模式
  */
void MPU6050_setI2CBypassEnabled(uint8 enabled)
{
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

/**
  * @brief: 初始化MPU6050以进入可用状态。
  */
void MPU6050_initialize(void)
{
    int16 temp[6];
    uint8 i;

    MPU6050_setClockSource(MPU6050_CLOCK_INTERNAL);         //设置时钟
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_250); //陀螺仪最大量程 +-250度每秒
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2); //加速度度最大量程 +-2G
    MPU6050_setSleepEnabled(0);                 //进入工作状态
    MPU6050_setI2CMasterModeEnabled(0); //不让MPU6050 控制AUXI2C
    MPU6050_setI2CBypassEnabled(1);         //主控制器的I2C与 MPU6050的AUXI2C  直通。控制器可以直接访问HMC5883L
    //配置MPU6050 的中断模式 和中断电平模式
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, 0);   //INT配置为高电平有效
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, 0);    //INT配置为推挽输出
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, 1);//INT pin持续为高电平知道清除
    MPU6050_writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, 1);//任意读取中断状态位，都清除中断
    //开数据转换完成中断
  MPU6050_writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, 1);  //打开测量数据准备好中断

    for(i=0;i<20;i++)
    {//更新FIFO数组
        OS_Delay_us(50);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
    }
    MPU6050_InitGyro_Offset(); //初始化陀螺仪的偏置，此时模块应该不要被移动
}

/**
  * @brief: 检查 MPU6050的中断引脚，测试是否完成转换
  * @retval: 1 - 转换完成
  *          0 - 数据寄存器还没有更新
  */
uint8 MPU6050_is_DRY(void)
{
    uint8 int_status = 0;

    MPU6050_read_byte(devAddr,MPU6050_RA_INT_STATUS,&int_status);
    int_status &= (1<<MPU6050_INTERRUPT_DATA_RDY_BIT);

    if(int_status)
    {
        return 1;
    }
    else
        return 0;
}

int16 MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz,MPU6050_LastTemp,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz;
/**
  * @brief: 读取 MPU6050的当前测量值
  */
void MPU6050_getMotion6(int16* ax, int16* ay, int16* az, int16* gx, int16* gy, int16* gz)
{
    if(MPU6050_is_DRY())
    {
        //加速度测量值
        MPU6050_read_bytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    MPU6050_Lastax=(((int16)buffer[0]) << 8) | buffer[1];
    MPU6050_Lastay=(((int16)buffer[2]) << 8) | buffer[3];
    MPU6050_Lastaz=(((int16)buffer[4]) << 8) | buffer[5];
        //温度测量值ADC
        MPU6050_LastTemp = (((int16)buffer[6]) << 8) | buffer[7];
        //陀螺仪测量值
    MPU6050_Lastgx=(((int16)buffer[8]) << 8)  | buffer[9];
    MPU6050_Lastgy=(((int16)buffer[10]) << 8) | buffer[11];
    MPU6050_Lastgz=(((int16)buffer[12]) << 8) | buffer[13];

        MPU6050_newValues(MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz
                                            ,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz);

        *ax  =MPU6050_FIFO[0][10];
        *ay  =MPU6050_FIFO[1][10];
        *az = MPU6050_FIFO[2][10];
        *gx  =MPU6050_FIFO[3][10]-Gx_offset;
        *gy = MPU6050_FIFO[4][10]-Gy_offset;
        *gz = MPU6050_FIFO[5][10]-Gz_offset;
    }
    else
    {
        *ax = MPU6050_FIFO[0][10];//=MPU6050_FIFO[0][10];
        *ay = MPU6050_FIFO[1][10];//=MPU6050_FIFO[1][10];
        *az = MPU6050_FIFO[2][10];//=MPU6050_FIFO[2][10];
        *gx = MPU6050_FIFO[3][10]-Gx_offset;//=MPU6050_FIFO[3][10];
        *gy = MPU6050_FIFO[4][10]-Gy_offset;//=MPU6050_FIFO[4][10];
        *gz = MPU6050_FIFO[5][10]-Gz_offset;//=MPU6050_FIFO[5][10];
    }
}

void MPU6050_getlastMotion6(int16* ax, int16* ay, int16* az, int16* gx, int16* gy, int16* gz)
{
    *ax  =MPU6050_FIFO[0][10];
    *ay  =MPU6050_FIFO[1][10];
    *az = MPU6050_FIFO[2][10];
    *gx  =MPU6050_FIFO[3][10]-Gx_offset;
    *gy = MPU6050_FIFO[4][10]-Gy_offset;
    *gz = MPU6050_FIFO[5][10]-Gz_offset;
}

/**
  * @brief: 读取 MPU6050的陀螺仪偏置
  * @note:
  *   - 此时模块应该被静止放置,以测试静止时的陀螺仪输出
  */
void MPU6050_InitGyro_Offset(void)
{
    uint8 i;
    int16 temp[6];
    s32 tempgx=0,tempgy=0,tempgz=0;
    s32 tempax=0,tempay=0,tempaz=0;
    Gx_offset=0;
    Gy_offset=0;
    Gz_offset=0;
    for(i=0;i<50;i++)
    {
        OS_Delay_us(100);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
//        LED1 = !LED1;
    }
    for(i=0;i<100;i++)
    {
        OS_Delay_us(200);
        MPU6050_getMotion6(&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
        tempax+= temp[0];
        tempay+= temp[1];
        tempaz+= temp[2];
        tempgx+= temp[3];
        tempgy+= temp[4];
        tempgz+= temp[5];
//        LED1 = !LED1;
    }

    Gx_offset=tempgx/100;//MPU6050_FIFO[3][10];
    Gy_offset=tempgy/100;//MPU6050_FIFO[4][10];
    Gz_offset=tempgz/100;//MPU6050_FIFO[5][10];
    tempax/=100;
    tempay/=100;
    tempaz/=100;
    Acc1G_Values= (float)(tempax+tempay+tempaz);
}

#endif

#endif
