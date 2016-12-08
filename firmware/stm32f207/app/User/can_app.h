/**
  ******************************************************************************
  * @file    can_bootloader.h
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   ����CAN���ߵ�Bootloader����.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __CAN_APP_H
#define __CAN_APP_H
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
typedef  struct  _CBL_CMD_LIST{
	uint8_t EraseFlash;			//����APP������������
	uint8_t	WriteBlockFlash;	//�Զ��ֽ���ʽд����
	uint8_t	ReadBlockFlash;		//�Զ��ֽ���ʽ������
	uint8_t BlockWriteInfo;		//���ö��ֽ�д������ز�����д��ʼ��ַ����������
	uint8_t BlockReadInfo;		//���ö��ֽ����ݶ���ز���������ʼ��ַ����������
	uint8_t	OnlineCheck;		//���ڵ��Ƿ�����
	uint8_t	CmdSuccess;			//����ִ�гɹ�
	uint8_t	CmdFaild;			//����ִ��ʧ��
	uint8_t	SetBaudRate;		//���ýڵ㲨����
	uint8_t	ExcuteApp;			//ִ��Ӧ�ó���
	uint8_t	Undefine0;			//δ����
	uint8_t	Undefine1;			//δ����
	uint8_t	Undefine2;			//δ����
	uint8_t	Undefine3;			//δ����
} CBL_CMD_LIST,*PCBL_CMD_LIST; 
/* Private define ------------------------------------------------------------*/
#define CMD_WIDTH   4
#define CMD_MASK    0xF
#define ADDR_MASK   0x1FFFFFF

#define EXE_APP_FLAG					((uint32_t)0x08004000)
#define APP_START_ADDRESS			((uint32_t)0x08008000)	
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint16_t CAN_BOOT_GetAddrData(void);
void CAN_BOOT_ExecutiveCommand(CAN_TypeDef* CANx,CanRxMsg *pRxMessage);
void CAN_BOOT_JumpToApplication(uint32_t Addr);
FLASH_Status CAN_BOOT_ProgramDatatoFlash(uint32_t StartAddress,uint8_t *pData,uint32_t DataNum);

#endif
/*********************************END OF FILE**********************************/

