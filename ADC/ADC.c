#include "ADC.h"
#include "math.h"

  uint8_t ADC_CMD[3] = {0};
  uint8_t ADC_tx_data[3];
  uint8_t ADC_rx_data[19];
  uint8_t ADC_reset[19];
  uint8_t Noliki[19] = {0};
  uint8_t Code_ADC[256] = {0};
  

  
void ADS131E0_ReadID(){

  ADC_CMD[0] = 0b00100000; 
  ADC_CMD[1] = 0b00000000; 

  ADC_CS_LOW
  delay(30);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(20);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[1], 1, HAL_MAX_DELAY);
  delay(20);
  HAL_SPI_Receive(&hspi1, &ADC_rx_data[0], 1, HAL_MAX_DELAY);
  delay(30);
  ADC_CS_HIGH
  delay(30);
}


void ADS131E0_RESET(){
 
  ADC_PWDN_OFF
  HAL_Delay(100);
  ADC_CS_HIGH
  ADC_RESET_ON
  HAL_Delay(100);
  ADC_RESET_OFF
  HAL_Delay(1000);
  
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Receive(&hspi1, &ADC_reset[0], 19, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
  delay(25);
  //////////////////////////////////////////////////////////
 ///*
  ADC_CMD[0] = 0b00010001;
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
  delay(25);

  ADC_CMD[0] = 0b01000101;
  ADC_CMD[1] = 0b00000000;
  ADC_CMD[2] = 0b00010001;
  ADC_CS_LOW
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[1], 1, HAL_MAX_DELAY);
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[2], 1, HAL_MAX_DELAY);
  delay(100);
  ADC_CS_HIGH
  delay(25);
  
  ADC_CMD[0] = 0b00011010;
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
  delay(25);
  
  ADC_CMD[0] = 0b01000101;
  ADC_CMD[1] = 0b00000000;
  ADC_CMD[2] = 0b00010000;
  ADC_CS_LOW
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[1], 1, HAL_MAX_DELAY);
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[2], 1, HAL_MAX_DELAY);
  delay(100);
  ADC_CS_HIGH
  delay(25);
  
  ADC_CMD[0] = 0b00010000;
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
  delay(25);
//*/
}


void ADS131E0_DataRead(){
  
  ADC_CS_LOW
  delay(20);
  HAL_SPI_TransmitReceive_DMA(&hspi1, &Noliki[0], &ADC_rx_data[0], 19);
}


void ADS131E0_Conf(){
  ADC_CMD[0] = 0;
  ADC_CMD[1] = 0;
  ADC_CMD[2] = 0;
    
  ADC_CMD[0] = 0b01000001; // WREG + адрес 01h
  ADC_CMD[1] = 0b00000000; // записать 1 регистр
  ADC_CMD[2] = 0b10010001; // новое значение CONFIG1
  
  ADC_CS_LOW
  delay(25);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 3, HAL_MAX_DELAY);
  delay(25);
  ADC_CS_HIGH
    
  ADC_CMD[0] = 0b00010000;
    
  ADC_CS_LOW
  delay(100);
  HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
  delay(100);
  ADC_CS_HIGH
    
  
}

void delay(uint32_t tik){
  for (int k = 0; k < tik; k++);
}



