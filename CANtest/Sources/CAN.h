struct can_msg     //发送报文的结构体
{
    unsigned int id;
    Bool RTR;
    unsigned char data[8];
    unsigned char len;
    unsigned char prty;
};

void INIT_CAN0(void);
void INIT_CAN1(void); 
Bool MSCAN0SendMsg(struct can_msg msg);
Bool MSCAN0GetMsg(struct can_msg *msg);
Bool MSCAN1SendMsg(struct can_msg msg);
Bool MSCAN1GetMsg(struct can_msg *msg);
