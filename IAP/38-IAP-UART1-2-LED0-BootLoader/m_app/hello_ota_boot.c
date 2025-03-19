#include "hello_ota_boot.h"
#include "main.h"
#include "stdio.h"

/*======================================================================*/
/*=========================Flash基本函数(start)=========================*/
/*======================================================================*/

/**
 * @bieaf 擦除页
 *
 * @param pageaddr  起始地址
 * @param num       擦除的页数
 * @return 1
 */
static int Erase_page(unsigned int pageaddr, unsigned int num)
{
    HAL_FLASH_Unlock();

    /* 擦除FLASH*/
    FLASH_EraseInitTypeDef FlashSet;
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
    FlashSet.PageAddress = pageaddr;
    FlashSet.NbPages = num;

    /*设置PageError, 调用擦除函数*/
    unsigned int PageError = 0;
    HAL_FLASHEx_Erase(&FlashSet, &PageError);

    HAL_FLASH_Lock();
    return 1;
}

/**
 * @bieaf 写若干个数据
 *
 * @param addr       写入的地址
 * @param buff       写入数据的起始地址
 * @param word_size  长度
 */
static void WriteFlash(unsigned int addr, unsigned int *buff, int word_size)
{
    /* 1/4解锁FLASH*/
    HAL_FLASH_Unlock();

    for (int i = 0; i < word_size; i++) {
        /* 3/4对FLASH烧写*/
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + 4 * i, buff[i]);
    }

    /* 4/4锁住FLASH*/
    HAL_FLASH_Lock();
}

/**
 * @bieaf 读若干个数据
 *
 * @param addr       读数据的地址
 * @param buff       读出数据的数组指针
 * @param word_size  长度
 */
static void ReadFlash(unsigned int addr, unsigned int *buff, uint16_t word_size)
{
    for (int i = 0; i < word_size; i++) {
        buff[i] = *(__IO unsigned int *)(addr + 4 * i);
    }
    return;
}

/*====================================================================*/
/*=========================Flash基本函数(end)=========================*/
/*====================================================================*/

/**
 * @bieaf 进行程序的覆盖
 *
 * @param  搬运的源地址
 * @param  搬运的目的地址
 * @return 搬运的程序大小
 */
void MoveCode(unsigned int src_addr, unsigned int des_addr, unsigned int byte_size)
{
    /*1.擦除目的地址*/
    printf("> Start erase des flash......\r\n");
    Erase_page(des_addr, (byte_size / PageSize));
    printf("> Erase des flash down......\r\n");

    /*2.开始拷贝*/
    unsigned int temp[256];

    printf("> Start copy......\r\n");
    for (int i = 0; i < byte_size / 1024; i++) {
        ReadFlash((src_addr + i * 1024), temp, 256);
        WriteFlash((des_addr + i * 1024), temp, 256);
    }
    printf("> Copy down......\r\n");

    /*3.擦除源地址*/
    printf("> Start erase src flash......\r\n");
    Erase_page(src_addr, (byte_size / PageSize));
    printf("> Erase src flash down......\r\n");
}

/* 修改启动模式 */
void Set_Start_Mode(unsigned int mode)
{
    //擦除一页 - 写数据
    Erase_page(ParamSetting_Addr, 1);
    WriteFlash(ParamSetting_Addr, &mode, 1);
}

/* 读取启动模式 */
unsigned int Read_Start_Mode(void)
{
    unsigned int mode = 0;
    ReadFlash(ParamSetting_Addr, &mode, 1);
    return mode;
}

// static unsigned int file_size = 0;  // 文件大小
// static unsigned int addr_now = 0;   // bin文件位置
// static int Read_Flag = 0;           // 升级准备就绪标志位

/* 串口2发送数据 */
void Uart2_Send(unsigned char *buf, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, 0xFFFF);
    HAL_Delay(5);
}

/* 发送指令 */
void send_command(unsigned char command)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&command, 1, 0xFFFF);
    HAL_Delay(10);
}

/**
 * @bieaf CRC-16 校验
 *
 * @param addr 开始地址
 * @param num   长度
 * @param num   CRC
 * @return crc  返回CRC的值
 */
#define POLY 0x1021
uint16_t crc16(unsigned char *addr, int num, uint16_t crc)
{
    for (int i; num > 0; num--) {                /* Step through bytes in memory */
        crc = crc ^ (*addr++ << 8);         /* Fetch byte from memory, XOR into CRC top byte*/
        for (i = 0; i < 8; i++) {           /* Prepare to rotate 8 bits */
            if (crc & 0x8000)               /* b15 is set... */
                crc = (crc << 1) ^ POLY;    /* rotate and XOR with polynomic */
            else                            /* b15 is clear... */
                crc <<= 1;                  /* just rotate */
        }                                   /* Loop for 8 bits */
        crc &= 0xFFFF;                      /* Ensure CRC remains 16-bit value */
    }                                       /* Loop until num=0 */
    return (crc);                           /* Return updated CRC */
}

/**
 * @bieaf 获取数据包的类型, 顺便进行校验
 *
 * @param buf 开始地址
 * @param len 长度
 * @return 1 -- 通过校验
 *         0 -- 未通过校验
 */
unsigned char Check_CRC(unsigned char *buf, int len)
{
    unsigned short crc = 0;
    if ((buf[0] == 0x00) && (len >= 133)) {
        crc = crc16(buf + 3, 128, crc);
        if (crc != (buf[131] << 8 | buf[132])) {
            return 0;
        }
        return 1;
    }
    return 0;
}

/* 设置升级的步骤 */
static enum UPDATE_STATE update_state = TO_START;
void Set_state(enum UPDATE_STATE state)
{
    update_state = state;
}

/* 查询升级的步骤 */
unsigned char Get_state(void)
{
    return update_state;
}

unsigned char temp_buf[512] = {0};
unsigned char temp_len = 0;

/* 数据处理函数 */
void Deal_Frame(unsigned char *buf, int len)
{
    /* 拷贝数据 */
    temp_len = Rx_Len;
    for (int i = 0; i < temp_len; i++) {
        temp_buf[i] = Rx_Buf[i];
    }

    switch (temp_buf[0]) 
		{
        case SOH: { // 数据包开始
            static unsigned char data_state = 0;
            if (Check_CRC(temp_buf, temp_len) == 0) {       // 未通过校验
                printf("> Notpass crc\r\n");
            }

            if ((Get_state() == TO_START) && (temp_buf[1] == 0x00) && (temp_buf[1] + temp_buf[2] == 0xFF)) {
                printf("> Receive start...\r\n");
                Set_state(TO_RECEIVE_DATA);
                send_command(ACK);
                send_command(CCC);
                Erase_page(Application_2_Addr, 40);         // 擦除App2
                data_state = 0x01;
            } else if ((Get_state() == TO_RECEIVE_DATA) && (temp_buf[1] + temp_buf[2] == 0xFF)) {
                printf("> Receive data bag:%d byte\r\n", data_state * 128);
                WriteFlash((Application_2_Addr + (data_state - 1) * 128), (uint32_t *)(&temp_buf[3]), 32);
                send_command(ACK);
                data_state++;
            } else if ((Get_state() == TO_RECEIVE_END) && (temp_buf[1] + temp_buf[2] == 0xFF)) {
                printf("> Receive end...\r\n");
                // Set_state(TO_START);
                send_command(ACK);
                HAL_NVIC_SystemReset();
            }
        } break;
        case EOT: {
            if (Get_state() == TO_RECEIVE_DATA) {
                printf("> Receive EOT1...\r\n");
                Set_state(TO_RECEIVE_EOT2);
                send_command(NACK);
            } else if (Get_state() == TO_RECEIVE_EOT2) {
                printf("> Receive EOT2...\r\n");
                send_command(ACK);
                send_command(CCC);
                Set_state(TO_RECEIVE_END);
                Set_Start_Mode(STARTUP_UPDATE);
            } else {
                printf("> Receive EOT, But error...\r\n");
            }
        } break;
    }
}

/**
 * @bieaf  获取升级文件的函数
 * @detail 在自检时间内收到升级命令就程序升级
 *
 * @param  int time_ms,自检时间
 */
void Get_OTA_File(unsigned int time_ms)
{
    unsigned int Waite_time = time_ms; // 自检时间
    printf("> Wait %dms...\r\n", time_ms);

    while (1) {
        if (Rx_Flag == 1) { // 收到一帧数据
            Rx_Flag = 0;
            Deal_Frame(Rx_Buf, Rx_Len);
        }

        if (Waite_time < 100) return;

        /* 发送命令包 */
        if (Get_state() == TO_START) {
            send_command(CCC);
            Waite_time -= 100;
            HAL_Delay(100);
        }
    }
}

/* 采用汇编设置栈的值 */
__asm void MSR_MSP(unsigned int ulAddr)
{
    MSR MSP, r0 // set Main Stack value
    BX r14
}

/* 程序跳转函数 */
typedef void (*Jump_Fun)(void);
void IAP_ExecuteApp(unsigned int App_Addr)
{
		
    Jump_Fun JumpToApp;
		
    if (((*(__IO unsigned int *)App_Addr) & 0x2FFE0000) == 0x20000000) //检查栈顶地址是否合法.
    {
			printf("IAP_ExecuteApp%x\r\n",App_Addr);
        JumpToApp = (Jump_Fun) * (__IO unsigned int *)(App_Addr + 4); //用户代码区第二个字为程序开始地址(复位地址)
        MSR_MSP(*(__IO unsigned int *)App_Addr); //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
        JumpToApp(); //跳转到APP.
    }
}

/* 进行BootLoader的启动 */
void Start_BootLoader(void)
{
		unsigned int x=0xFFFFFFFF;
    /*==========打印消息==========*/

		printf("\r\n");
    printf("*******************************\r\n");
		printf("   Program : BootLoader        \r\n");
    printf("    author : iotRobinV2        \r\n");
    printf("     time  : %s                \r\n", __TIME__);
    printf("*******************************\r\n");

    // 自检时间
    Get_OTA_File(1000);

    printf("> Choose a startup method......\r\n");
		x=Read_Start_Mode();
		printf("x=%x\r\n",x);
    switch (x) {            // 读取是否启动应用程序 */
        case STARTUP_NORMOL:                // 正常启动
            printf("> Normal start......\r\n");
            break;
        case STARTUP_UPDATE:                // 升级再启动
            printf("> Start update......\r\n");
//            MoveCode(Application_2_Addr, Application_1_Addr, Application_1_Size);
            printf("> Update down......\r\n");
            Set_Start_Mode(STARTUP_NORMOL);
            break;
        case STARTUP_OTANOW:                // 现在就升级
            printf("> OTA now......\r\n");
            Get_OTA_File(20000);
            break;
        case STARTUP_RESET:                 // 恢复出厂设置 目前没使用 */
            printf("> Restore to factory program......\r\n");
            printf("> Not currently supported......\r\n");
            break;
        default:                            // 启动失败
            printf("> Error:%X!!!......\r\n", Read_Start_Mode());
            return;
    }

    /* 跳转到应用程序 */
    printf("> Start up......\r\n\r\n");
    IAP_ExecuteApp(Application_2_Addr);
}
