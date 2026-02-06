#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_gpio.h"
#include "stm32f2xx_hal_dma.h"

extern SPI_HandleTypeDef hspi2;
extern uint8_t rxbuf [256];//
extern uint8_t txbuf [256];//
extern uint8_t data_TX [256];
extern uint8_t flag_mem;
extern uint32_t rx_flag;
extern void delay(uint32_t tik);
void Flash_ChipErase(uint8_t num_pin);
void Memory(uint8_t *data_TX);
void Memory_test(void);
void Flash_WaitBusy(uint8_t num_pin);
void Flash_cmd(uint8_t cmd, uint8_t CS);
void Flash_Transmit(uint8_t num_pin, uint32_t addr, uint8_t *data_TX);
void Flash_Receive(uint8_t num_pin, uint32_t addr, uint8_t *rxbuf);
void Flash_SectorErase(uint8_t num_pin, uint32_t addr);
void flash_Init(void);
void Memory_Interleaved_Fast(uint8_t *data_TX);
void FLASH_CS_LOW(uint8_t num);
void FLASH_CS_HIGH(uint8_t num);

void Memory_Interleaved_Read(uint8_t *data_RX, uint32_t pages_to_read);

extern uint8_t CS_num;

extern GPIO_TypeDef* FLASH_CS_PORT[8];
extern uint16_t      FLASH_CS_PIN[8];



extern uint8_t cmd;

#define CMD_READ            0x03
#define CMD_FAST_READ       0x0B
#define CMD_PAGE_PROGRAM    0x02
#define CMD_SECTOR_ERASE    0x20 
#define CMD_CHIP_ERASE      0xC7
#define CMD_WRITE_ENABLE    0x06
#define CMD_WRITE_DISABLE   0x04
#define CMD_READ_STATUS     0x05
#define CMD_READ_ID         0x9F
#define CMD_Block_Protection_Unlock    0x98 


typedef struct {
    uint32_t total_pages;          // сколько страниц записано всего
    uint8_t  last_chip;            // последний чип записи
    uint32_t last_addr;             // последний адрес в чипе
    uint8_t  buffer_wrapped;        // 1 = была перезапись
} Flash_WriteMeta;


