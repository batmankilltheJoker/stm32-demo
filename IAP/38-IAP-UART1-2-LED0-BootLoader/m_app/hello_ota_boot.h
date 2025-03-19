#ifndef _HELLO_OTA_BOOT_H_
#define _HELLO_OTA_BOOT_H_



/****** 用户配置(根据自己的分区进行配置) ******/

#define PageSize        FLASH_PAGE_SIZE             // 2K

#define BootLoader_addr            0x08000000U      // BootLoader的首地址
#define Application_1_Addr        0x08005000U       // 应用程序1的首地址
#define Application_2_Addr        0x0801A000U       // 应用程序2的首地址
#define Application_3_Addr        0x0802F000U       // 应用程序3的首地址
#define ParamSetting_Addr        0x0803F000U       // 参数设置的首地址

#define BootLoader_Size         0x5000U             // BootLoader的大小 20K
#define Application_1_Size        0x15000U             // 应用程序的大小 84K
#define Application_2_Size        0x15000U             // 应用程序的大小 84K
#define Application_3_Size        0x10000U             // 应用程序的大小 64K
#define ParamSetting_Size        0x1000U             // 应用程序的大小 4K


/* 启动的步骤 */
#define STARTUP_NORMOL 0xFFFFFFFF    // 正常启动
#define STARTUP_UPDATE 0xAAAAAAAA    // 升级再启动
#define STARTUP_OTANOW 0x55555555    // 现在升级
#define STARTUP_RESET  0x5555AAAA    // ***恢复出厂 目前没使用***

/*==========================================*/

#define SOH        0x01
#define STX        0x02
#define ACK        0x06
#define NACK       0x15
#define EOT        0x04
#define CCC        0x43

/* 升级的步骤 */
enum UPDATE_STATE {
    TO_START = 0x01,
    TO_RECEIVE_DATA = 0x02,
    TO_RECEIVE_EOT1 = 0x03,
    TO_RECEIVE_EOT2 = 0x04,
    TO_RECEIVE_END = 0x05,
};

int  Erase_page(unsigned int pageaddr, unsigned int num);
void WriteFlash(unsigned int addr, unsigned int * buff, int word_size);

void Uart2_Send(unsigned char * buf, int len);
void Deal_Frame(unsigned char * buf, int len);

void Start_BootLoader(void);

#endif

