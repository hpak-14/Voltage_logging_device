#include <flash.h>

uint8_t ADC_Code [256] = {0};
static uint32_t addr = 0;
uint32_t masa = 0;
uint8_t mem_CS = 0;
uint8_t cmd = 0;
uint8_t CS_num = 0;
uint8_t DMA_TX_Finish = 0;
uint8_t DMA_RX_Finish = 0;
uint32_t addr_flash = 0x000000;     // адресс
uint8_t rxbuf_test [2560] = {0};      // 256 байт данных
uint16_t rxbuf_test_ADC [1280] = {0}; 

  uint32_t addr_test = 0;

uint8_t data_RX [256] = {0};
//  онф. CS                         0           1           2           3           4           5           6           7
GPIO_TypeDef* FLASH_CS_PORT[8] = {  GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOB,      GPIOD,      GPIOD       };
uint16_t FLASH_CS_PIN[8]   =     {  GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_0  };

// ”становить CS низким
void FLASH_CS_LOW(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_RESET);
}
// ”становить CS высоким
void FLASH_CS_HIGH(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_SET);
}


void Flash_cmd(uint8_t cmd, uint8_t CS)
{
  CS_num = CS;
  delay(10);
  FLASH_CS_LOW(CS);
  delay(5);
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);
  delay(5);
  FLASH_CS_HIGH(CS);
}
/*
        Flash_Transmit( ? ???? ?????? (0-7) , ?????? ?????? (0x000000) );
        Flash_Receive (  ? ???? ?????? (0-7) , ?????? ?????? (0x000000) );
        rxbuf [256] 
        data_TX [256]
*/

void Flash_Transmit(uint8_t num_pin, uint32_t addr, uint8_t *data_TX)
{
  uint8_t txbuf [1 + 3 + 256] = {0};  // 1 байт комада + 3 байта адреса + 256 байт данных
  
  CS_num = num_pin;
  Flash_cmd(0x06, num_pin);
  delay(10);
  Flash_cmd(0x98, num_pin);
  delay(10);
  Flash_SectorErase(num_pin, addr);
  delay(10);
  Flash_WaitBusy();
  delay(20);

  Flash_cmd(CMD_WRITE_ENABLE, num_pin );  
  txbuf[0] = CMD_PAGE_PROGRAM;
  txbuf[1] = (addr >> 16) & 0xFF;
  txbuf[2] = (addr >> 8) & 0xFF;
  txbuf[3] = addr & 0xFF;
  
  memcpy(&txbuf[4], data_TX, 256);
  FLASH_CS_HIGH(CS_num);
  delay(20);
  
  FLASH_CS_LOW(num_pin);
  delay(10);
  HAL_SPI_Transmit_DMA(&hspi2, txbuf, 260);
}


void Flash_Receive(uint8_t num_pin, uint32_t addr, uint8_t *rxbuf){
  
  CS_num = num_pin;
  uint8_t tx_buffer[4] = {
    CMD_READ,           
    (addr >> 16) & 0xFF,
    (addr >> 8) & 0xFF, 
    addr & 0xFF
  };
  
  FLASH_CS_LOW(num_pin);
  HAL_Delay(10);
  HAL_SPI_Transmit(&hspi2, tx_buffer, 4, 100);
  HAL_SPI_Receive_DMA(&hspi2, rxbuf, 256);
}


void Flash_SectorErase(uint8_t num_pin, uint32_t addr)
{
  Flash_cmd(CMD_WRITE_ENABLE, num_pin);
  delay(10);
  uint8_t tx_buffer[4] = {
    CMD_SECTOR_ERASE,
    (addr >> 16) & 0xFF,
    (addr >> 8) & 0xFF,
    addr & 0xFF
  };
  
  FLASH_CS_LOW(num_pin);
  delay(10);
  HAL_SPI_Transmit_DMA(&hspi2, tx_buffer, 4);
}


void Flash_WaitBusy(void)
{
    uint8_t cmd = 0x05;
    uint8_t status;
    FLASH_CS_LOW(0);   
    delay(5);

    do {     
        HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
        FLASH_CS_LOW(0);
        HAL_SPI_Receive(&hspi2, &status, 1, HAL_MAX_DELAY);
    } while (status & 0x01);
    delay(5);
    FLASH_CS_HIGH(0);
}

//          лишние биты    CS         адресс
// masa = 11111       111   8бит + 8бит + 8бит = 32бит
void Memory(uint8_t *data_TX){
  addr_flash = 0;
  masa |= (0b11111 << 27);          // 5 бит лишние выставл€ем в 1
  mem_CS = (masa >> 24) & 0b111;    // бит 24,25,26 это CS
  addr_flash = masa & 0xFFFFFF;
  Flash_Transmit(mem_CS, addr_flash, &data_TX[0]);
  masa++;
}

void Memory_test(void){
  if (addr_test < 0x7FFFFF){
    Flash_Receive(0, addr_test, &rxbuf_test[addr_test * 16]);
    addr_test ++;
  }
  
}