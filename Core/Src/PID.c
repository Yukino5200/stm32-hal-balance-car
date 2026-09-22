#include "PID.h"


/**
* 函    数：PID初始化
  * 参    数：p 指定结构体的地址
  * 返 回 值：无
  */
void PID_Init(PID_t *p)
{
    /*把PID表示状态的参数清零，避免之前遗留的参数对本次启动造成影响*/
    p->Target = 0;
    p->Actual = 0;
    p->Actual1 = 0;
    p->Out = 0;
    p->Error0 = 0;
    p->Error1 = 0;
    p->ErrorInt = 0;
}

/**
  * 函    数：PID计算及结构体变量值更新
  * 参    数：p 指定结构体的地址
  * 返 回 值：无
  */
void PID_Update(PID_t *p)
{
    /*获取本次误差和上次误差*/
    p->Error1 = p->Error0;					//获取上次误差
    p->Error0 = p->Target - p->Actual;		//获取本次误差，目标值减实际值，即为误差值

    /*外环误差积分（累加）*/
    /*如果Ki不为0，才进行误差积分，这样做的目的是便于调试*/
    /*因为在调试时，我们可能先把Ki设置为0，这时积分项无作用，误差消除不了，误差积分会积累到很大的值*/
    /*后续一旦Ki不为0，那么因为误差积分已经积累到很大的值了，这就导致积分项疯狂输出，不利于调试*/
    if (p->Ki != 0)				//如果Ki不为0
    {
        p->ErrorInt += p->Error0;	//进行误差积分

        /*误差积分限幅*/
        if (p->ErrorInt > p->ErrorIntMax)
        {
            p->ErrorInt = p->ErrorIntMax;
        }	//限制误差积分最大为结构体指定的ErrorIntMax
        if (p->ErrorInt < p->ErrorIntMin)
        {
            p->ErrorInt = p->ErrorIntMin;
        }	//限制误差积分最小为结构体指定的ErrorIntMin
    }
    else							//否则
    {
        p->ErrorInt = 0;			//误差积分直接归0
    }

    /*PID计算*/
    /*使用位置式PID公式，计算得到输出值*/
    p->Out = p->Kp * p->Error0
           + p->Ki * p->ErrorInt
//		   + p->Kd * (p->Error0 - p->Error1);		//普通PID计算公式
           - p->Kd * (p->Actual - p->Actual1);		//微分先行计算公式

    /*输出偏移*/
    if (p->Out > 0)
    {
        p->Out += p->OutOffset;
    }		//如果输出值为正，则加上结构体指定的OutOffset
    if (p->Out < 0)
    {
        p->Out -= p->OutOffset;
    }		//如果输出值为负，则减去结构体指定的OutOffset

    /*输出限幅*/
    if (p->Out > p->OutMax)
    {
        p->Out = p->OutMax;
    }	//限制输出值最大为结构体指定的OutMax
    if (p->Out < p->OutMin)
    {
        p->Out = p->OutMin;
    }	//限制输出值最小为结构体指定的OutMin

    /*获取上次实际值*/
    p->Actual1 = p->Actual;		//本轮计算后进行变量传递，下轮计算时Actual1即为上次实际值
}