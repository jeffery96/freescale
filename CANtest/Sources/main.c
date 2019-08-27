#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "CAN.h"

#define  BUS_CLOCK		   32000000	   //����Ƶ��
#define  OSC_CLOCK		   16000000	   //����Ƶ��

#define ID1 0x7DF
#define ID2 0x7E8
#define data_len_TX 8

unsigned char msg1_data[8] = {0x02, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char msg2_data[8] = {0x06, 0x41, 0x0, 0xBE, 0x3E, 0xB8, 0x11, 0x0};


struct can_msg msg_send, msg_get;   //���ͽ������ݱ���

msg_send.id = ID2;
msg_send.len = data_len_TX;
msg_send.RTR = FALSE;
msg_send.prty = 0;


void INIT_PLL(void) {

    CRGINT = 0;                  //���ж�
    CLKSEL_PLLSEL = 0;           //��δ��ʼ��PLLǰ��ʹ��PLL�������ΪCPUʱ��
        
    #if(BUS_CLOCK == 40000000) 
        SYNR = 4;
    #elif(BUS_CLOCK == 32000000)
        SYNR = 3;     
    #elif(BUS_CLOCK == 24000000)
        SYNR = 2;
    #endif 

    REFDV = 1;                   //PLLCLK=2��OSCCLK��(SYNR+1)/(REFDV+1)��64MHz ,fbus=32M
    PLLCTL_PLLON = 1;            //��PLL
    PLLCTL_AUTO = 1;             //ѡȡ�Զ�ģʽ
    while (CRGFLG_LOCK == 0);    //�ȴ�PLL����Ƶ��
        CLKSEL_PLLSEL = 1;       //ѡ��ϵͳʱ����PLL����
        
}


void initialize_ect(void){

    TSCR1_TFFCA = 1;  // ��ʱ����־λ�������
    TSCR1_TEN = 1;    // ��ʱ��ʹ��λ. 1=����ʱ����������; 0=ʹ����ʱ����������(����������)
    TIOS  = 0xff;      //ָ������ͨ��Ϊ����ȽϷ�ʽ
    TCTL1 = 0x00;	    // ���ĸ�ͨ������Ϊ��ʱ����������ŶϿ�
    TCTL2 = 0x00;     // ǰ�ĸ�ͨ������Ϊ��ʱ����������ŶϿ�
    DLYCT = 0x00;	    // �ӳٿ��ƹ��ܽ�ֹ
    ICOVW = 0x00;	    // ��Ӧ�ļĴ�����������;  NOVWx = 1, ��Ӧ�ļĴ�����������
    ICSYS = 0x00;	    // ��ֹIC��PAC�ı��ּĴ���
    TIE   = 0x00;     // ��ֹ����ͨ����ʱ�ж�
    TSCR2 = 0x07;	    // Ԥ��Ƶϵ��pr2-pr0:111,,ʱ������Ϊ4us,
    TFLG1 = 0xff;	    // �����IC/OC�жϱ�־λ
    TFLG2 = 0xff;     // ������ɶ�ʱ���жϱ�־λ
  
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
void interrupt CAN_receive(void) 
{
    if(MSCAN1GetMsg(&msg_get)) 
    {
        // ��������Ϣ
         if(msg_get.id == ID1 && (!msg_get.RTR)) 
         {
             for(;;){
                delayms(1000);       
                if(!MSCAN0SendMsg(msg_send)) {
                    for(;;);
                }
             }
         }
    }
    else 
    {
        for(;;);
    }
}
#pragma CODE_SEG DEFAULT


void main(void) {
  /* put your own code here */
  
    DisableInterrupts;
    INIT_PLL();
    initialize_ect();
    INIT_CAN0();
    INIT_CAN1();
    LEDCPU_dir=1;
    LEDCPU=0;
    
    
    for(int i=0; i<8; i++){
        msg_send.data[i] = msg2_data[i];   
    }
    
    EnableInterrupts;
  
    

  for(;;) {

}
