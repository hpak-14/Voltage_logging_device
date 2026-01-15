#include <flash.h>

uint8_t flag_mem = 0;
uint8_t cmd = 0;
uint8_t CS_num = 0;
uint32_t DMA_TX_Finish = 0;
uint8_t DMA_RX_Finish = 0;
uint32_t addr = 0x000000;
uint8_t rxbuf [2560] = {0};  
uint8_t txbuf [256] = {0};  
uint8_t data_TX [256] = {
    1, 2, 8, 0, 1, 7, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
    91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
    101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
    121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
    131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
    141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
    161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
    171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
    181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
    191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
    201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
    211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
    221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
    251, 252, 253, 254, 255 , 11
};         

uint8_t data_RX [256] = {0};
// Конф. CS                         0           1           2           3           4           5           6           7
GPIO_TypeDef* FLASH_CS_PORT[8] = {  GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOD,      GPIOB,      GPIOD,      GPIOD       };
uint16_t FLASH_CS_PIN[8]   =     {  GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_0  };

// Установить CS низким
void FLASH_CS_LOW(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_RESET);
}
// Установить CS высоким
void FLASH_CS_HIGH(uint8_t num) {
    HAL_GPIO_WritePin(FLASH_CS_PORT[num], FLASH_CS_PIN[num], GPIO_PIN_SET);
}


void Flash_cmd(uint8_t cmd, uint8_t CS)
{
  CS_num = CS;
  delay(10);
  FLASH_CS_LOW(CS);
  delay(10);
  HAL_SPI_Transmit_DMA(&hspi2, &cmd, 1);
 // while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);


}


void Flash_Transmit(uint8_t num_pin, uint32_t addr, uint8_t *data_TX)
{
  uint8_t txbuf [1 + 3 + 256] = {0};
  CS_num = num_pin;
  Flash_cmd(CMD_WRITE_ENABLE, num_pin);
  delay(10);
  Flash_cmd(CMD_WRITE_ENABLE, num_pin );  
  txbuf[0] = CMD_PAGE_PROGRAM;
  txbuf[1] = (addr >> 16) & 0xFF;
  txbuf[2] = (addr >> 8) & 0xFF;
  txbuf[3] = addr & 0xFF;
  memcpy(&txbuf[4], data_TX, 256);
  FLASH_CS_LOW(num_pin);
  delay(10);
  HAL_SPI_Transmit_DMA(&hspi2, txbuf, 260);
  //Flash_WaitBusy(num_pin);
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

#define CHIP_SIZE  (8 * 1024 * 1024)  // 8 МБ на чип
#define PAGE_SIZE 256
#define SECTOR_SIZE 4096
#define CHIPS_COUNT 8

uint8_t current_chip = 0;                      // Текущий активный чип
uint32_t chip_addresses[8] = {0};              // Текущие адреса для каждого чипа
uint32_t sector_counters[8] = {0};             // Счётчики записанных байт в текущем секторе

void Memory_Interleaved_Fast(uint8_t *data_TX) {
  
    static uint8_t fast_cycle = 0;
    static uint32_t last_write_time[8] = {0};
    uint8_t target_chip = current_chip;
    
    //if (chip_addresses[target_chip] >= CHIP_SIZE) {
    //current_chip = (current_chip + 1) % CHIPS_COUNT;
    //return;
    //}
    if (chip_addresses[target_chip] >= CHIP_SIZE) {
    chip_addresses[target_chip] = 0;  // начинаем с начала
    }

    Flash_Transmit(target_chip, chip_addresses[target_chip], &data_TX[0]);
    
    // Обновляем состояние
    chip_addresses[target_chip] += PAGE_SIZE;
    sector_counters[target_chip] += PAGE_SIZE;
    
    // Если сектор заполнен
    if (sector_counters[target_chip] >= SECTOR_SIZE) {
        sector_counters[target_chip] = 0;
        // Стираем следующий сектор в фоне
        uint32_t next_sector = chip_addresses[target_chip] & 0xFFFFF000;
        Flash_SectorErase(target_chip, next_sector);
    }
    
    // Переходим к следующему чипу
    current_chip = (target_chip + 1) % CHIPS_COUNT;
}