/*
 * Modbus_Lib.c
 *
 *  Created on: Feb 10, 2022
 *      Author: Ümit Can Güveren
 */
#include <ModbusRTU_Slave.h>
#include "main.h"
extern void RS485_Send(uint8_t *data, uint16_t size);

uint8_t uartRxData;
uint8_t DataCounter;
uint8_t RxInterruptFlag;
uint8_t uartTimeCounter;
uint8_t uartPacketComplatedFlag;

    	uint16_t NumberOfReg = 0;
char ModbusRx[BUFFERSIZE];
char tempModbusRx[BUFFERSIZE];
char ModbusTx[BUFFERSIZE];

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
void transmitDataMake(char *msg, uint8_t Lenght)
{
	switch(msg[1])
	{
	case ReadCoil:
		makePacket_01(msg, Lenght);
		break;

	case ReadHoldingRegister:
		makePacket_03(msg, Lenght);
		break;

	case WriteSingleRegister:
		makePacket_06(msg, Lenght);
		break;

	case WriteSingleCoil:
		makePacket_05(msg, Lenght);
		break;

	case WriteMultipleCoils:
		makePacket_15(msg, Lenght);
		break;

	case WriteMultipleResisters:
		makePacket_16(msg, Lenght);
		break;
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

void sendMessage(char *msg, uint8_t len)
{
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)msg, len); 
}


/*****Modbus Function*****/
//The function are used to respond to receiving modbus data.

/*Send coil data*/
void makePacket_01(char *msg, uint8_t Lenght)
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
void makePacket_03(char *msg, uint8_t Lenght)
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

/*Write single coil*/
void makePacket_05(char *msg, uint8_t Lenght)
{
	uint16_t RegAddress, RegValue;
	RegAddress = (msg[2] << 8) | (msg[3]);
	RegValue = (msg[4] << 8) | (msg[5]);

	ModbusCoil[RegAddress] = (bool)RegValue;

	sendMessage(msg, Lenght);
}

/*Write single register*/
void makePacket_06(char *msg, uint8_t Lenght)
{
	uint16_t RegAddress, RegValue;
	RegAddress = (msg[2] << 8) | (msg[3]);
	RegValue = (msg[4] << 8) | (msg[5]);

	ModbusRegister[RegAddress] = RegValue;
	sendMessage(msg, Lenght);

}

/*Write multiple coils*/
void makePacket_15(char *msg, uint8_t Lenght)
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
void makePacket_16(char *msg, uint8_t Lenght)
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
uint16_t MODBUS_CRC16(char *buf, uint8_t len )// Нормальный CRC16
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
uint16_t MODBUS_CRC16(char *buf, uint8_t len )// Перевернутый CRC16
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


