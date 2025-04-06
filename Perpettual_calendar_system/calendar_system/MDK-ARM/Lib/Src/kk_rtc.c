#include "kk_rtc.h"

// RTC�Ѿ�����ʼ����ֵ ��¼��RTC_BKP_DR1��
#define RTC_INIT_FLAG 0x2333

// ��������ʱ������ж���
#define kk_year 2025
#define kk_mon 1
#define kk_mday 1
#define kk_hour 23
#define kk_min 59
#define kk_sec 59

/**
 * @brief  ����RTC��ʼ��ģʽ
 * @param  hrtc  ָ�����RTC������Ϣ��RTC_HandleTypeDef�ṹ���ָ��
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* �ȴ�RTC����INIT״̬���������Time out ���˳� */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)/* �ȴ�RTOFFλ��Ϊ1 */
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* ����RTC�Ĵ�����д���� */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);/*��CNF ��1*/


  return HAL_OK;
}

/**
 * @brief  �˳�RTC��ʼ��ģʽ
 * @param  hrtc   ָ�����RTC������Ϣ��RTC_HandleTypeDef�ṹ���ָ��
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* ����RTC�Ĵ�����д������ */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);/* ����д���������궨�� */

  tickstart = HAL_GetTick();
  /* �ȵ�RTC����INIT״̬���������Time out ���˳� */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)/* �ȴ�RTOFFλ��Ϊ1 */
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
 * @brief  д��RTC_CNT�Ĵ����е�ʱ���������
 * @param  hrtc   ָ�����RTC������Ϣ��RTC_HandleTypeDef�ṹ���ָ�롣
 * @param  TimeCounter: д��RTC_CNT�Ĵ����ļ�����
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* ����RTC��ʼ��ģʽ */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
	/* ����RTC��������λ�Ĵ��� */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* ����RTC��������λ�Ĵ��� */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* �˳�RTC��ʼ��ģʽ */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}


/**
 * @brief  ��ȡRTC_CNT�Ĵ����е�ʱ���������
 * @param  hrtc   ָ�����RTC������Ϣ��RTC_HandleTypeDef�ṹ���ָ�롣
 * @retval ʱ�������
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
	/* ����ȡCNTL��CNTH�Ĵ����ڼ���������ʱ, ���¶�ȡCNTL�Ĵ���Ȼ�󷵻ؼ�����ֵ */
    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
	/* ����ȡCNTL��CNTH�Ĵ����ڼ�û�м��������, ������ֵ���ڵ�һ�ζ�ȡ��CNTL��CNTHֵ */
    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}

/**
 * @brief ����RTCʱ��
 * @param time ʱ��
 * @retval HAL status
 */

HAL_StatusTypeDef KK_RTC_SetTime(struct tm *time){
	uint32_t unixTime = mktime(time);		 					/* ��������ʱ������Ϣת��λunixʱ��� */		
	return RTC_WriteTimeCounter(&hrtc, unixTime);	/* ����RTC_WriteTimeCounter����������RTC�ļ�����Ϊ��ʱ��� */
}

/**
 * @brief ��ȡRTCʱ��
 * @retval ʱ��
 */
struct tm *KK_RTC_GetTime(void) {
  time_t unixTime = RTC_ReadTimeCounter(&hrtc);
  // return gmtime(&unixTime);										/* ����gmtime�������ܻ�����ⷵ��NULL ����ʹ��localtime���� */
	return localtime(&unixTime);
}
/**
	* ͨ���жϱ��ݼĴ����е�ֵ��ʵ��RTC��һ���Գ�ʼ����RTCʱ�������
	* ȷ���������ϵͳ��λʱʱ�������BUG
	* ȷ��ϵͳ�����ϵ��λʱ�����������趨ʱ��
	*/
void KK_RTC_Init(void){
	uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);		/* ��ȡ���ݼĴ����е�ֵ */
	if(initFlag == RTC_INIT_FLAG) return;													/* ������������õ����ݣ�������Ѿ����ù�ʱ�䣬���������� */
	
	if (HAL_RTC_Init(&hrtc) != HAL_OK){		/* �ж��Ƿ��ʼ����RTC���ѳ�ʼ�����������ʼ�� */
		Error_Handler();
	}
	struct tm time = {										/* ��Ҫ���õ�ʱ����г�ʼ�� */
		  .tm_year = kk_year - 1900,				/* �˳�Ա����Ҫ�洢���������1900��Ĳ�ֵ */
		  .tm_mon = kk_mon - 1,							/* �˳�Ա������0��ʼ���� */
		  .tm_mday = kk_mday,
		  .tm_hour = kk_hour,
		  .tm_min = kk_min,
		  .tm_sec = kk_sec,
	};
	KK_RTC_SetTime(&time);			/* д��RTC_CNT�Ĵ����ļ����� */
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_INIT_FLAG); /* д���ݼĴ�������Ӧ������RTC���ָ�룬д��ļ��żĴ�����д������� */

}


