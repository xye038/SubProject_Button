#ifndef  JUBEI_BUTTON_H_
#define  JUBEI_BUTTON_H_

#include "CH57x_common.h"

/* 按键消抖时间为10ms，调用周期为20ms（可以放在主函数中调用也可以放在定时器中断中调用）
        只有连续检测到40ms状态不变才认为有效，包括弹起和按下两种事件
 */

#define BUTTON_ON               0   //按键按下电平
#define BUTTON_OFF              1   //按键未按下电平
#define BUTTON_DEBOUNCE_TIME    2   //消抖时间      (n-1)*调用周期
#define BUTTON_LONG_TIME        15  /* 持续n秒((n-1)*调用周期 ms)，认为长按事件 */
#define BUTTON_LONG_CYCLE       5   //长按触发周期时间  (n-1)*调用周期
#define BUTTON_LONG_CYCLE_ENABLE 0   //连续长按触发允许
#define BUTTON_LONG_CYCLE_DISABLE 1  //连续长按触发禁止




#define TRIGGER_CB(event)   \
        if(btn->CallBack_Function[event]) \
          btn->CallBack_Function[event]((Button_t*)btn)

typedef void (*Button_CallBack)(void*);   /* 按键触发回调函数，需要用户实现 */

typedef enum {
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event, /* 触发回调的事件 */
  NONE_TRIGGER
}Button_Event;

/*
  每个按键对应1个全局的结构体变量。
  其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct button
{
  /* 下面是一个函数指针，指向判断按键手否按下的函数 */
  uint8_t (*Read_Button_Level)(uint32_t Pin); /* 读取按键电平函数，需要用户实现 */


  uint8_t Button_State              :   4;    /* 按键当前状态（按下还是弹起） */
  uint8_t Button_Trigger_Level      :   2;    /* 按键触发电平 */
  uint8_t Button_Last_Level         :   2;    /* 按键当前电平 */

  uint8_t Button_Trigger_Event;     /* 按键触发事件，单击，双击，长按等 */
  uint8_t Button_Trigger_Last_Event; /*上一次的触发事件*/

  Button_CallBack CallBack_Function[number_of_event];

  uint8_t Button_Cycle;            /* 连续按键周期 */
  uint8_t Button_Cycle_Enable;    /*连续按键是否触发*/

  uint8_t Timer_Count;      /* 计时 */
  uint8_t Debounce_Time;    /* 消抖时间 */

  uint8_t Long_Time;      /* 按键按下持续时间 */

  uint32_t Pin;

  struct button *Next;

}Button_t;

void Button_Create(Button_t *btn,
                   uint8_t(*read_btn_level)(uint32_t),
                   uint32_t Pin,
                   uint8_t btn_trigger_level,
                   uint8_t BUTTON_LONG_CYCLE_FLAG);
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback);
void Button_Delete(Button_t *btn);
void Button_Process(void);

#endif
