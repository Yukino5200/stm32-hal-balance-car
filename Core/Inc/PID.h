//
// Created by lenovo on 2026/9/16.
//

#ifndef HAL_PID_H
#define HAL_PID_H

typedef struct {		//定义PID结构体变量类型
    float Target;		//目标值，由用户设定
    float Actual;		//实际值，从传感器读取
    float Actual1;		//上次实际值
    float Out;			//输出值，作用于执行器

    float Kp;			//比例项权重
    float Ki;			//积分项权重
    float Kd;			//微分项权重

    float Error0;		//本次误差
    float Error1;		//上次误差
    float ErrorInt;		//误差积分

    float ErrorIntMax;	//误差积分的最大值
    float ErrorIntMin;	//误差积分的最小值

    float OutMax;		//输出限幅的最大值
    float OutMin;		//输出限幅的最小值

    float OutOffset;	//输出偏移
} PID_t;

void PID_Init(PID_t *p);
void PID_Update(PID_t *p);

#endif //HAL_PID_H
