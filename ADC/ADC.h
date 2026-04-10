#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_gpio.h"
#include "spi_dma.h"



extern SPI_HandleTypeDef hspi1;
extern uint8_t flag_ADC;
extern uint8_t flag_ADC_Data;

#define ADC_CS_HIGH            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
#define ADC_CS_LOW             HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);

#define ADC_START_ON           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);    // begin conversions
#define ADC_START_OFF          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // conversions are halted

#define ADC_PWDN_OFF           HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);    // Power-Down OFF
#define ADC_PWDN_ON            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);  // Power-Down ON

#define ADC_RESET_OFF          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);    // RESET OFF
#define ADC_RESET_ON           HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);  // RESET ON

void ADS131E0_Init();
void ADS131E0_ReadID();
void ADS131E0_DataRead();
void delay(uint32_t tik);