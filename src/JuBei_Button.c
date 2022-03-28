#include "JuBei_Button.h"

static struct button* Head_Button = NULL;

static void Add_Button(Button_t* btn);


/************************************************************
  * @brief   ��������
  * @param   name : ��������
  * @param   btn : �����ṹ��
  * @param   read_btn_level : ������ƽ��ȡ��������Ҫ�û��Լ�ʵ�ַ���uint8_t���͵ĵ�ƽ
  * @param   btn_trigger_level : ����������ƽ
  * @return  NULL
  ***********************************************************/
void Button_Create(Button_t *btn,
                   uint8_t(*read_btn_level)(uint32_t),
                   uint32_t Pin,
                   uint8_t btn_trigger_level)
{
  memset(btn, 0, sizeof(struct button));  //����ṹ����Ϣ�������û���֮ǰ���
  btn->Pin = Pin;
  btn->Button_State = NONE_TRIGGER;       //����״̬
  btn->Button_Trigger_Event = NONE_TRIGGER; //���������¼�   ������������ȥ�����������
  btn->Read_Button_Level = read_btn_level; //��������ƽ����
  btn->Button_Trigger_Level = btn_trigger_level;  //����������ƽ
//  btn->Button_Last_Level = btn->Read_Button_Level(btn->Pin); //������ǰ��ƽ
  btn->Button_Last_Level = 1; //������ǰ��ƽ
  btn->Debounce_Time = 0;
  Add_Button(btn);
}

/************************************************************
  * @brief   ���������¼���ص�����ӳ����������
  * @param   btn : �����ṹ��
  * @param   btn_event : ���������¼�
  * @param   btn_callback : ��������֮��Ļص�����������Ҫ�û�ʵ��
  * @return  NULL
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback; //�����¼������Ļص����������ڴ������¼�
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //�����¼������Ļص����������ڴ������¼�
  }
}

/************************************************************
  * @brief   �������ڴ�����
  * @param   btn:����İ���
  * @return  NULL
  * @note    ������һ�����ڵ��ô˺�������������Ϊ20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  uint8_t current_level = (uint8_t)btn->Read_Button_Level(btn->Pin);//��ȡ��ǰ������ƽ
  if((current_level != btn->Button_Last_Level)&&(++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME)) //������ƽ�����仯������
  {
      btn->Button_Last_Level = current_level; //���µ�ǰ������ƽ
      btn->Debounce_Time = 0;                 //ȷ�����ǰ���

      //���������û�����µģ��ı䰴��״̬Ϊ����(�״ΰ���)
      if(btn->Button_State == NONE_TRIGGER)
      {
        btn->Button_State = BUTTON_DOWM;
      }
      //�ͷŰ���
      else if(btn->Button_State == BUTTON_DOWM)
      {
        btn->Button_State = BUTTON_UP;
        TRIGGER_CB(BUTTON_UP);    // �����ͷ�
      }
  }

  switch(btn->Button_State)
  {
    case BUTTON_DOWM :            // ����״̬
    {
      if(btn->Button_Last_Level == btn->Button_Trigger_Level) //��������
      {

        btn->Button_Trigger_Event = BUTTON_DOWM;

        if(++(btn->Long_Time) >= BUTTON_LONG_TIME)  //�ͷŰ���ǰ���´����¼�Ϊ����
        {

          if(++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE)    //������������������
          {
            btn->Button_Cycle = 0;
            btn->Button_Trigger_Event = BUTTON_LONG;
            TRIGGER_CB(BUTTON_LONG);    //����
          }

          if(btn->Long_Time == 0xFF)  //����ʱ�����
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

    case BUTTON_UP :        // ����״̬
    {
      if(btn->Button_Trigger_Event == BUTTON_DOWM)  //��������
      {
            TRIGGER_CB(BUTTON_DOWM);    //����
            btn->Long_Time = 0;
            btn->Button_State = NONE_TRIGGER;
      }

      else if(btn->Button_Trigger_Event == BUTTON_LONG)
      {
            TRIGGER_CB(BUTTON_LONG_FREE);    //�����ͷ�
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
  * @brief   ɾ��һ���Ѿ������İ���
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
  * @brief   ʹ�õ�����������������
  * @param   NULL
  * @return  NULL
  ***********************************************************/
static void Add_Button(Button_t* btn)
{
  btn->Next = Head_Button;
  Head_Button = btn;
}

/************************************************************
  * @brief   �����ķ�ʽɨ�谴�������ᶪʧÿ������
  * @param   NULL
  * @return  NULL
  * @note    �˺���Ҫ���ڵ��ã�����20-50ms����һ��
  ***********************************************************/
void Button_Process(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
      Button_Cycle_Process(pass_btn);
  }
}


