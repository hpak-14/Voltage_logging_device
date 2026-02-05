#include <ModbusRTU_Slave.h>
#include "main.h"
#include <flash.h>
extern void RS485_Send(uint8_t *data, uint16_t size);

uint8_t uartRxData;
uint8_t DataCounter;
uint8_t RxInterruptFlag;
uint8_t uartTimeCounter;
uint8_t uartPacketComplatedFlag;
uint8_t SLAVEID = 0;

    	uint16_t NumberOfReg = 0;
uint8_t ModbusRx[BUFFERSIZE];
uint8_t tempModbusRx[BUFFERSIZE];
uint8_t ModbusTx[BUFFERSIZE];


uint8_t read_page_buffer[256] = {0};
uint32_t pages_to_read = 0;         // сколько страниц осталось
uint8_t read_in_progress = 0;       // флаг чтения

uint16_t rxCRC;

uint16_t ModbusRegister[NUMBER_OF_REGISTER] = {0};
bool ModbusCoil[NUMBER_OF_COIL] = {0};

extern UART_HandleTypeDef huart4;

/*Receive data interrupt*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	RxInterruptFlag = SET;
	ModbusRx[DataCounter++] = uartRxData;
	if(DataCounter >= BUFFERSIZE)
	{
		DataCounter  = 0;
	}

	HAL_UART_Receive_IT(&huart4 , &uartRxData , 1);
	uartTimeCounter = 0;
}


/*calls the corresponding function according to the received function command*/
void transmitDataMake(uint8_t *msg, uint8_t Lenght)
{

        if(Modbus_CheckIllegalDataAddress(msg)) return;
        if(Modbus_CheckIllegalDataValue(msg)) return;
          
	switch(msg[1])
	{
	//case ReadCoil:
		//makePacket_01(msg, Lenght);
		//break;

	case ReadHoldingRegister:
		makePacket_03(msg, Lenght);
		break;
                
        case ReadInputRegister:
		makePacket_04(msg, Lenght);
		break;

	case WriteSingleRegister:
		makePacket_06(msg, Lenght);
		break;

	//case WriteSingleCoil:
		//makePacket_05(msg, Lenght);
		//break;

	//case WriteMultipleCoils:
		//makePacket_15(msg, Lenght);
		//break;

	//case WriteMultipleResisters:
		//makePacket_16(msg, Lenght);
		//break;
                
        default:
                ILLEGAL_FUNCTION(msg);
        
	}
        

}

/*Runs when data retrieval is complete and check CRC*/
void uartDataHandler(void)
{
	uint8_t tempCounter;
	uint16_t CRCValue;

	if(uartPacketComplatedFlag == SET)     //Data receiving is finished
	{
            uartPacketComplatedFlag = RESET;
	    memcpy(tempModbusRx, ModbusRx, DataCounter + 1);
	    tempCounter = DataCounter;
		DataCounter = 0;
		memset(ModbusRx, 0, BUFFERSIZE);
		memset(ModbusTx, 0, BUFFERSIZE);

		/*CRC Check*/
		CRCValue = MODBUS_CRC16(tempModbusRx, tempCounter - 2);
                rxCRC = tempModbusRx[tempCounter - 2] | (tempModbusRx[tempCounter - 1] << 8);

		/*If the calculated CRC value and the received CRC value are equal and the Slave ID is correct, respond to the receiving data.  */
		if(rxCRC == CRCValue && tempModbusRx[0] == SLAVEID)
		{
			transmitDataMake(&tempModbusRx[0], tempCounter);
		}

	}
}

/* This function should be called in systick timer */
void uartTimer(void)
{
	if(RxInterruptFlag == SET)
	{
		if(uartTimeCounter++ > 200)
		{

			RxInterruptFlag = RESET;
			uartTimeCounter = 0;
			uartPacketComplatedFlag = SET;
		}
	}
}

void sendMessage(uint8_t *msg, uint8_t len)
{
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)msg, len); 
}


/*****Modbus Function*****/
//The function are used to respond to receiving modbus data.

/*Send coil data*/
void makePacket_01(uint8_t *msg, uint8_t Lenght)
{
    uint16_t RegAddress, NumberCoils, NumberByte, CRCValue;
    RegAddress = (msg[2] << 8) | (msg[3]);
    NumberCoils = (msg[4] << 8) | (msg[5]);
    
    // Ограничиваем количество запрашиваемых катушек
    if(NumberCoils > NUMBER_OF_COIL) NumberCoils = NUMBER_OF_COIL;
    
    NumberByte = findByte(NumberCoils);
    
    // Используем ModbusTx сразу для хранения временных данных
    ModbusTx[0] = msg[0];
    ModbusTx[1] = msg[1];
    ModbusTx[2] = NumberByte;
    
    // Очищаем байты данных
    for(uint8_t j = 0; j < NumberByte; j++)
    {
        ModbusTx[3 + j] = 0;
    }
    
    // Заполняем биты
    for(uint16_t CoilCount = 0; CoilCount < NumberCoils; CoilCount++)
    {
        uint8_t byteIndex = CoilCount / 8;
        uint8_t bitIndex = CoilCount % 8;
        
        if(byteIndex < NumberByte)
        {
            if(ModbusCoil[RegAddress + CoilCount])
            {
                ModbusTx[3 + byteIndex] |= (1 << bitIndex);
            }
        }
    }
    
    /*Calculating the CRC value of the data to be sent*/
    CRCValue = MODBUS_CRC16(ModbusTx, 3 + NumberByte);
    ModbusTx[3 + NumberByte] = (CRCValue & 0x00FF);
    ModbusTx[4 + NumberByte] = (CRCValue >> 8);
    /**************************************************/
    
    sendMessage(ModbusTx, 5 + NumberByte);
}

/*Send register data*/
void makePacket_03(uint8_t *msg, uint8_t Lenght)
{
	uint8_t i, m = 0;

	uint16_t RegAddress = 0;

	uint16_t CRCValue;

	RegAddress = (msg[2] << 8) | (msg[3]);
	NumberOfReg = (msg[4] << 8) | (msg[5]);
	ModbusTx[0] = msg[0];
	ModbusTx[1] = msg[1];
	ModbusTx[2] = (NumberOfReg * 2);

	for(i = 0; i < NumberOfReg * 2; i += 2)
	{
		ModbusTx[3 + i] = (uint8_t)(ModbusRegister[RegAddress + m] >> 8);
		ModbusTx[4 + i] = (uint8_t)(ModbusRegister[RegAddress + m] & 0x00FF);
		m++;
	}

	//CRC Calculate
	CRCValue = MODBUS_CRC16(ModbusTx, 3 + (NumberOfReg * 2 ));
	ModbusTx[4 + (NumberOfReg * 2 )] = (CRCValue >> 8);
	ModbusTx[3 + (NumberOfReg * 2 )] = (CRCValue & 0x00FF);
	/********************************************************/
	RS485_Send(ModbusTx, 5 + (NumberOfReg * 2 ));
}


void makePacket_04(uint8_t *msg, uint8_t Lenght){ // Кастом 
    
	uint16_t RegAddress = 0;
        
        RegAddress = (msg[2] << 8) | (msg[3]);
	NumberOfReg = (msg[4] << 8) | (msg[5]);
    
     if (RegAddress == MB_M1 && pages_to_read < 16 ) {
       
       if (pages_to_read == 0){
            Memory_Interleaved_Read(read_page_buffer, 1);
            HAL_Delay(10);
       }
        ModbusRegister[MB_M1] = (read_page_buffer[pages_to_read * 16] << 8) | read_page_buffer[pages_to_read * 16 + 1];
        pages_to_read++;   
        if (pages_to_read >= 16) pages_to_read = 0;
     }
     
     makePacket_03(msg, Lenght);
}
/*Write single coil*/
void makePacket_05(uint8_t *msg, uint8_t Lenght)
{
	uint16_t RegAddress, RegValue;
	RegAddress = (msg[2] << 8) | (msg[3]);
	RegValue = (msg[4] << 8) | (msg[5]);

	ModbusCoil[RegAddress] = (bool)RegValue;

	sendMessage(msg, Lenght);
}

/*Write single register*/
void makePacket_06(uint8_t *msg, uint8_t Lenght)
{
	uint16_t RegAddress, RegValue;
	RegAddress = (msg[2] << 8) | (msg[3]);
	RegValue = (msg[4] << 8) | (msg[5]);

	ModbusRegister[RegAddress] = RegValue;
	sendMessage(msg, Lenght);

}

/*Write multiple coils*/
void makePacket_15(uint8_t *msg, uint8_t Lenght)
{
	uint16_t NumberOfCoils, CRCValue;
	uint8_t i,m,k;
	//uint16_t NumberOfByte, RegAddress; -> not used
	//RegAddress = (msg[2] << 8) | (msg[3]);

	NumberOfCoils = (msg[4] << 8) | (msg[5]);

	//NumberOfByte = msg[6];

	for(i = 0; i < NumberOfCoils; i++)
	{
		ModbusCoil[i] = (msg[7 + m] & (0x01 << k)) ;

		k++;
		if(k % 8 == 0 && k != 0)
		{
			m++;
			k = 0;
		}
	}

	memcpy(ModbusTx, msg, 6);

	CRCValue = MODBUS_CRC16(ModbusTx, 6);
	ModbusTx[6] = (CRCValue & 0x00FF);
	ModbusTx[7] = (CRCValue >> 8);
	sendMessage(ModbusTx, 8);
}


/*Write multiple registers*/
void makePacket_16(uint8_t *msg, uint8_t Lenght)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	uint16_t RegAddress, NumberOfReg, CRCValue;
	uint8_t i,m = 0;
	RegAddress = (msg[2] << 8) | (msg[3]);
	NumberOfReg = (msg[4] << 8) | (msg[5]);

	for(i = 0; i < NumberOfReg; i++)
	{
		ModbusRegister[RegAddress + i] = (uint16_t)((uint16_t)msg[7 + m] << 8) | (msg[8 + m]);
		m += 2;
	}

    memcpy(ModbusTx, msg, 6);

	CRCValue = MODBUS_CRC16(ModbusTx, 6);
	ModbusTx[6] = (CRCValue & 0x00FF);
	ModbusTx[7] = (CRCValue >> 8);
	sendMessage(ModbusTx, 8);
}

uint8_t findByte(int16_t NumberOfCoil)
{
	volatile uint8_t NumberOfByte = 0;

	while(NumberOfCoil >= 0)
	{
		NumberOfCoil -= 8;

		NumberOfByte++;
		if(NumberOfCoil < 0)
		{
			break;
		}
	}
	return NumberOfByte;
}


/*
uint16_t MODBUS_CRC16(uint8_t *buf, uint8_t len )// Нормальный CRC16.
{
    static const uint16_t table[2] = { 0x0000, 0xA001 };
    uint16_t crc = 0xFFFF;

    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= buf[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            crc = (crc >> 1) ^ table[crc & 0x01];
        }
    }

    crc = (crc << 8) | (crc >> 8);

    return crc;   
}

*/
uint16_t MODBUS_CRC16(uint8_t *buf, uint8_t len )// Перевернутый CRC16.
{
    static const uint16_t table[2] = { 0x0000, 0xA001 };
    uint16_t crc = 0xFFFF;

    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= buf[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            crc = (crc >> 1) ^ table[crc & 0x01];
        }
    }

    return crc;   
}


void Init_Modbus(void){

    DataCounter = 0;
    RxInterruptFlag = RESET;
    uartTimeCounter = 0;
    uartPacketComplatedFlag = RESET;
    HAL_UART_Receive_IT(&huart4, &uartRxData, 1);  // Включаем прерывание по приему

    if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11) == GPIO_PIN_RESET) SLAVEID |= (1 << 0); // тумблер 1
    if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == GPIO_PIN_RESET) SLAVEID |= (1 << 1); // тумблер 2
    if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)  == GPIO_PIN_RESET) SLAVEID |= (1 << 2); // тумблер 3
    if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8)  == GPIO_PIN_RESET) SLAVEID |= (1 << 3); // тумблер 4
    ModbusRegister[MB_add] = SLAVEID;

}

bool Modbus_CheckIllegalDataAddress(uint8_t *msg)
{
    uint16_t addr;
    uint16_t qty;

    switch (msg[1])   // Function code
    {
        // Read Holding Registers
        case 0x03:
            addr = (msg[2] << 8) | msg[3];
            qty  = (msg[4] << 8) | msg[5];

            if (qty == 0 ||
                addr < HOLDING_START ||
                (addr + qty - 1) >= (HOLDING_START + HOLDING_COUNT))
            {
                ILLEGAL_DATA_ADDRESS(msg);
                return true;   // сразу выходим
            }
            break;

        // Read Input Registers
        case 0x04:
            addr = (msg[2] << 8) | msg[3];
            qty  = (msg[4] << 8) | msg[5];

            if (qty == 0 ||
                addr < INPUT_START ||
                (addr + qty - 1) >= (INPUT_START + INPUT_COUNT))
            {
                ILLEGAL_DATA_ADDRESS(msg);
                return true;
            }
            break;

        // Write Single Register
        case 0x06:
            addr = (msg[2] << 8) | msg[3];

            if (addr < HOLDING_START ||
                addr >= (HOLDING_START + HOLDING_COUNT))
            {
                ILLEGAL_DATA_ADDRESS(msg);
                return true;
            }
            break;
    }
    return false; 
}

bool Modbus_CheckIllegalDataValue(uint8_t *msg)
{
    uint16_t addr, value;

    switch(msg[1])
    {
        case 0x06: // Write Single Register
            addr  = (msg[2] << 8) | msg[3];
            value = (msg[4] << 8) | msg[5];

            // Проверка регистра MB_ADC_START
            if(addr == MB_ADC_START)
            {
                if(value != 0 && value != 1)  // допустимые значения только 0 и 1
                {
                    ILLEGAL_DATA_VALUE(msg);
                    return true; // ошибка
                }
            }
            break;
    }
    return false; 
}

  void ILLEGAL_FUNCTION(uint8_t *msg){

    uint16_t crc;

    ModbusTx[0] = msg[0];              // Slave Address
    ModbusTx[1] = msg[1] | 0x80;       // Function + 0x80
    ModbusTx[2] = 0x01;                // Illegal Function

    crc = MODBUS_CRC16(ModbusTx, 3);
    ModbusTx[3] = crc & 0xFF;
    ModbusTx[4] = crc >> 8;

    RS485_Send(ModbusTx, 5);
}

void ILLEGAL_DATA_ADDRESS(uint8_t *msg)
{
    uint16_t crc;

    ModbusTx[0] = msg[0];              // Slave Address
    ModbusTx[1] = msg[1] | 0x80;       // Function + 0x80
    ModbusTx[2] = 0x02;                // Illegal Data Address

    crc = MODBUS_CRC16(ModbusTx, 3);
    ModbusTx[3] = crc & 0xFF;          // CRC Low
    ModbusTx[4] = crc >> 8;            // CRC High

    RS485_Send(ModbusTx, 5);
}

void ILLEGAL_DATA_VALUE(uint8_t *msg)
{
    uint16_t crc;

    ModbusTx[0] = msg[0];            // Slave ID
    ModbusTx[1] = msg[1] | 0x80;     // Function + 0x80
    ModbusTx[2] = 0x03;              // Illegal Data Value

    crc = MODBUS_CRC16(ModbusTx, 3);
    ModbusTx[3] = crc & 0xFF;
    ModbusTx[4] = crc >> 8;

    RS485_Send(ModbusTx, 5);
}

