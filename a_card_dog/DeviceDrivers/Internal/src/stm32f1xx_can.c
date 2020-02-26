/**

作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_can.h"
#include "can_id.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
#include "dialogue.h"
/* 私有宏定义 ----------------------------------------------------------------*/
//	#define CAN1_USE_PA12_PA11 //使用CAN1_PA12/PA11
	#define CAN1_USE_PB9_PB8 //使用CAN1_PB9/PB8
//	#define CAN1_USE_PD1_PD0 //使用CAN1_PD1/PD0
//	#define CAN2_USE_PB6_PB5 //使用CAN2_PB6/PB5
//	#define CAN2_USE_PB13_PB12 //使用CAN2_PB13/PB12
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
static const uint32_t can_baud_table[9][4] =
{
#if defined(STM32F105xC) || defined(STM32F107xC)
    // 48 M
	{CAN_SJW_2TQ, CAN_BS1_12TQ, CAN_BS2_3TQ, 3},
	{CAN_SJW_2TQ, CAN_BS1_6TQ,  CAN_BS2_3TQ, 6},
	{CAN_SJW_2TQ, CAN_BS1_12TQ, CAN_BS2_3TQ, 5},
	{CAN_SJW_2TQ, CAN_BS1_12TQ, CAN_BS2_3TQ, 11},//1
	{CAN_SJW_2TQ, CAN_BS1_12TQ, CAN_BS2_3TQ, 23},
	{CAN_SJW_2TQ, CAN_BS1_12TQ, CAN_BS2_3TQ, 29},
	{CAN_SJW_2TQ, CAN_BS1_12TQ, CAN_BS2_3TQ, 59},
	{CAN_SJW_2TQ, CAN_BS1_14TQ, CAN_BS2_3TQ, 149},
	{CAN_SJW_2TQ, CAN_BS1_16TQ, CAN_BS2_8TQ, 199}
#else
	// 36 M
	{CAN_SJW_2TQ, CAN_BS1_8TQ,  CAN_BS2_3TQ, 3}, //1000
	{CAN_SJW_2TQ, CAN_BS1_11TQ, CAN_BS2_3TQ, 3}, //800
	{CAN_SJW_2TQ, CAN_BS1_9TQ,  CAN_BS2_2TQ, 6}, //500
	{CAN_SJW_2TQ, CAN_BS1_13TQ, CAN_BS2_2TQ, 9}, //250
	{CAN_SJW_2TQ, CAN_BS1_13TQ, CAN_BS2_2TQ, 18}, //125
	{CAN_SJW_2TQ, CAN_BS1_9TQ,  CAN_BS2_2TQ, 30}, //100
	{CAN_SJW_2TQ, CAN_BS1_13TQ, CAN_BS2_2TQ, 45}, //50
	{CAN_SJW_2TQ, CAN_BS1_14TQ, CAN_BS2_3TQ, 100}, //20
	{CAN_SJW_2TQ, CAN_BS1_14TQ, CAN_BS2_3TQ, 200} //10
#endif
};
/* 扩展变量 ------------------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
#ifdef USE_CAN2
CAN_HandleTypeDef hcan2;
#endif
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/* CAN init function */
void MX_CAN1_Init(Can_Band can_band)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_FilterTypeDef  sFilterConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 1
	__HAL_RCC_CAN1_FORCE_RESET();
	__HAL_RCC_CAN1_RELEASE_RESET();
#ifdef CAN1_USE_PA12_PA11
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);
#endif
#ifdef CAN1_USE_PB9_PB8
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
#endif
#ifdef CAN1_USE_PD1_PD0
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);
#endif
	HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
#endif
	__HAL_RCC_CAN1_CLK_ENABLE(); //CAN1时钟使能
//--------------------------------------------------------------------------------
#ifdef CAN1_USE_PA12_PA11
	//PA12     ------> CAN1_TX
	//PA11     ------> CAN1_RX
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//__HAL_AFIO_REMAP_CAN1_1();
#endif
#ifdef CAN1_USE_PB9_PB8
	//PB9     ------> CAN1_TX
	//PB8     ------> CAN1_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_AFIO_REMAP_CAN1_2();
#endif
#ifdef CAN1_USE_PD1_PD0
	//PD1     ------> CAN1_TX
	//PD0     ------> CAN1_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	__HAL_AFIO_REMAP_CAN1_3();
#endif
//--------------------------------------------------------------------------------
	HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
//--------------------------------------------------------------------------------
	hcan1.Instance = CAN1;
	//波特率(Baud)>800K 采样点(sample point):75%
	//波特率(Baud)>500K 采样点(sample point):80%
	//波特率(Baud)<=500K 采样点(sample point):87.5%
	hcan1.Init.Prescaler = can_baud_table[can_band][3];
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = can_baud_table[can_band][0];
	hcan1.Init.TimeSeg1 = can_baud_table[can_band][1];
	hcan1.Init.TimeSeg2 = can_baud_table[can_band][2];
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	s_error |= HAL_CAN_Init(&hcan1); //初始化CAN总线
	s_error |= HAL_CAN_Start(&hcan1); //启动CAN总线
//--------------------------------------------------------------------------------
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; //工作在标识符屏蔽位模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //过滤器位宽为单个32位。
	sFilterConfig.FilterMaskIdHigh = 0xFFFF; //过滤器高16位每位必须匹配
	sFilterConfig.FilterMaskIdLow = 0xFFFF; //过滤器低16位每位必须匹配
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; //过滤器被关联到FIFO 0
	sFilterConfig.FilterActivation = ENABLE; //使能过滤器
	sFilterConfig.SlaveStartFilterBank = 13; //为从CAN实例选择启动过滤器组
	
	sFilterConfig.FilterBank = 0; //CAN1 0~13
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_ONIY_ID<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_ONIY_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
	
	sFilterConfig.FilterBank = 1; //CAN1 0~13
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_MAIN<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_MAIN<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);

	s_error |= HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//--------------------------------------------------------------------------------
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1; //过滤器被关联到FIFO 1
	
	sFilterConfig.FilterBank = 8; //过滤器组1
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_FROM<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_FROM<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);

	sFilterConfig.FilterBank = 9; //CAN1 0~13
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_AEROSOL<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_AEROSOL<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);

	s_error |= HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_CAN1_Init(); OK\r\n"); 
	} else
	{
		printf("MX_CAN1_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
#ifdef USE_CAN2
void MX_CAN2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_FilterConfTypeDef  sFilterConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 1
	__HAL_RCC_CAN1_CLK_DISABLE();
	__HAL_RCC_CAN2_CLK_DISABLE();
#ifdef CAN2_USE_PB13_PB12
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);
#endif
#ifdef CAN2_USE_PB6_PB5
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5|GPIO_PIN_6);
#endif
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
#endif
	__HAL_RCC_CAN2_CLK_ENABLE();
//--------------------------------------------------------------------------------
#ifdef CAN2_USE_PB13_PB12
    //PB13     ------> CAN2_TX
    //PB12     ------> CAN2_RX
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    //__HAL_AFIO_REMAP_CAN2_DISABLE();
#endif
#ifdef CAN2_USE_PB6_PB5
    //PB6     ------> CAN2_TX
    //PB5     ------> CAN2_RX
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    __HAL_AFIO_REMAP_CAN2_ENABLE();
#endif
//--------------------------------------------------------------------------------
	HAL_NVIC_SetPriority(USB_LP_CAN2_RX0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(USB_LP_CAN2_RX0_IRQn);
//--------------------------------------------------------------------------------
	hcan2.Instance = CAN2;
	//波特率(Baud)>800K 采样点(sample point):75%
	//波特率(Baud)>500K 采样点(sample point):80%
	//波特率(Baud)<=500K 采样点(sample point):87.5%
	hcan2.Init.Prescaler = 45;
	hcan2.Init.Mode = CAN_MODE_NORMAL;
	hcan2.Init.SyncJumpWidth = CAN_SJW_2TQ;
	hcan2.Init.TimeSeg1 = CAN_BS1_6TQ;
	hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan2.Init.TimeTriggeredMode = DISABLE;
	hcan2.Init.AutoBusOff = DISABLE;
	hcan2.Init.AutoWakeUp = DISABLE;
	hcan2.Init.AutoRetransmission = DISABLE;
	hcan2.Init.ReceiveFifoLocked = DISABLE;
	hcan2.Init.TransmitFifoPriority = DISABLE;
	s_error |= HAL_CAN_Init(&hcan2); //初始化CAN总线
	s_error |= HAL_CAN_Start(&hcan2); //启动CAN总线
	s_error |= HAL_CAN_Init(&hcan2);
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; //工作在标识符屏蔽位模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //过滤器位宽为单个32位。
	sFilterConfig.FilterMaskIdHigh = 0xFFFF; //过滤器高16位每位必须匹配
	sFilterConfig.FilterMaskIdLow = 0xFFFF; //过滤器低16位每位必须匹配
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; //过滤器被关联到FIFO 0
	sFilterConfig.FilterActivation = ENABLE; //使能过滤器
	sFilterConfig.SlaveStartFilterBank = 13; //CAN2 14~27
	
	sFilterConfig.FilterBank = 0; //CAN1 0~13
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_ONIY_ID<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_ONIY_ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig); //这里必须 写成 can1
	
	sFilterConfig.FilterBank = 1; //CAN1 0~13
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_MAIN<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_MAIN<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig); //这里必须 写成 can1
	
	sFilterConfig.FilterBank = 2; //CAN1 0~13
	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_FROM<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
	sFilterConfig.FilterIdLow = (((uint32_t)CAN_FROM<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig); //这里必须 写成 can1
	
	s_error |= HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//--------------------------------------------------------------------------------
//	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1; //过滤器被关联到FIFO 1
	
//	sFilterConfig.FilterBank = 8; //过滤器组1
//	sFilterConfig.FilterIdHigh = (((uint32_t)CAN_MAIN<<3)&0xFFFF0000)>>16; //要过滤的ID高位 
//	sFilterConfig.FilterIdLow = (((uint32_t)CAN_MAIN<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位
//	s_error |= HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig); //这里必须 写成 can1
	
//	s_error |= HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_CAN2_Init(); OK\r\n"); 
	} else
	{
		printf("MX_CAN2_Init(); ERROR\r\n"); 
	}
#endif
}
#endif // USE_CAN2
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: CAN1通信报文内容设置
  * 输入参数: 无 地址 模式 (标准:CAN_ID_STD 扩展CAN_ID_EXT) 类校(数据:CAN_RTR_DATA 远程:CAN_RTR_REMOTE) 长度 数据
  * 返 回 值: 无
  * 说    明：无
  */
void CAN1_send_message(uint8_t TxDataBuffer[], uint32_t p_frame_id, uint8_t p_length)
{
	CAN_TxHeaderTypeDef pTxHeader;
	uint32_t TxMailbox;   //CAN_TX_MAILBOX0邮箱作为TX发送邮箱

	//pTxHeader.StdId = p_frame_id; //标准标识符
	pTxHeader.ExtId = p_frame_id; //扩展标识符(29位)
	pTxHeader.IDE = CAN_ID_EXT; //使用标准帧
	pTxHeader.RTR = CAN_RTR_DATA; //数据帧
	pTxHeader.DLC = p_length; //指定要发送的数据个数（字节数）
	pTxHeader.TransmitGlobalTime = DISABLE;

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {;}
//	_delay_us(1000);
//将* TXDataBuffer指针变量指向某个数组，将数组里的数据增加到TX邮箱，（具体增加到哪个具体邮箱，由单片机自行分配），
//单片机将分配的TX邮箱编号存入TxMailbox数组
	if (HAL_CAN_AddTxMessage(&hcan1, &pTxHeader, TxDataBuffer, (uint32_t*)&TxMailbox) != HAL_OK)
	{
		printf("error\r\n");
	} else
	{
	}
}
#ifdef USE_CAN2
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: CAN2通信报文内容设置
  * 输入参数: 无 地址 模式 (标准:CAN_ID_STD 扩展CAN_ID_EXT) 类校(数据:CAN_RTR_DATA 远程:CAN_RTR_REMOTE) 长度 数据
  * 返 回 值: 无
  * 说    明：无
  */
void  CAN2_send_message(uint8_t* TxDataBuffer, uint32_t p_frame_id, uint8_t p_length)
{
	CAN_TxHeaderTypeDef pTxHeader;
	uint32_t TxMailbox;   //CAN_TX_MAILBOX0邮箱作为TX发送邮箱

	pTxHeader.StdId = p_frame_id; //标准标识符
	pTxHeader.ExtId = p_frame_id; //扩展标识符(29位)
	pTxHeader.IDE = CAN_ID_EXT; //使用标准帧
	pTxHeader.RTR = CAN_RTR_DATA; //数据帧
	pTxHeader.DLC = p_length; //指定要发送的数据个数（字节数）
	pTxHeader.TransmitGlobalTime = DISABLE;

	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {;}
//	_delay_us(1000);
//将* TXDataBuffer指针变量指向某个数组，将数组里的数据增加到TX邮箱，（具体增加到哪个具体邮箱，由单片机自行分配），
//单片机将分配的TX邮箱编号存入TxMailbox数组
	if (HAL_CAN_AddTxMessage(&hcan2, &pTxHeader, TxDataBuffer, (uint32_t*)&TxMailbox) != HAL_OK)
	{
		printf("error\r\n");
	} else
	{
	}
}
#endif
/*----------------------------------------------------------------------------*/
/**
  * @brief 接收FIFO 0完全回调
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef pRxHeader;
	uint8_t Rx_buffer[8];
	//union Convert{{uint8_t the_switch;uint8_t the_switch;uint8_t the_switch;} float f;}buffer;
	if(hcan->Instance==CAN1)
	{
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &pRxHeader, Rx_buffer) == HAL_OK)
		{
#if 1 // 显示收到的数据
			char s_display[128] = {'\0','\0','\0','\0','\0',}, s_temp[32];
			uint8_t i, s_number;
			if (hcan->Instance == CAN1) {s_number = 1;}
			#ifdef USE_CAN2
			else if (hcan->Instance == CAN2) {s_number = 2;}
			#endif // USE_CAN2
			sprintf(s_temp,"CAN%d-0:{%.8X;",s_number, pRxHeader.ExtId), strcat(s_display, s_temp); // ID
			if (pRxHeader.IDE == CAN_ID_STD) {sprintf(s_temp,"STD;"), strcat(s_display, s_temp);} // 贞类型
			if (pRxHeader.IDE == CAN_ID_EXT) {sprintf(s_temp,"EXT;"), strcat(s_display, s_temp);}
			for (i=0; i<pRxHeader.DLC; i++) {sprintf(s_temp,"%.2X;", Rx_buffer[i]), strcat(s_display, s_temp);} // 数据
			sprintf(s_temp,"%d}",pRxHeader.DLC), strcat(s_display, s_temp); // 长度
			printf("%s\r\n",s_display);
#endif
			switch (pRxHeader.ExtId&0xFFFFFF00)
			{
				case CAN_ONIY_ID: //读取唯一ID
					if ((pRxHeader.DLC == 0)&&((pRxHeader.ExtId&0x000000FF) == 0))
					{
						uint8_t (*id)[12] = (uint8_t (*)[12])(0x1FFFF7E8);
						Rx_buffer[0] = (*id)[0];
						Rx_buffer[1] = (*id)[1];
						Rx_buffer[2] = (*id)[2];
						Rx_buffer[3] = (*id)[3];
						Rx_buffer[4] = (*id)[4];
						Rx_buffer[5] = (*id)[5];
						CAN1_send_message((uint8_t*)&Rx_buffer, 0x00000001, 6);

						Rx_buffer[0] = (*id)[6];
						Rx_buffer[1] = (*id)[7];
						Rx_buffer[2] = (*id)[8];
						Rx_buffer[3] = (*id)[9];
						Rx_buffer[4] = (*id)[10];
						Rx_buffer[5] = (*id)[11];
						CAN1_send_message((uint8_t*)&Rx_buffer, 0x00000002, 6);
					}
				break;
				default: break;
			}
		}
	}
#ifdef USE_CAN2
	else if (hcan->Instance == CAN2)
	{

	}
#endif // USE_CAN2
}
/*----------------------------------------------------------------------------*/
/**
  * @brief 接收FIFO 1完全回调
  */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef pRxHeader;
	uint8_t Rx_buffer[8];
	//union Convert{{uint8_t the_switch;uint8_t the_switch;uint8_t the_switch;} float f;}buffer;
	
	if(hcan->Instance==CAN1)
	{
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &pRxHeader, Rx_buffer) == HAL_OK)
		{
#if 1 // 显示收到的数据
			char s_display[128] = {'\0','\0','\0','\0','\0',}, s_temp[32];
			uint8_t i, s_number;
			if (hcan->Instance == CAN1) {s_number = 1;}
			#ifdef USE_CAN2
			else if (hcan->Instance == CAN2) {s_number = 2;}
			#endif // USE_CAN2
			sprintf(s_temp,"CAN%d-1:{%.8X;",s_number, pRxHeader.ExtId), strcat(s_display, s_temp); // ID
			if (pRxHeader.IDE == CAN_ID_STD) {sprintf(s_temp,"STD;"), strcat(s_display, s_temp);} // 贞类型
			if (pRxHeader.IDE == CAN_ID_EXT) {sprintf(s_temp,"EXT;"), strcat(s_display, s_temp);}
			for (i=0; i<pRxHeader.DLC; i++) {sprintf(s_temp,"%.2X;", Rx_buffer[i]), strcat(s_display, s_temp);} // 数据
			sprintf(s_temp,"%d}",pRxHeader.DLC), strcat(s_display, s_temp); // 长度
			printf("%s\r\n",s_display);
#endif
			switch (pRxHeader.ExtId&0xFFFFFF00)
			{
				case CAN_ONIY_ID: //读取唯一ID
					if ((pRxHeader.DLC == 0)&&((pRxHeader.ExtId&0x000000FF) == 0))
					{
						uint8_t (*id)[12] = (uint8_t (*)[12])(0x1FFFF7E8);
						Rx_buffer[0] = (*id)[0];
						Rx_buffer[1] = (*id)[1];
						Rx_buffer[2] = (*id)[2];
						Rx_buffer[3] = (*id)[3];
						Rx_buffer[4] = (*id)[4];
						Rx_buffer[5] = (*id)[5];
						CAN1_send_message((uint8_t*)&Rx_buffer, 0x00000001, 6);

						Rx_buffer[0] = (*id)[6];
						Rx_buffer[1] = (*id)[7];
						Rx_buffer[2] = (*id)[8];
						Rx_buffer[3] = (*id)[9];
						Rx_buffer[4] = (*id)[10];
						Rx_buffer[5] = (*id)[11];
						CAN1_send_message((uint8_t*)&Rx_buffer, 0x00000002, 6);
					}
				break;
				default: break;
			}
		}
	}
#ifdef USE_CAN2
	else if (hcan->Instance == CAN2)
	{

	}
#endif // USE_CAN2
}
/*----------------------------------------------------------------------------*/
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
#if 0 // 显示 错误
	uint8_t s_number;
	if (hcan->Instance == CAN1) {s_number = 1;}
#ifdef USE_CAN2
	else if (hcan->Instance == CAN2) {s_number = 2;}
#endif // USE_CAN2
	switch (hcan->ErrorCode)
	{
		case HAL_CAN_ERROR_EWG: printf("CAN%d ERRROR = EWG\r\n", s_number); break;
		case HAL_CAN_ERROR_EPV: printf("CAN%d ERRROR = EPV\r\n", s_number); break;
		case HAL_CAN_ERROR_BOF: printf("CAN%d ERRROR = BOF\r\n", s_number); break;
		case HAL_CAN_ERROR_STF: printf("CAN%d ERRROR = Stuff\r\n", s_number); break;
		case HAL_CAN_ERROR_FOR: printf("CAN%d ERRROR = Form\r\n", s_number); break;
		case HAL_CAN_ERROR_ACK: printf("CAN%d ERRROR = ACK\r\n", s_number); break;
		case HAL_CAN_ERROR_BR: printf("CAN%d ERRROR = Bit recessive\r\n", s_number); break;
		case HAL_CAN_ERROR_BD: printf("CAN%d ERRROR = LEC dominant\r\n", s_number); break;
		case HAL_CAN_ERROR_CRC: printf("CAN%d ERRROR = LEC transfer\r\n", s_number); break;
		case HAL_CAN_ERROR_NONE: printf("CAN%d ERRROR = NO\r\n", s_number); break;
		default: printf("CAN%d ERRROR = Unknown\r\n", s_number); break;
	}
#endif
//--------------------------------------------------------------------------------
	hcan->ErrorCode = HAL_CAN_ERROR_NONE;
	hcan->State = HAL_CAN_STATE_READY;
	hcan->Instance->IER = 0; // 清除全部中断
	hcan->Instance->ESR = 0; // 清除错误计数
	HAL_CAN_DeInit(hcan);
	if (hcan->Instance == CAN1) {MX_CAN1_Init(CAN_BAND_125);}
#ifdef USE_CAN2
	else if (hcan->Instance == CAN2) {MX_CAN2_Init(CAN_BAND_125);}
#endif // USE_CAN2
}
/*----------------------------------------------------------------------------*/
