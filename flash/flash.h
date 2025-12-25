#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_gpio.h"
#include "stm32f2xx_hal_dma.h"

extern SPI_HandleTypeDef hspi2;
extern void delay(uint32_t tik);
void Flash_ChipErase(uint8_t num_pin);
void Memory_test(void);
void Flash_WaitBusy(uint8_t num_pin);
void Flash_cmd(uint8_t cmd, uint8_t CS);
void Flash_Transmit(uint8_t num_pin, uint32_t addr);
void Flash_Receive(uint8_t num_pin, uint32_t addr);
void Flash_SectorErase(uint8_t num_pin, uint32_t addr);

void FLASH_CS_LOW(uint8_t num);
void FLASH_CS_HIGH(uint8_t num);

extern uint8_t CS_num;

extern GPIO_TypeDef* FLASH_CS_PORT[8];
extern uint16_t      FLASH_CS_PIN[8];


/* Команды SST26 */
extern uint8_t cmd;

#define CMD_READ            0x03
#define CMD_FAST_READ       0x0B
#define CMD_PAGE_PROGRAM    0x02
#define CMD_SECTOR_ERASE    0x20 /* 4KB */
#define CMD_CHIP_ERASE      0xC7
#define CMD_WRITE_ENABLE    0x06
#define CMD_WRITE_DISABLE   0x04
#define CMD_READ_STATUS     0x05
#define CMD_READ_ID         0x9F
#define Block_Protection    0x98

#define WRITE_TIMEOUT       1000    // 1 секунда 
#define ERASE_TIMEOUT       5000    // 5 секунд 
#define CHIP_ERASE_TIMEOUT  60000   // 60 секунд 
