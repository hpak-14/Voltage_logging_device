#include "spi_dma.h"

uint16_t rega1 = 0;
uint16_t rega2 = 0;
uint16_t rega3 = 0;
uint16_t rega4 = 0;

uint8_t taxa[19] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
uint8_t raxa[19] = {0};

uint8_t SPI_Transfer_Finished = 0;

void spi_DMA_Init(void)
{
    // 1. Тактирование
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // 2. GPIO PB3,4,5 > AF5
    GPIOB->MODER &= ~(GPIO_MODER_MODER3 |
                      GPIO_MODER_MODER4 |
                      GPIO_MODER_MODER5);

    GPIOB->MODER |= (GPIO_MODER_MODER3_1 |
                     GPIO_MODER_MODER4_1 |
                     GPIO_MODER_MODER5_1);

    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3_1 |
                       GPIO_OSPEEDER_OSPEEDR4_1 |
                       GPIO_OSPEEDER_OSPEEDR5_1);

    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3 |
                      GPIO_PUPDR_PUPDR4 |
                      GPIO_PUPDR_PUPDR5);

    GPIOB->AFR[0] &= ~((0xF << (3*4)) |
                       (0xF << (4*4)) |
                       (0xF << (5*4)));

    GPIOB->AFR[0] |= (5 << (3*4)) |
                     (5 << (4*4)) |
                     (5 << (5*4));

    // 3. SPI1
    SPI1->CR1 = 0;
    SPI1->CR1 = SPI_CR1_MSTR |
                SPI_CR1_SSM |
                SPI_CR1_SSI |
                (0 << SPI_CR1_BR_Pos);

    // Включаем TX и RX DMA
    SPI1->CR2 = SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;

    (void)SPI1->SR;
    (void)SPI1->DR;

    // ---------- DMA2 Stream3 > TX ----------
    DMA2_Stream3->CR = 0;
    DMA2->LIFCR = DMA_LIFCR_CTCIF3 |
                  DMA_LIFCR_CHTIF3 |
                  DMA_LIFCR_CTEIF3;

    DMA2_Stream3->CR |= DMA_SxCR_MINC |
                        DMA_SxCR_DIR_0 |
                        DMA_SxCR_TCIE;

    DMA2_Stream3->CR |= (3 << DMA_SxCR_CHSEL_Pos); // CH3

    DMA2_Stream3->PAR = (uint32_t)&SPI1->DR;

    // ---------- DMA2 Stream0 > RX ----------
    DMA2_Stream0->CR = 0;
    DMA2->LIFCR = DMA_LIFCR_CTCIF0 |
                  DMA_LIFCR_CHTIF0 |
                  DMA_LIFCR_CTEIF0;

    DMA2_Stream0->CR |= DMA_SxCR_MINC |
                        DMA_SxCR_TCIE;   // Peripheral > Memory (DIR=0)

    DMA2_Stream0->CR |= (3 << DMA_SxCR_CHSEL_Pos); // CH3

    DMA2_Stream0->PAR = (uint32_t)&SPI1->DR;

    // NVIC
    NVIC_SetPriority(DMA2_Stream3_IRQn, 5);
    NVIC_SetPriority(DMA2_Stream0_IRQn, 5);
    NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    // SPI enable
    SPI1->CR1 |= SPI_CR1_SPE;
}

// ---------- TX ----------
void SPI_DMA_Transmit(uint8_t *data, uint32_t size)
{
    SPI_Transfer_Finished = 0;
    
    cs_low();
      
    DMA2_Stream3->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream3->CR & DMA_SxCR_EN);

    DMA2->LIFCR = DMA_LIFCR_CTCIF3 |
                  DMA_LIFCR_CHTIF3 |
                  DMA_LIFCR_CTEIF3;

    DMA2_Stream3->M0AR = (uint32_t)data;
    DMA2_Stream3->NDTR = size;

    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

// ---------- RX ----------
void SPI_DMA_Receive(uint8_t *data, uint32_t size)
{
    SPI_Transfer_Finished = 0;
    
    cs_low();
      
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream0->CR & DMA_SxCR_EN);

    DMA2->LIFCR = DMA_LIFCR_CTCIF0 |
                  DMA_LIFCR_CHTIF0 |
                  DMA_LIFCR_CTEIF0;

    DMA2_Stream0->M0AR = (uint32_t)data;
    DMA2_Stream0->NDTR = size;

    DMA2_Stream0->CR |= DMA_SxCR_EN;

    // важно: для RX нужен такт > отправляем dummy
    for (uint32_t i = 0; i < size; i++)
    {
        while (!(SPI1->SR & SPI_SR_TXE));
        *((volatile uint8_t*)&SPI1->DR) = 0xFF;
    }
}

// ---------- FULL DUPLEX ----------
void SPI_DMA_TransmitReceive(uint8_t *tx, uint8_t *rx, uint32_t size)
{
    SPI_Transfer_Finished = 0;
    
    cs_low();
      
    // RX сначала!
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream0->CR & DMA_SxCR_EN);

    DMA2->LIFCR = DMA_LIFCR_CTCIF0 |
                  DMA_LIFCR_CHTIF0 |
                  DMA_LIFCR_CTEIF0;

    DMA2_Stream0->M0AR = (uint32_t)rx;
    DMA2_Stream0->NDTR = size;
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    // TX
    DMA2_Stream3->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream3->CR & DMA_SxCR_EN);

    DMA2->LIFCR = DMA_LIFCR_CTCIF3 |
                  DMA_LIFCR_CHTIF3 |
                  DMA_LIFCR_CTEIF3;

    DMA2_Stream3->M0AR = (uint32_t)tx;
    DMA2_Stream3->NDTR = size;
    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

// ---------- IRQ ----------
void DMA2_Stream3_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF3)
    {
        DMA2_Stream3->CR &= ~DMA_SxCR_EN;
        DMA2->LIFCR = DMA_LIFCR_CTCIF3;
        SPI_Transfer_Finished = 1;
    }
}



// ---------- utils ----------
uint8_t finished_transfer(void)
{
    return SPI_Transfer_Finished;
}

void reset_finished(void)
{
    SPI_Transfer_Finished = 0;
}

void cs_low(void)
{
    GPIOE->BSRR = GPIO_BSRR_BR6;
}

void cs_high(void)
{
    GPIOE->BSRR = GPIO_BSRR_BS6;
}