#include "MPU6050_Reg.h"
#include "MPU6050.h"
#include "MyI2C.h"

#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址


/*
 * 该函数用于向MPU6050的寄存器写入数据
 * RegAddress:要写入的寄存器地址
 * Data:要写入寄存器的数据
 */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    MyI2C_Start();						//I2C起始
    MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
    MyI2C_ReceiveAck();					//接收应答
    MyI2C_SendByte(RegAddress);			//发送寄存器地址
    MyI2C_ReceiveAck();					//接收应答
    MyI2C_SendByte(Data);				//发送要写入寄存器的数据
    MyI2C_ReceiveAck();					//接收应答
    MyI2C_Stop();						//I2C终止
}


/*
 * 该函数用于从MPU6050的寄存器读取数据
 * RegAddress:要读取的寄存器地址
 * 返回值:读取到的寄存器数据
 */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    MyI2C_Start();						//I2C起始
    MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
    MyI2C_ReceiveAck();					//接收应答
    MyI2C_SendByte(RegAddress);			//发送寄存器地址
    MyI2C_ReceiveAck();					//接收应答

    MyI2C_Start();						//I2C重复起始
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取(读写位看最低位)
    MyI2C_ReceiveAck();					//接收应答
    Data = MyI2C_ReceiveByte();			//接收指定寄存器的数据
    MyI2C_SendAck(1);					//发送应答，给从机非应答，终止从机的数据输出
    MyI2C_Stop();						//I2C终止

    return Data;
}

/*
 * 该函数用于从MPU6050的寄存器连续读取多个字节数据
 * RegAddress:要读取的寄存器地址
 * DataArray:存储读取到的数据的数组首地址（取指针，写到该位置的数组将会被该函数改变）
 * Count:要读取的字节数
 */
void MPU6050_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
    uint8_t i;

    MyI2C_Start();						//I2C起始
    MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
    MyI2C_ReceiveAck();					//接收应答
    MyI2C_SendByte(RegAddress);			//发送寄存器地址
    MyI2C_ReceiveAck();					//接收应答

    MyI2C_Start();						//I2C重复起始
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取
    MyI2C_ReceiveAck();					//接收应答
    for (i = 0; i < Count; i ++)		//循环Count次，连续读取多个字节，MPU6050内部地址指针会自动自增
    {
        DataArray[i] = MyI2C_ReceiveByte();	//接收指定寄存器的数据，存入数组第i个数据
        if (i < Count - 1)				//未读取到最后一个字节
        {
            MyI2C_SendAck(0);			//正常发送应答，从机后续会继续输出下一个字节数据
        }
        else							//读取到了最后一个字节
        {
            MyI2C_SendAck(1);			//给非应答，从机后续将不会输出数据，主机收回总线控制权
        }
    }
    MyI2C_Stop();						//I2C终止
}

/*
 * 该函数用于初始化MPU6050寄存器，配置MPU6050的工作模式和参数
 */
void MPU6050_Init(void)
{
    /*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);		//采样率分频寄存器，配置采样率
    MPU6050_WriteReg(MPU6050_CONFIG, 0x00);			//配置寄存器，配置DLPF
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

/*
 * 该函数用于获取MPU6050的ID信息
 * 返回值:MPU6050的ID信息
 */
uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);		//返回WHO_AM_I寄存器的值
}

/*
 * 该函数用于获取MPU6050的加速度计和陀螺仪数据
 * AccX, AccY, AccZ:输出参数，返回加速度计X、Y、Z轴的数据
 * GyroX, GyroY, GyroZ:输出参数，返回陀螺仪X、Y、Z轴的数据
 */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
                        int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t Data[14];

    /*通过MPU6050_ReadRegs连续读取多个数据，效率更高*/
    MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, Data, 14);	//从MPU6050_ACCEL_XOUT_H开始，连续读取14个字节，存入Data数组里

    /*
     *数据拼接，通过输出参数返回
     *Data数组类型为uint8_t，存储的是每个寄存器的8位数据，需要将相邻的两个寄存器数据拼接成一个16位的数据，因此需要强转为int16_t类型
     */
    *AccX = (int16_t)((Data[0] << 8) | Data[1]);		//Data[0]和Data[1]为加速度计X轴数据
    *AccY = (int16_t)((Data[2] << 8) | Data[3]);		//Data[2]和Data[3]为加速度计Y轴数据
    *AccZ = (int16_t)((Data[4] << 8) | Data[5]);		//Data[4]和Data[5]为加速度计Z轴数据

    //Data[6]和Data[7]为温度数据，此处暂时不用

    *GyroX = (int16_t)((Data[8] << 8) | Data[9]);		//Data[8]和Data[9]为陀螺仪X轴数据
    *GyroY = (int16_t)((Data[10] << 8) | Data[11]);	//Data[10]和Data[11]为陀螺仪Y轴数据
    *GyroZ = (int16_t)((Data[12] << 8) | Data[13]);	//Data[12]和Data[13]为陀螺仪Z轴数据
}
