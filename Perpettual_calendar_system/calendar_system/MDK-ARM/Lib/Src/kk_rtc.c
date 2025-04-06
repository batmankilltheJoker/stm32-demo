#include "kk_rtc.h"

// RTC已经被初始化的值 记录在RTC_BKP_DR1中
#define RTC_INIT_FLAG 0x2333

// 对年月日时分秒进行定义
#define kk_year 2025
#define kk_mon 1
#define kk_mday 1
#define kk_hour 23
#define kk_min 59
#define kk_sec 59

/**
 * @brief  进入RTC初始化模式
 * @param  hrtc  指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* 等待RTC处于INIT状态，如果到达Time out 则退出 */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)/* 等待RTOFF位变为1 */
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* 禁用RTC寄存器的写保护 */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);/*将CNF 置1*/


  return HAL_OK;
}

/**
 * @brief  退出RTC初始化模式
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* 禁用RTC寄存器的写保护。 */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);/* 调用写保护开启宏定义 */

  tickstart = HAL_GetTick();
  /* 等到RTC处于INIT状态，如果到达Time out 则退出 */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)/* 等待RTOFF位变为1 */
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
 * @brief  写入RTC_CNT寄存器中的时间计数器。
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针。
 * @param  TimeCounter: 写入RTC_CNT寄存器的计数器
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* 进入RTC初始化模式 */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
	/* 设置RTC计数器高位寄存器 */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* 设置RTC计数器低位寄存器 */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* 退出RTC初始化模式 */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}


/**
 * @brief  读取RTC_CNT寄存器中的时间计数器。
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针。
 * @retval 时间计数器
 */
static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {
	/* 当读取CNTL和CNTH寄存器期间计数器溢出时, 重新读取CNTL寄存器然后返回计数器值 */
    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
	/* 当读取CNTL和CNTH寄存器期间没有计数器溢出, 计数器值等于第一次读取的CNTL和CNTH值 */
    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}

/**
 * @brief 设置RTC时间
 * @param time 时间
 * @retval HAL status
 */

HAL_StatusTypeDef KK_RTC_SetTime(struct tm *time){
	uint32_t unixTime = mktime(time);		 					/* 将年月日时分秒信息转化位unix时间戳 */		
	return RTC_WriteTimeCounter(&hrtc, unixTime);	/* 调用RTC_WriteTimeCounter函数，设置RTC的计数器为此时间戳 */
}

/**
 * @brief 获取RTC时间
 * @retval 时间
 */
struct tm *KK_RTC_GetTime(void) {
  time_t unixTime = RTC_ReadTimeCounter(&hrtc);
  // return gmtime(&unixTime);										/* 这里gmtime函数可能会出问题返回NULL 可以使用localtime函数 */
	return localtime(&unixTime);
}
/**
	* 通过判断备份寄存器中的值，实现RTC的一次性初始化和RTC时间的设置
	* 确保不会出现系统复位时时间变慢的BUG
	* 确保系统重新上电或复位时，不会重新设定时间
	*/
void KK_RTC_Init(void){
	uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);		/* 读取备份寄存器中的值 */
	if(initFlag == RTC_INIT_FLAG) return;													/* 如果是我们设置的数据，则代表已经设置过时间，无需再设置 */
	
	if (HAL_RTC_Init(&hrtc) != HAL_OK){		/* 判断是否初始化过RTC，已初始化后，则无需初始化 */
		Error_Handler();
	}
	struct tm time = {										/* 对要设置的时间进行初始化 */
		  .tm_year = kk_year - 1900,				/* 此成员变量要存储的是年份与1900年的差值 */
		  .tm_mon = kk_mon - 1,							/* 此成员变量从0开始计数 */
		  .tm_mday = kk_mday,
		  .tm_hour = kk_hour,
		  .tm_min = kk_min,
		  .tm_sec = kk_sec,
	};
	KK_RTC_SetTime(&time);			/* 写入RTC_CNT寄存器的计数器 */
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_INIT_FLAG); /* 写备份寄存器，对应参数：RTC句柄指针，写入的几号寄存器，写入的数据 */

}


