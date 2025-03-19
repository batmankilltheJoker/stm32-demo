#include "hello_ota_app.h"
#include "main.h"
#include "stdio.h"
#include "ds18b20.h"
/*======================================================================*/
/*======================== Flash 基本函数(start) =======================*/
/*======================================================================*/

/**
 * @bieaf 擦除页
 *
 * @param pageaddr  页起始地址
 * @param num       擦除的页数
 * @return 1
 */
static int Erase_page(uint32_t pageaddr, uint32_t num)
{
    HAL_FLASH_Unlock();

    /* 擦除FLASH*/
    FLASH_EraseInitTypeDef FlashSet;
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
    FlashSet.PageAddress = pageaddr;
    FlashSet.NbPages = num;

    /*设置PageError，调用擦除函数*/
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&FlashSet, &PageError);

    HAL_FLASH_Lock();
    return 1;
}

/**
 * @bieaf 写若干个数据
 *
 * @param addr       写入的地址
 * @param buff       写入数据的数组指针
 * @param word_size  长度
 */
static void WriteFlash(uint32_t addr, uint32_t *buff, int word_size)
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

/*====================================================================*/
/*=========================Flash基本函数(end)=========================*/
/*====================================================================*/

/* 修改启动模式 */
void Set_Start_Mode(unsigned int Mode)
{
    // 擦除1页->写数据
    Erase_page(ParamSetting_Addr, 1);
    WriteFlash(ParamSetting_Addr, &Mode, 1);
}

/* 串口2发送数据 */
void Uart2_Send(unsigned char *buf, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, 0xFFFF);
    HAL_Delay(5);
}

unsigned char ymodem_command[] = "rb -E";

/*数据处理函数 */
void Deal_Frame(unsigned char *buf, int len)
{
    if (len <= 10) { // 开始 ("START")+(4字节地址)+(1字节校验) = 10
        if (buf[0] != ymodem_command[0]) return;
        if (buf[1] != ymodem_command[1]) return;
        if (buf[2] != ymodem_command[2]) return;
        if (buf[3] != ymodem_command[3]) return;
        if (buf[4] != ymodem_command[4]) return;

        //修改标志位
        Set_Start_Mode(Startup_Normol);
        printf("> File ok!, restart...\r\n");
        HAL_NVIC_SystemReset();
    }
}

/* app升级函数 */
void ota_app(void)
{
		uint16_t count=100;
    printf("\r\n");
    printf("*******************************\r\n");
		printf("   Program : 222154101127       \r\n");
    printf("    author : plj             	\r\n");
    printf("     time  : %s                \r\n", __TIME__);
		printf("	temperature :%.2f\r\n",ReadTemperature());
    printf("*******************************\r\n");
		
    while (count--) {
        if (Rx_Flag == 1) { // 收到一帧数据
            Rx_Flag = 0;
            Deal_Frame(Rx_Buf, Rx_Len);
        }
    }
}
