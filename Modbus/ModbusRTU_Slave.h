/*
 * Modbus_Lib.h
 *
 *  Created on: Feb 10, 2022
 *      Author: Ümit Can Güveren
 */


#ifndef INC_MODBUSRTU_SLAVE_H_
#define INC_MODBUSRTU_SLAVE_H_

#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"

enum ModbusFunctions
{
	ReadCoil = 0x01,
	ReadHoldingRegister = 0x03,
	WriteSingleCoil = 0x05,
	WriteSingleRegister = 0x06,
	WriteMultipleCoils = 0x0F,
	WriteMultipleResisters = 0x10,
};
   
#define SLAVEID    2
#define NUMBER_OF_REGISTER 100
#define NUMBER_OF_COIL 100
#define BUFFERSIZE (NUMBER_OF_REGISTER * 2 + 5)

extern uint16_t ModbusRegister[NUMBER_OF_REGISTER];
extern bool ModbusCoil[NUMBER_OF_COIL];


extern uint8_t uartRxData;
extern uint8_t DataCounter;
extern uint8_t RxInterruptFlag;
extern uint8_t uartTimeCounter;
extern uint8_t uartPacketComplatedFlag;
extern char ModbusRx[BUFFERSIZE];

/* Receiver functions */
void uartDataHandler(void);
void uartTimer(void);
void transmitDataMake(char *msg, uint8_t Lenght);


/* Modbus Functions */
void makePacket_01(char *msg, uint8_t Lenght);
void makePacket_03(char *msg, uint8_t Lenght);
void makePacket_05(char *msg, uint8_t Lenght);
void makePacket_06(char *msg, uint8_t Lenght);
void makePacket_15(char *msg, uint8_t Lenght);
void makePacket_16(char *msg, uint8_t Lenght);

void ILLEGAL_FUNCTION(char *msg, uint8_t Lenght);

void sendMessage(char *msg, uint8_t len);
uint8_t findByte(int16_t NumberOfCoil);
uint16_t MODBUS_CRC16(char *buf, uint8_t len );



//*** Карта регистров Modbus ***//

// Мгновенные значения
#define MB_M1 0
#define MB_M2 1
#define MB_M3 2
#define MB_M4 3
#define MB_M5 4
#define MB_M6 5
#define MB_M7 6
#define MB_M8 7

// RMS
#define MB_RMS1 8
#define MB_RMS2 9
#define MB_RMS3 10
#define MB_RMS4 11
#define MB_RMS5 12
#define MB_RMS6 13
#define MB_RMS7 14
#define MB_RMS8 15

// Старт АЦП (1-Старт 0-Стоп)
#define MB_ADC_START 16

// Запись в LOG (1-Логировать канал 0-Выключить логирования канала) Через битовую маску
#define MB_LOG_ch 17

// Объём памяти LOGа
#define MB_LOG_SISE 18

// Заданный Адресс устройства
#define MB_add 19

// Описание устройства(ХЗ зачем, серега(Омск) сказал добавить)
#define MB_info_device 20

// ХЗ зачем, серега(Омск) сказал добавить
#define MB_LOG_protocol_info 21

// Ethernet config
#define MB_Ethernet_conf_1  22
#define MB_Ethernet_conf_2  23
#define MB_Ethernet_conf_3  24
#define MB_Ethernet_conf_4  25
#define MB_Ethernet_conf_5  26
#define MB_Ethernet_conf_6  27
#define MB_Ethernet_conf_7  28
#define MB_Ethernet_conf_8  29
#define MB_Ethernet_conf_9  30
#define MB_Ethernet_conf_10 31
#define MB_Ethernet_conf_11 32
#define MB_Ethernet_conf_12 33
#define MB_Ethernet_conf_13 34
#define MB_Ethernet_conf_14 35
#define MB_Ethernet_conf_15 36
#define MB_Ethernet_conf_16 37




#endif /* INC_MODBUSRTU_SLAVE_H_ */


