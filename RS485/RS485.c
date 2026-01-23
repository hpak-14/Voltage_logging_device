#include "RS485.h"


extern UART_HandleTypeDef huart4;


void Enable_Transmitter(void) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  
}

void Enable_Receiver(void) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);  
}



void RS485_Send(uint8_t *data, uint16_t size){
  
    Enable_Transmitter();
    
    HAL_UART_Transmit(&huart4, data, size, 1000);
    
    Enable_Receiver();
}


