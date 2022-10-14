#ifndef  JUBEI_BUTTON_H_
#define  JUBEI_BUTTON_H_

#include "CH57x_common.h"

/* ��������ʱ��Ϊ10ms����������Ϊ20ms�����Է����������е���Ҳ���Է��ڶ�ʱ���ж��е��ã�
        ֻ��������⵽40ms״̬�������Ϊ��Ч����������Ͱ��������¼�
 */

#define BUTTON_ON               0   //�������µ�ƽ
#define BUTTON_OFF              1   //����δ���µ�ƽ
#define BUTTON_DEBOUNCE_TIME    2   //����ʱ��      (n-1)*��������
#define BUTTON_LONG_TIME        15  /* ����n��((n-1)*�������� ms)����Ϊ�����¼� */
#define BUTTON_LONG_CYCLE       5   //������������ʱ��  (n-1)*��������
#define BUTTON_LONG_CYCLE_ENABLE 0   //����������������
#define BUTTON_LONG_CYCLE_DISABLE 1  //��������������ֹ




#define TRIGGER_CB(event)   \
        if(btn->CallBack_Function[event]) \
          btn->CallBack_Function[event]((Button_t*)btn)

typedef void (*Button_CallBack)(void*);   /* ���������ص���������Ҫ�û�ʵ�� */

typedef enum {
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event, /* �����ص����¼� */
  NONE_TRIGGER
}Button_Event;

/*
  ÿ��������Ӧ1��ȫ�ֵĽṹ�������
  ���Ա������ʵ���˲��Ͷ��ְ���״̬�������
*/
typedef struct button
{
  /* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
  uint8_t (*Read_Button_Level)(uint32_t Pin); /* ��ȡ������ƽ��������Ҫ�û�ʵ�� */


  uint8_t Button_State              :   4;    /* ������ǰ״̬�����»��ǵ��� */
  uint8_t Button_Trigger_Level      :   2;    /* ����������ƽ */
  uint8_t Button_Last_Level         :   2;    /* ������ǰ��ƽ */

  uint8_t Button_Trigger_Event;     /* ���������¼���������˫���������� */
  uint8_t Button_Trigger_Last_Event; /*��һ�εĴ����¼�*/

  Button_CallBack CallBack_Function[number_of_event];

  uint8_t Button_Cycle;            /* ������������ */
  uint8_t Button_Cycle_Enable;    /*���������Ƿ񴥷�*/

  uint8_t Timer_Count;      /* ��ʱ */
  uint8_t Debounce_Time;    /* ����ʱ�� */

  uint8_t Long_Time;      /* �������³���ʱ�� */

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
