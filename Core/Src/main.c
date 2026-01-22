/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "ADC.h"
#include "ModbusRTU_Slave.h"
#include "flash.h"
#include "RMS.h"
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
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart4;

/* Definitions for ADC_data */
osThreadId_t ADC_dataHandle;
const osThreadAttr_t ADC_data_attributes = {
  .name = "ADC_data",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for modbus */
osThreadId_t modbusHandle;
const osThreadAttr_t modbus_attributes = {
  .name = "modbus",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ethernet */
osThreadId_t ethernetHandle;
const osThreadAttr_t ethernet_attributes = {
  .name = "ethernet",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Flash */
osThreadId_t FlashHandle;
const osThreadAttr_t Flash_attributes = {
  .name = "Flash",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Diod */
osThreadId_t DiodHandle;
const osThreadAttr_t Diod_attributes = {
  .name = "Diod",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for RMS */
osThreadId_t RMSHandle;
const osThreadAttr_t RMS_attributes = {
  .name = "RMS",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_UART4_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM6_Init(void);
void Task_ADC_Data(void *argument);
void Task_ModBus(void *argument);
void Task_Ethernet(void *argument);
void Task_Flash_data(void *argument);
void Task_Diod(void *argument);
void Task_RMS(void *argument);

static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ��� ���(

    uint8_t ADC_data8[256] = {0};
    int16_t ADC_16[8] = {0};
    
    extern uint8_t ADC_rx_data[19];

    uint8_t experement = 0;
    uint32_t SPI1_CR1 = 0;
    uint32_t cikl = 0;
    



// ��� ���)
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  RMS_Init(&ch[0], 3200);
  RMS_Init(&ch[1], 3200);
  RMS_Init(&ch[2], 3200);
  RMS_Init(&ch[3], 3200);
  RMS_Init(&ch[4], 3200);
  RMS_Init(&ch[5], 3200);
  RMS_Init(&ch[6], 3200);
  RMS_Init(&ch[7], 3200);
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
  MX_DMA_Init();
  MX_UART4_Init();
  MX_SPI2_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  ADS131E0_Init();
  flash_Init();
  

  
  // Инициализация Modbus
    DataCounter = 0;
    RxInterruptFlag = RESET;
    uartTimeCounter = 0;
    uartPacketComplatedFlag = RESET;
    HAL_UART_Receive_IT(&huart4, &uartRxData, 1);  // Включаем прерывание по приему

     
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
  /* creation of ADC_data */
  ADC_dataHandle = osThreadNew(Task_ADC_Data, NULL, &ADC_data_attributes);

  /* creation of modbus */
  modbusHandle = osThreadNew(Task_ModBus, NULL, &modbus_attributes);

  /* creation of ethernet */
  ethernetHandle = osThreadNew(Task_Ethernet, NULL, &ethernet_attributes);

  /* creation of Flash */
  FlashHandle = osThreadNew(Task_Flash_data, NULL, &Flash_attributes);

  /* creation of Diod */
  DiodHandle = osThreadNew(Task_Diod, NULL, &Diod_attributes);

  /* creation of RMS */
  RMSHandle = osThreadNew(Task_RMS, NULL, &RMS_attributes);

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
  //  SPI1_CR1 = SPI1 -> CR1;
  //         if (cikl == 16){
  //      Memory_Interleaved_Fast(&ADC_data8[0]);
  //        cikl = 0;
  //   }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 13;
  RCC_OscInitStruct.PLL.PLLN = 195;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_5);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, RESET_Pin|PWDN_Pin|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_ADC_GPIO_Port, SPI1_CS_ADC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ETH_RST_Pin|SPI1_CSf6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_3_Pin|LED_2_Pin|LED_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Start_in_Pin|UART4_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SPI1_CSf8_Pin|SPI1_CSf7_Pin|SPI1_CSf5_Pin|SPI1_CSf4_Pin
                          |SPI1_CSf3_Pin|SPI1_CSf2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CSf1_GPIO_Port, SPI1_CSf1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PE2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : RESET_Pin PWDN_Pin SPI1_CS_ADC_Pin PE0
                           PE1 */
  GPIO_InitStruct.Pin = RESET_Pin|PWDN_Pin|SPI1_CS_ADC_Pin|GPIO_PIN_0
                          |GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : ETH_RST_Pin SPI1_CSf6_Pin */
  GPIO_InitStruct.Pin = ETH_RST_Pin|SPI1_CSf6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_3_Pin LED_2_Pin LED_1_Pin */
  GPIO_InitStruct.Pin = LED_3_Pin|LED_2_Pin|LED_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Start_in_Pin UART4_DIR_Pin */
  GPIO_InitStruct.Pin = Start_in_Pin|UART4_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_CSf8_Pin SPI1_CSf7_Pin SPI1_CSf5_Pin SPI1_CSf4_Pin
                           SPI1_CSf3_Pin SPI1_CSf2_Pin SPI1_CSf1_Pin */
  GPIO_InitStruct.Pin = SPI1_CSf8_Pin|SPI1_CSf7_Pin|SPI1_CSf5_Pin|SPI1_CSf4_Pin
                          |SPI1_CSf3_Pin|SPI1_CSf2_Pin|SPI1_CSf1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// ��� ���(

 void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
  {
      if (hspi == &hspi2)      
      {
        delay(10);
          FLASH_CS_HIGH(CS_num);
          
      }
  }

  void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
  {
      if (hspi == &hspi2)      
      {
        delay(10);
         FLASH_CS_HIGH(CS_num);
      }
  }
  

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if(flag_ADC && hspi == &hspi1){
     // delay(5);
      ADC_CS_HIGH;
      
      ADC_16[0] = (int16_t)(((uint16_t)ADC_rx_data[3]  << 8) | ADC_rx_data[4]);   // CH1
      ADC_16[1] = (int16_t)(((uint16_t)ADC_rx_data[5]  << 8) | ADC_rx_data[6]);   // CH2
      ADC_16[2] = (int16_t)(((uint16_t)ADC_rx_data[7]  << 8) | ADC_rx_data[8]);   // CH3
      ADC_16[3] = (int16_t)(((uint16_t)ADC_rx_data[9]  << 8) | ADC_rx_data[10]);  // CH4
      ADC_16[4] = (int16_t)(((uint16_t)ADC_rx_data[11] << 8) | ADC_rx_data[12]);  // CH5
      ADC_16[5] = (int16_t)(((uint16_t)ADC_rx_data[13] << 8) | ADC_rx_data[14]);  // CH6
      ADC_16[6] = (int16_t)(((uint16_t)ADC_rx_data[15] << 8) | ADC_rx_data[16]);  // CH7
      ADC_16[7] = (int16_t)(((uint16_t)ADC_rx_data[17] << 8) | ADC_rx_data[18]);  // CH8
      
      ModbusRegister[0] = ADC_16[0];
      ModbusRegister[1] = ADC_16[1];
      ModbusRegister[2] = ADC_16[2];
      ModbusRegister[3] = ADC_16[3];
      ModbusRegister[4] = ADC_16[4];
      ModbusRegister[5] = ADC_16[5];
      ModbusRegister[6] = ADC_16[6];
      ModbusRegister[7] = ADC_16[7];
      
      RMS_Sample(&ch[0], ADC_16[0]);
      RMS_Sample(&ch[1], ADC_16[1]);
      RMS_Sample(&ch[2], ADC_16[2]);
      RMS_Sample(&ch[3], ADC_16[3]);
      RMS_Sample(&ch[4], ADC_16[4]);
      RMS_Sample(&ch[5], ADC_16[5]);
      RMS_Sample(&ch[6], ADC_16[6]);
      RMS_Sample(&ch[7], ADC_16[7]);
 
  }
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == UART4)
    {
        // Небольшая задержка
        volatile uint32_t delay = 10;
        while(delay--);
        
        // Переключаемся в режим приема
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); 
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (hspi1.State != HAL_SPI_STATE_READY) return;
 delay(10);
 ADS131E0_DataRead();
}


// ��� ���)
/* USER CODE END 4 */

/* USER CODE BEGIN Header_Task_ADC_Data */
/**
  * @brief  Function implementing the ADC_data thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task_ADC_Data */
void Task_ADC_Data(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    switch (experement){
    case 1:
        ADC_START_ON
        flag_ADC = 1;
        experement = 0;
        break;
    
    case 2:
        ADC_START_OFF
        flag_ADC = 0;
        experement = 0;
        break;
    }
    osDelay(1);
       
  //  default:
      //  vTaskSuspend(NULL);

    
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task_ModBus */
/**
* @brief Function implementing the modbus thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_ModBus */
void Task_ModBus(void *argument)
{
  /* USER CODE BEGIN Task_ModBus */
  /* Infinite loop */
  for(;;)
  {
    uartDataHandler();  // Обработка полученных данных
    osDelay(1);
  }
  /* USER CODE END Task_ModBus */
}

/* USER CODE BEGIN Header_Task_Ethernet */
/**
* @brief Function implementing the ethernet thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Ethernet */
void Task_Ethernet(void *argument)
{
  /* USER CODE BEGIN Task_Ethernet */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Task_Ethernet */
}

/* USER CODE BEGIN Header_Task_Flash_data */
/**
* @brief Function implementing the Flash thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Flash_data */
void Task_Flash_data(void *argument)
{
  /* USER CODE BEGIN Task_Flash_data */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Task_Flash_data */
}

/* USER CODE BEGIN Header_Task_Diod */
/**
* @brief Function implementing the Diod thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_Diod */
void Task_Diod(void *argument)
{
  /* USER CODE BEGIN Task_Diod */
  /* Infinite loop */
  for(;;)
  {
    //HAL_Delay(500);
    //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
    
    osDelay(1);
  }
  /* USER CODE END Task_Diod */
}

/* USER CODE BEGIN Header_Task_RMS */
/**
* @brief Function implementing the RMS thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_RMS */
void Task_RMS(void *argument)
{
  /* USER CODE BEGIN Task_RMS */
  /* Infinite loop */
  for(;;)
  {
  RMS_CalcResult(&ch[0]);
  RMS_CalcResult(&ch[1]);
  RMS_CalcResult(&ch[2]);
  RMS_CalcResult(&ch[3]);
  RMS_CalcResult(&ch[4]);
  RMS_CalcResult(&ch[5]);
  RMS_CalcResult(&ch[6]);
  RMS_CalcResult(&ch[7]);
  
  osDelay(1);
  }
  /* USER CODE END Task_RMS */
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
#ifdef USE_FULL_ASSERT
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
