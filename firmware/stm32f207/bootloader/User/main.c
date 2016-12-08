/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 
#include	"main.h"
#include "can_bootloader.h"
CBL_CMD_LIST CMD_List;
extern CanRxMsg CAN1_RxMessage,CAN2_RxMessage;
extern volatile uint8_t CAN1_CanRxMsgFlag;//���յ�CAN���ݺ�ı�־
extern volatile uint8_t CAN2_CanRxMsgFlag;
extern volatile uint8_t TimeOutFlag;				///<��ʱ����ʱ��־
volatile uint16_t BOOT_TimeOutCount;
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/**
  * @brief  ���ݵ�ַ�����������ó���������ʽ
  * @param  None
  * @retval None
  */
void BOOT_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	/*Configure Addr Pin*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	if(CAN_BOOT_GetAddrData()==0x00){
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		// ʹ������ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
		/* ʱ�Ӽ���Ƶ���� */
		/* 30M/30000 = 1ms */
		TIM_TimeBaseStructure.TIM_Prescaler = 60000-1;// Ԥ��Ƶ����Ƶ���Ƶ��Ϊ1K    
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //����ģʽ:���ϼ���
		// TIM������ֵ��������ʱʱ�䳤�ȣ�400*0.5ms=200ms
		TIM_TimeBaseStructure.TIM_Period =3000;	  // ��������ֵ�����ϼ���ʱ����������ֵ��������¼���ʱ���Ӹ�ֵ��ʼ����
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // �������˲����Ĳ������й�       
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //���¼�������ʼֵ
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

		TimeOutFlag = 0;
		
		/* Disable the TIM Counter */
		TIM2->CR1 &= (uint16_t)(~TIM_CR1_CEN);

		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //��������ж�
		
		/* Enable the TIM2 global Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;							// �ж�Դ
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	// ��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					// �����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						// ʹ���ж�
		NVIC_Init(&NVIC_InitStructure);		
		
		/* 	Enable the TIM Counter */
		TIM2->CR1 |= (uint16_t)TIM_CR1_CEN;
		while(CAN_BOOT_GetAddrData()==0x00){
			if(TimeOutFlag){
				TimeOutFlag = 0;
				__set_PRIMASK(1);//�ر������ж�
				FLASH_Unlock();
				CAN_BOOT_ErasePage(0x8004000,0x8008000);
				__set_PRIMASK(0);//���������ж�
			}
		}
		/* Disable the TIM Counter */
		TIM2->CR1 &= (uint16_t)(~TIM_CR1_CEN);
	}
	if((*((uint32_t *)0x08004000)==0x78563412)&&(*((uint32_t *)0x08008000)!=0xFFFFFFFF)){
		CAN_BOOT_JumpToApplication(0x8008000);	
	}
	__set_PRIMASK(0);//�������ж�
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000); //����ӳ���ж�������
}

/**
  * @brief  TIM��ʱ����ʱ�жϴ�����
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3,TIM_IT_Update)== SET)
  {
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //��������־
    BOOT_TimeOutCount++;
  }
}

/**
  * @brief  ���ݵ�ַ�����������ó���������ʽ
  * @param  None
  * @retval None
  */
void BOOT_Delay_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	__set_PRIMASK(0);//�������ж�
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000); //����ӳ���ж�������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// ʹ������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	/* ʱ�Ӽ���Ƶ���� */
	/* 30M/60000 = 1ms */
	TIM_TimeBaseStructure.TIM_Prescaler = 60000-1;// Ԥ��Ƶ����Ƶ���Ƶ��Ϊ1K
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //����ģʽ:���ϼ���
	// TIM������ֵ��������ʱʱ�䳤�ȣ�400*0.5ms=200ms
	TIM_TimeBaseStructure.TIM_Period =3;	  // ��������ֵ�����ϼ���ʱ����������ֵ��������¼���ʱ���Ӹ�ֵ��ʼ����
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // �������˲����Ĳ������й�       
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //���¼�������ʼֵ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TimeOutFlag = 0;
		
	/* Disable the TIM Counter */
	TIM3->CR1 &= (uint16_t)(~TIM_CR1_CEN);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //��������ж�
	
	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;							// �ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	// ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					// �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						// ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);		
		
	/* 	Enable the TIM Counter */
	TIM3->CR1 |= (uint16_t)TIM_CR1_CEN;
	BOOT_TimeOutCount = 0;
}



/** @defgroup APP_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */
int main(void)
{
//	int i;
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32fxxx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file
  */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000); //����ӳ���ж�������
  __set_PRIMASK(0);//�������ж�
  
  CAN_Configuration(CAN1,1000000);
  CAN_Configuration(CAN2,1000000);

  CMD_List.OnlineCheck = 0x01;
  CMD_List.EraseFlash = 0x03;
  CMD_List.SetBaudRate = 0x04;
  CMD_List.BlockWriteInfo = 0x05;
  CMD_List.WriteBlockFlash = 0x06;
  CMD_List.BlockReadInfo = 0x07;
  CMD_List.ReadBlockFlash = 0x08;
  CMD_List.ExcuteApp = 0x09;
  CMD_List.CmdFaild = 0x00;
  CMD_List.CmdSuccess = 0x0A;
// 	/* Enable the Flash option control register access */
// 	FLASH_OB_Unlock();
// 	/* Enable FLASH_WRP_SECTORS write protection */
// 	FLASH_OB_WRPConfig(OB_WRP_Sector_0, ENABLE); 
// 	FLASH_OB_Launch();
// 	FLASH_OB_Lock();	
	
// 	if(FLASH_OB_GetRDP() != SET)
// 	{
// 		FLASH_OB_Unlock();
// 		FLASH_OB_RDPConfig(OB_RDP_Level_1); 
// 		FLASH_OB_Launch();
// 		FLASH_OB_Lock();		
// 	}
  while (1)
  {
    if(CAN1_CanRxMsgFlag){
      CAN1_CanRxMsgFlag = 0;
      CAN_BOOT_ExecutiveCommand(CAN1,&CAN1_RxMessage);
    }
    if(CAN2_CanRxMsgFlag){
      CAN2_CanRxMsgFlag = 0;
      CAN_BOOT_ExecutiveCommand(CAN2,&CAN2_RxMessage);
    }
  }
} 

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
