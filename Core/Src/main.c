/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <csp/csp.h>
#include <csp/csp_conn.h>
#include <csp/csp_promisc.h>
#include <stdio.h>

#include "../../CAN Driver/include/can_stm32.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location=0x2004c000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x2004c0a0
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x2004c000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x2004c0a0))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */

#endif

ETH_TxPacketConfig TxConfig;

CAN_HandleTypeDef hcan1;

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for cspSend */
osThreadId_t cspSendHandle;
const osThreadAttr_t cspSend_attributes = {
  .name = "cspSend",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for cspReceive */
osThreadId_t cspReceiveHandle;
const osThreadAttr_t cspReceive_attributes = {
  .name = "cspReceive",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* USER CODE BEGIN PV */
//uint8_t USB_InputBuffer[100];
//uint8_t USB_OutputBuffer[127];
//int USB_OutBufLen;
#define MY_SERVER_PORT		10
#define SERVER_ADDRESS      255

//static uint8_t server_port = 10;

//static bool test_mode = false;
//static unsigned int server_received = 0;

CAN_FilterTypeDef canfil;
//uint8_t canSend = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_CAN1_Init(void);
void StartDefaultTask(void *argument);
void CSPSend(void *argument);
void CSPReceive(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len) {
	for(int i=0; i<len; i++)
		ITM_SendChar((*ptr++));
	return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_CAN1_Init();

  /* USER CODE BEGIN 2 */
  /* Setting the debug level. Only debugging statements up to 'debug_level' will be printed to the debug terminal. */
//  csp_debug_level_t debug_level = CSP_INFO;
  csp_debug_level_t debug_level = CSP_PACKET;
  for (csp_debug_level_t i = 0; i <= CSP_LOCK; ++i) {
	  csp_debug_set_level(i, (i <= debug_level) ? true: false);
  }

  /* Initialising CSP */
  csp_log_info("Initialising CSP");
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
  uint8_t csp_address = 255;
  csp_conf.address = csp_address;
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
	  csp_log_error("csp_init() failed, error: %d", error);
  }
  /* Add interface(s) */
  csp_iface_t CSP_IF_CAN = {
		  .name = "CSP IF CAN",
		  .driver_data = &hcan1,
//		  .nexthop = csp_can_tx_stm32,
		  .mtu = 64,
  };
  csp_iface_t *can_iface = &CSP_IF_CAN;
  error = csp_can_stm32_open_and_add_interface(CSP_IF_CAN.name, &can_iface, 0);
  if (error != CSP_ERR_NONE) {
	  csp_log_error("csp_can_stm32_open_and_add_interface() failed, error: %d", error);
  }
  /* Setting route table */
  if (can_iface) {
//	  csp_rtable_set(CSP_DEFAULT_ROUTE, 0, can_iface, CSP_NO_VIA_ADDRESS);
	  csp_route_set(CSP_DEFAULT_ROUTE, can_iface, CSP_NO_VIA_ADDRESS);
  } else {
	  csp_log_info("Couldn't set route table");
//	  server_address = csp_address;
  }
  /* Start router task with 1000 bytes of stack (priority is only supported on FreeRTOS) */
  if(csp_route_start_task(1000, 1) != CSP_ERR_NONE){
	  csp_log_warn("Failed to start router!");
  }

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of cspSend */
  cspSendHandle = osThreadNew(CSPSend, NULL, &cspSend_attributes);

  /* creation of cspReceive */
  cspReceiveHandle = osThreadNew(CSPReceive, NULL, &cspReceive_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 4;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  canfil.FilterBank = 0;
  canfil.FilterMode = CAN_FILTERMODE_IDMASK;
  canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
  canfil.FilterIdHigh = 0;
  canfil.FilterIdLow = 0;
  canfil.FilterMaskIdHigh = 0;
  canfil.FilterMaskIdLow = 0;
  canfil.FilterScale = CAN_FILTERSCALE_32BIT;
  canfil.FilterActivation = ENABLE;
  canfil.SlaveStartFilterBank = 14;

  /* Initialise CAN Filter */
  if (HAL_CAN_ConfigFilter(&hcan1, &canfil) != HAL_OK) {
	  Error_Handler();
  }

  /* Activate CAN Interrupts */
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

  /* Start the CAN Peripheral */
  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
  Error_Handler();
  }
  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void SendSimpleCSPPacket(unsigned int size)
{
	unsigned int i;
	uint32_t timeout;

	timeout = 1000;

	/* Open connection */
	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, SERVER_ADDRESS, 10, timeout, CSP_SO_NONE);
	if (conn == NULL)
		return;

	/* Prepare data */
	csp_packet_t * packet = csp_buffer_get(size);
	if (packet == NULL)
		goto out;

	/* Set data to increasing numbers */
	packet->length = size;
	for (i = 0; i < size; i++)
		packet->data[i] = i;

	/* Try to send frame */
	if (!csp_send(conn, packet, 0))
		goto out;

	out:
		/* Clean up */
		csp_buffer_free(packet);
		csp_close(conn);
}


//CAN_RxHeaderTypeDef rxHeader;
//uint8_t rxData[8] = {0,0,0,0,0,0,0,0};
//
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
//	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData);
//	csp_log_warn((char*) rxHeader.StdId);
//}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
	  osDelay(200);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_CSPSend */
/**
* @brief Function implementing the cspSend thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CSPSend */
void CSPSend(void *argument)
{
  /* USER CODE BEGIN CSPSend */
  /* Infinite loop */
//  int ping_response = -1;
  for(;;)
  {
	  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

//	  SendSimpleCSPPacket(2);
//	  ping_response = csp_ping(6, 3000, 2, 0);
//	  if(ping_response >= 0) {
//		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
//	  }

	  osDelay(5000);
  }
  /* USER CODE END CSPSend */
}

/* USER CODE BEGIN Header_CSPReceive */
/**
* @brief Function implementing the cspReceive thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CSPReceive */
void CSPReceive(void *argument)
{
  /* USER CODE BEGIN CSPReceive */

  /* Create socket without any socket options */
//  csp_socket_t *sock = csp_socket(CSP_SO_CONN_LESS);
//  csp_socket_t *sock = csp_socket(CSP_SO_NONE);

  /* Bind all ports to socket */
//  if(csp_bind(sock, CSP_ANY) != CSP_ERR_NONE){
//	  csp_log_warn("Couldn't bind port to socket");
//  }

//  /* Create 5 connections backlog queue */
//  if(csp_listen(sock, 5) != CSP_ERR_NONE){
//	  csp_log_warn("Couldn't start listening");
//  }

  /* Pointer to current connection and packet */
//  csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, server_address, 10, 2000, CSP_SO_NONE);
//  if ((conn = csp_accept(&sock, 10000)) == NULL) {
//	  /* timeout */
//	  csp_log_warn("connection timeout");
//  }

  if(csp_promisc_enable(100) != CSP_ERR_NONE) {
	  csp_log_warn("Couldn't enable promiscuous mode.");
  }

  csp_packet_t *packet;
//  csp_conn_t *conn;

  /* Infinite loop */
  for(;;)
  {
	  /* Wait for connection, 10000ms timeout */
//	  if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) != NULL){
//		  continue;
//	  } else {
//		  csp_log_warn("failed miserably");
//	  }


//	  if((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL ) continue;
//
//	  /* Read packets. Timeout is 1000ms */
////	  while ((packet = csp_read(conn, 2000)) != NULL) {
//	  while ((packet = csp_recvfrom(sock, CSP_MAX_TIMEOUT)) != NULL) {
//		  csp_log_warn("received a packet!!!!");
//		  switch (csp_conn_dport(conn)) {
//			  case 9:
//				  /* Process packet here */
//				  goto delay;
//			  default:
//				  /* Let the service handler reply pings, buffer use, etc. */
//				  csp_service_handler(conn, packet);
//				  break;
//		  }
//	  }

//	  while(true) {
//		  conn = csp_accept(sock, 200);
//
//		  /* Listener loop (waits for packets) */
//		  while(true) {
//			  packet = csp_read(conn, 1000);
//			  if(packet == NULL) {
//				  csp_close(conn);
//				  break;
//			  }
//			  csp_log_warn("Packet received!\r\n");
//			  printf("Packet received\n");
//		  }
//		  if(conn != NULL) {
//			  csp_close(conn);
//		  }
//	  }

	  packet = csp_promisc_read(2000);

	  if(packet != NULL) {
//		  csp_log_warn("Packet Data: %d\nPacket Data 16: %d\n Packet Data 32: %d Packet Length: %d", (int)*packet->data, (int)*packet->data16, (int)*packet->data32, (int)packet->length);
//		  csp_log_packet("INP: S %u, D %u, Dp %u, Sp %u, Pr %u, Fl 0x%02X, Sz %"PRIu16" VIA: %s",
//		  			packet->id.src, packet->id.dst, packet->id.dport,
//		  			packet->id.sport, packet->id.pri, packet->id.flags, packet->length, input.iface->name);
		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		  int full_packet_buffer_size = packet->length*2+1;
		  char full_packet[full_packet_buffer_size];
		  full_packet[full_packet_buffer_size-1] = '\0';
		  for(int i=0; i<packet->length; i++) {
			  char str_buff[3];
			  snprintf(str_buff, 3, "%x", packet->data[i]);
			  strncpy(full_packet+(i*2), str_buff, 2);
		  }
//		  csp_log_info("Packet Data: 0x%x", packet->data[0]);
		  csp_log_info("Packet Data: 0x%s", full_packet);
		  csp_buffer_free(packet);
		  SendSimpleCSPPacket(4);
		  goto delay;
	  }



	  /* Close current connection, and handle next */
//	  csp_close(conn);

	  delay:
	  	  osDelay(10);
  }
  /* USER CODE END CSPReceive */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
