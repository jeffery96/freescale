#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "CAN.h"

#define  BUS_CLOCK		   32000000	   //总线频率
#define  OSC_CLOCK		   16000000	   //晶振频率

#define ID1 0x7DF
#define ID2 0x7E8
#define data_len_TX 8

unsigned char msg1_data[8] = {0x02, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char msg2_data[8] = {0x06, 0x41, 0x0, 0xBE, 0x3E, 0xB8, 0x11, 0x0};


struct can_msg msg_send, msg_get;   //发送接收数据报文

msg_send.id = ID2;
msg_send.len = data_len_TX;
msg_send.RTR = FALSE;
msg_send.prty = 0;


void INIT_PLL(void) {

    CRGINT = 0;                  //关中断
    CLKSEL_PLLSEL = 0;           //在未初始化PLL前不使用PLL的输出作为CPU时钟
        
    #if(BUS_CLOCK == 40000000) 
        SYNR = 4;
    #elif(BUS_CLOCK == 32000000)
        SYNR = 3;     
    #elif(BUS_CLOCK == 24000000)
        SYNR = 2;
    #endif 

    REFDV = 1;                   //PLLCLK=2×OSCCLK×(SYNR+1)/(REFDV+1)＝64MHz ,fbus=32M
    PLLCTL_PLLON = 1;            //开PLL
    PLLCTL_AUTO = 1;             //选取自动模式
    while (CRGFLG_LOCK == 0);    //等待PLL锁定频率
        CLKSEL_PLLSEL = 1;       //选择系统时钟由PLL产生
        
}


void initialize_ect(void){

    TSCR1_TFFCA = 1;  // 定时器标志位快速清除
    TSCR1_TEN = 1;    // 定时器使能位. 1=允许定时器正常工作; 0=使主定时器不起作用(包括计数器)
    TIOS  = 0xff;      //指定所有通道为输出比较方式
    TCTL1 = 0x00;	    // 后四个通道设置为定时器与输出引脚断开
    TCTL2 = 0x00;     // 前四个通道设置为定时器与输出引脚断开
    DLYCT = 0x00;	    // 延迟控制功能禁止
    ICOVW = 0x00;	    // 对应的寄存器允许被覆盖;  NOVWx = 1, 对应的寄存器不允许覆盖
    ICSYS = 0x00;	    // 禁止IC及PAC的保持寄存器
    TIE   = 0x00;     // 禁止所有通道定时中断
    TSCR2 = 0x07;	    // 预分频系数pr2-pr0:111,,时钟周期为4us,
    TFLG1 = 0xff;	    // 清除各IC/OC中断标志位
    TFLG2 = 0xff;     // 清除自由定时器中断标志位
  
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
void interrupt CAN_receive(void) 
{
    if(MSCAN1GetMsg(&msg_get)) 
    {
        // 接收新信息
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
