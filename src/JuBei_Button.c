#include "JuBei_Button.h"

static struct button* Head_Button = NULL;

static void Add_Button(Button_t* btn);


/************************************************************
  * @brief   按键创建
  * @param   name : 按键名称
  * @param   btn : 按键结构体
  * @param   read_btn_level : 按键电平读取函数，需要用户自己实现返回uint8_t类型的电平
  * @param   btn_trigger_level : 按键触发电平
  * @return  NULL
  ***********************************************************/
void Button_Create(Button_t *btn,
                   uint8_t(*read_btn_level)(uint32_t),
                   uint32_t Pin,
                   uint8_t btn_trigger_level)
{
  memset(btn, 0, sizeof(struct button));  //清除结构体信息，建议用户在之前清除
  btn->Pin = Pin;
  btn->Button_State = NONE_TRIGGER;       //按键状态
  btn->Button_Trigger_Event = NONE_TRIGGER; //按键触发事件   长按，单按下去，单按弹起等
  btn->Read_Button_Level = read_btn_level; //按键读电平函数
  btn->Button_Trigger_Level = btn_trigger_level;  //按键触发电平
//  btn->Button_Last_Level = btn->Read_Button_Level(btn->Pin); //按键当前电平
  btn->Button_Last_Level = 1; //按键当前电平
  btn->Debounce_Time = 0;
  Add_Button(btn);
}

/************************************************************
  * @brief   按键触发事件与回调函数映射链接起来
  * @param   btn : 按键结构体
  * @param   btn_event : 按键触发事件
  * @param   btn_callback : 按键触发之后的回调处理函数。需要用户实现
  * @return  NULL
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
  }
}

/************************************************************
  * @brief   按键周期处理函数
  * @param   btn:处理的按键
  * @return  NULL
  * @note    必须以一定周期调用此函数，建议周期为20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  uint8_t current_level = (uint8_t)btn->Read_Button_Level(btn->Pin);//获取当前按键电平
  if((current_level != btn->Button_Last_Level)&&(++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME)) //按键电平发生变化，消抖
  {
      btn->Button_Last_Level = current_level; //更新当前按键电平
      btn->Debounce_Time = 0;                 //确定了是按下

      //如果按键是没被按下的，改变按键状态为按下(首次按下)
      if(btn->Button_State == NONE_TRIGGER)
      {
        btn->Button_State = BUTTON_DOWM;
      }
      //释放按键
      else if(btn->Button_State == BUTTON_DOWM)
      {
        btn->Button_State = BUTTON_UP;
        TRIGGER_CB(BUTTON_UP);    // 触发释放
      }
  }

  switch(btn->Button_State)
  {
    case BUTTON_DOWM :            // 按下状态
    {
      if(btn->Button_Last_Level == btn->Button_Trigger_Level) //按键按下
      {

        btn->Button_Trigger_Event = BUTTON_DOWM;

        if(++(btn->Long_Time) >= BUTTON_LONG_TIME)  //释放按键前更新触发事件为长按
        {

          if(++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE)    //连续触发长按的周期
          {
            btn->Button_Cycle = 0;
            btn->Button_Trigger_Event = BUTTON_LONG;
            TRIGGER_CB(BUTTON_LONG);    //长按
          }

          if(btn->Long_Time == 0xFF)  //更新时间溢出
          {
            btn->Long_Time = BUTTON_LONG_TIME;
          }
        }
        if(btn->Button_Trigger_Last_Event == BUTTON_LONG)
        {
          btn->Button_Trigger_Event = BUTTON_LONG;
        }
        btn->Button_Trigger_Last_Event = btn->Button_Trigger_Event;
      }
      break;
    }

    case BUTTON_UP :        // 弹起状态
    {
      if(btn->Button_Trigger_Event == BUTTON_DOWM)  //触发单击
      {
            TRIGGER_CB(BUTTON_DOWM);    //单击
            btn->Long_Time = 0;
            btn->Button_State = NONE_TRIGGER;
      }

      else if(btn->Button_Trigger_Event == BUTTON_LONG)
      {
            TRIGGER_CB(BUTTON_LONG_FREE);    //长按释放
            btn->Long_Time = 0;
            btn->Button_State = NONE_TRIGGER;
      }
      btn->Button_Trigger_Last_Event = BUTTON_DOWM;
      break;
    }
    default :
      break;
  }

}

/************************************************************
  * @brief   删除一个已经创建的按键
  * @param   NULL
  * @return  NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
  struct button** curr;
  for(curr = &Head_Button; *curr;)
  {
    struct button* entry = *curr;
    if (entry == btn)
    {
      *curr = entry->Next;
    }
    else
    {
      curr = &entry->Next;
    }
  }
}

/************************************************************
  * @brief   使用单链表将按键连接起来
  * @param   NULL
  * @return  NULL
  ***********************************************************/
static void Add_Button(Button_t* btn)
{
  btn->Next = Head_Button;
  Head_Button = btn;
}

/************************************************************
  * @brief   遍历的方式扫描按键，不会丢失每个按键
  * @param   NULL
  * @return  NULL
  * @note    此函数要周期调用，建议20-50ms调用一次
  ***********************************************************/
void Button_Process(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
      Button_Cycle_Process(pass_btn);
  }
}


