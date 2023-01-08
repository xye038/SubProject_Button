#include "JuBei_Button.h"

#define Button_Pin_1     1
#define Button_Pin_2     2

Button_t Button_1;
Button_t Button_2; 

uint8_t Read_Key_Level(uint32_t Pin )  
{
   uint32_t data;
  // data=GPIOB_ReadPortPin(Pin);   
   if(data==0)
   {
     return 0;
   }else {
    return 1;
  }
}

void Button_One_Down_CallBack(void *btn)
{
  //����1�̰�����
}

void Button_Two_Long_CallBack(void *btn)
{
  //����2��������
} 
 
 void Button_Init(void)
 {
 	Button_Create(&Button_1, Read_Key_Level, Button_Pin_1, BUTTON_ON,BUTTON_LONG_CYCLE_DISABLE );
 	Button_Attach(&Button_1, BUTTON_DOWM, Button_One_Down_CallBack);
 	Button_Create(&Button_2, Read_Key_Level, Button_Pin_2, BUTTON_ON,BUTTON_LONG_CYCLE_ENABLE);
 	Button_Attach(&Button_2, BUTTON_LONG, Button_Two_Long_CallBack);
 }
 
 int main(void)
 {
 	Button_Init();
 	
 	while(1)
 	{
 		Button_Process();
	 }
 	
 }
