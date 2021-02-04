#include  "BEEP.h"


void BEEP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);     //打开GPIOB时钟
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;			
	GPIO_Init(GPIOB,&GPIO_InitStruct);								 //GPIOB_Pin_8 推挽输出
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);                  //PB8输出清0
}




