#include "ADC.h"
#include "math.h"

  uint8_t ADC_CMD[3] = {0};
  uint8_t ADC_tx_data[3];
  uint8_t ADC_rx_data[19];
  uint8_t ADC_reset[19];
  uint8_t Noliki[19] = {0};
  uint8_t Code_ADC[256] = {0};
  uint8_t flag_ADC = 0xff;
  uint8_t flag_ADC_Data = 0;
  

  
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


void ADS131E0_Init(){
 
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
  // 1. Остановить непрерывное чтение
ADC_CMD[0] = 0b00010001; // SDATAC
ADC_CS_LOW
delay(25);
HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
delay(25);
ADC_CS_HIGH
delay(25);

// 2. Настроить все каналы на замкнутый вход для калибровки
uint8_t channel_addrs[] = {0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C}; // CH1SET-CH8SET

for(int i = 0; i < 8; i++) {
    ADC_CMD[0] = 0b01000000 | channel_addrs[i]; // WREG + адрес регистра
    ADC_CMD[1] = 0b00000000; // 1 регистр
    ADC_CMD[2] = 0b00010001; // PD=0, GAIN=1, MUX=001 (замкнутый вход)
    
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
}

// 3. Выполнить калибровку смещения для всех каналов
ADC_CMD[0] = 0b00011010; // OFFSETCAL
ADC_CS_LOW
delay(25);
HAL_SPI_Transmit(&hspi1, &ADC_CMD[0], 1, HAL_MAX_DELAY);
delay(25);
ADC_CS_HIGH
delay(25);

// 4. Вернуть все каналы к нормальному входу
for(int i = 0; i < 8; i++) {
    ADC_CMD[0] = 0b01000000 | channel_addrs[i]; // WREG + адрес регистра
    ADC_CMD[1] = 0b00000000; // 1 регистр
    ADC_CMD[2] = 0b00010000; // PD=0, GAIN=1, MUX=000 (нормальный вход)
    
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
}

// 5. Включить непрерывное чтение
ADC_CMD[0] = 0b00010000; // RDATAC
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
  delay(4);
  HAL_SPI_TransmitReceive_DMA(&hspi1, &Noliki[0], &ADC_rx_data[0], 19);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
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



