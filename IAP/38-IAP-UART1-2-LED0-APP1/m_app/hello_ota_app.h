#ifndef _HELLO_OTA_APP_H
#define _HELLO_OTA_APP_H
#ifdef __cplusplus
extern "C" {
#endif

/*=====用户配置(根据自己的分区进行配置)=====*/

#define PageSize        FLASH_PAGE_SIZE            //1K

#define BootLoader_addr           0x08000000U      // BootLoader的首地址
#define Application_1_Addr        0x08005000U      // 应用程序1的首地址
#define Application_2_Addr        0x0801A000U      // 应用程序2的首地址
#define Application_3_Addr        0x0802F000U      // 应用程序3的首地址
#define ParamSetting_Addr         0x0801F000U      // 参数设置的首地址

#define BootLoader_Size           0x05000U         // BootLoader的大小 20K
#define Application_1_Size        0x15000U         // 应用程序的大小 84K
#define Application_2_Size        0x15000U         // 应用程序的大小 84K
#define Application_3_Size        0x10000U         // 应用程序的大小 64K
#define ParamSetting_Size         0x01000U         // 应用程序的大小 4K

/* 启动的步骤 */
#define Startup_Normol 0xFFFFFFFF    // 正常启动
#define Startup_Update 0xAAAAAAAA    // 升级再启动
#define Startup_OtaNow 0x55555555    // 现在升级
#define Startup_Reset  0x5555AAAA    // ***恢复出厂 目前没使用***

/*==========================================*/

void Uart2_Send(unsigned char * buf, int len);
void Deal_Frame(unsigned char * buf, int len);
void ota_app(void);

#ifdef __cplusplus
}
#endif

#endif /* __YMODEM_H */
