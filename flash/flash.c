#include <flash.h>

Flash_WriteMeta flash_meta = {0};
uint8_t  read_chip = 0;
uint32_t read_addr = 0;
uint32_t read_bytes_left = 0;

uint8_t flag_mem = 0;
uint8_t cmd = 0;
uint8_t CS_num = 0;
uint32_t DMA_TX_Finish = 0;
uint8_t DMA_RX_Finish = 0;
uint32_t addr = 0x000000;
uint8_t rxbuf [256] = {0};  
uint8_t txbuf [256] = {0};  
uint8_t data_TX [256] = {0};         

uint8_t data_RX [256] = {0};
// ����. CS                         0           1           2           3           4           5           6           7
GPIO_TypeDef* FLASH_CS_PORT[8] = {  GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOB,      GPIOD,      GPIOD       };
uint16_t FLASH_CS_PIN[8]   =     {  GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_0  };

// ���������� CS ������
void FLASH_CS_LOW(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_RESET);
}
// ���������� CS �������
void FLASH_CS_HIGH(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_SET);
}


void Flash_cmd(uint8_t cmd, uint8_t CS)
{
  CS_num = CS;
 // delay(10);
  FLASH_CS_LOW(CS);
  //delay(10);
  HAL_SPI_Transmit_DMA(&hspi2, &cmd, 1);
 // while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);


}


void Flash_Transmit(uint8_t num_pin, uint32_t addr, uint8_t *data_TX)
{
  uint8_t txbuf [1 + 3 + 256] = {0};
  CS_num = num_pin;
  Flash_cmd(CMD_WRITE_ENABLE, num_pin);
 // delay(10);
  Flash_cmd(CMD_WRITE_ENABLE, num_pin );  
  txbuf[0] = CMD_PAGE_PROGRAM;
  txbuf[1] = (addr >> 16) & 0xFF;
  txbuf[2] = (addr >> 8) & 0xFF;
  txbuf[3] = addr & 0xFF;
  memcpy(&txbuf[4], data_TX, 256);
  FLASH_CS_LOW(num_pin);
  delay(10);
  HAL_SPI_Transmit_DMA(&hspi2, txbuf, 260);
  while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
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
  delay(10);
  HAL_SPI_Transmit(&hspi2, tx_buffer, 4, 1000);
  HAL_SPI_Receive_DMA(&hspi2, rxbuf, 256);
  while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
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
 
void Flash_ChipErase(uint8_t num_pin){
  Flash_cmd(CMD_WRITE_ENABLE, num_pin);
  delay(10);
  Flash_cmd(CMD_CHIP_ERASE, num_pin);
  //Flash_WaitBusy(num_pin);
}


void Flash_WaitBusy(uint8_t num_pin)
{
    uint8_t cmd = 0x05;
    uint8_t status = 0;
    FLASH_CS_LOW(num_pin);
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);
    do {
        HAL_SPI_Receive(&hspi2, &status, 1, 100);
    } while (status & 0x01);
    FLASH_CS_HIGH(num_pin);
}


uint32_t addr_flash = 0;
uint32_t masa = 0;
uint32_t mem_CS = 0;

void Memory(uint8_t *data_TX){
  addr_flash = 0;
  masa |= (0b11111 << 27);          
  mem_CS = (masa >> 24) & 0b111;    
  addr_flash = masa & 0xFFFFFF;
  Flash_Transmit(mem_CS, addr_flash, &data_TX[0]);
  masa++;
}

/*
void Memory(uint8_t *data_TX){
  addr_flash = 0;
  masa |= (0b11111 << 27);          
  mem_CS = (masa >> 24) & 0b111;    
  addr_flash = masa & 0xFFFFFF;
  Flash_Transmit(mem_CS, addr_flash, &data_TX[0]);
  masa++;
}
*/

void Memory_test(void){
  
   uint8_t tx_buffer[4] = {
    CMD_READ,           
    (0 >> 16) & 0xFF,
    (0 >> 8) & 0xFF, 
    0 & 0xFF
  };
  
  FLASH_CS_LOW(0);
  delay(10);
  HAL_SPI_Transmit(&hspi2, tx_buffer, 4, 1000);
  HAL_SPI_Receive_DMA(&hspi2, &rxbuf[0], 2560);
  while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
}

void flash_Init(void){
  
    for (uint8_t CS = 0; CS < 8; CS++) {

        Flash_cmd(CMD_WRITE_ENABLE, CS);
        delay(10);

        Flash_cmd(CMD_Block_Protection_Unlock, CS);
        delay(10);
        
        Flash_ChipErase(CS);
        delay(10);
    }

}

#define CHIP_SIZE  (8 * 1024 * 1024)  // 8 MB
#define PAGE_SIZE 256
#define SECTOR_SIZE 4096
#define CHIPS_COUNT 8

uint8_t current_chip = 0;                      
uint32_t chip_addresses[8] = {0};              
uint32_t sector_counters[8] = {0};             

void Memory_Interleaved_Fast(uint8_t *data_TX)
{
    uint8_t target_chip = current_chip;

    /* зацикливание адреса */
    if (chip_addresses[target_chip] >= CHIP_SIZE) {
        chip_addresses[target_chip] = 0;
        flash_meta.buffer_wrapped = 1;
    }

    /* запись страницы */
    Flash_Transmit(target_chip,
                   chip_addresses[target_chip],
                   data_TX);

    /* обновление метаданных */
    flash_meta.last_chip = target_chip;
    flash_meta.last_addr = chip_addresses[target_chip];
    flash_meta.total_pages++;

    /* продвижение */
    chip_addresses[target_chip] += PAGE_SIZE;
    sector_counters[target_chip] += PAGE_SIZE;

    /* стирание следующего сектора заранее */
    if (sector_counters[target_chip] >= SECTOR_SIZE) {
        sector_counters[target_chip] = 0;
        Flash_SectorErase(
            target_chip,
            chip_addresses[target_chip] & 0xFFFFF000
        );
    }

    /* следующий чип */
    current_chip = (target_chip + 1) % CHIPS_COUNT;
}



uint8_t current_chip_read = 0;                      
uint32_t chip_addresses_read[8] = {0};              
uint32_t sector_counters_read[8] = {0};   

void Memory_Interleaved_Read(uint8_t *data_RX, uint32_t pages_to_read)
{
    uint8_t target_chip = current_chip_read;

    for(uint32_t i = 0; i < pages_to_read; i++)
    {
        // Зацикливание адреса чипа
        if (chip_addresses_read[target_chip] >= CHIP_SIZE) {
            chip_addresses_read[target_chip] = 0;
        }

        // Чтение страницы
        Flash_Receive(target_chip,
                      chip_addresses_read[target_chip],
                      data_RX);

        // Продвижение адреса и счетчика сектора
        chip_addresses_read[target_chip] += PAGE_SIZE;
        sector_counters_read[target_chip] += PAGE_SIZE;

        // Сброс счетчика сектора, если достигнут конец сектора
        if (sector_counters_read[target_chip] >= SECTOR_SIZE) {
            sector_counters_read[target_chip] = 0;
        }

        // Переход к следующему чипу
        target_chip = (target_chip + 1) % CHIPS_COUNT;
    }

    // Обновляем текущий чип
    current_chip_read = target_chip;

    // Сброс всех адресов и счетчиков после завершения чтения
   // for (uint8_t chip = 0; chip < CHIPS_COUNT; chip++) {
    //    chip_addresses_read[chip] = 0;
   //     sector_counters_read[chip] = 0;
   // }
    //current_chip_read = 0;
    current_chip = target_chip;
}

