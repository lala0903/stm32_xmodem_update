#include "KEY.h"
#include "stm32f10x.h"
#include  "delay.h"

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);     //打开GPIOA和GPIOE时钟
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);                  //GPIOA_Pin_0 下拉输入
		
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	GPIO_SetBits(GPIOE,GPIO_Pin_4|GPIO_Pin_3);//GPIOE_Pin_3 4 上拉输入
}
u8 KEY_Scan(u8 mode)    //按键检测
{
	static u8 key_up=1;   
	if(mode)
	{
		key_up=1;
	}
	if(key_up&&(KEY0==0||KEY1==0||KEY3==1))     //有按键按下
	{
		delay_ms(10);
		key_up=0;
		if(KEY0==0) return KEY0_PRES;             //按键0按下
		else if(KEY1==0) return KEY1_PRES;				//按键1按下
		else if(KEY3==1) return WK_UP_PRES;				//按键WK_UP按下
	}
	else if(KEY0==1&&KEY1==1&&KEY3==0)					
	{
		key_up=1;
	}
	return 0;																		//按键没有按键按下
}

