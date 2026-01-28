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
   

#define NUMBER_OF_REGISTER 100
#define NUMBER_OF_COIL 100
#define BUFFERSIZE (NUMBER_OF_REGISTER * 2 + 5)

extern uint16_t ModbusRegister[NUMBER_OF_REGISTER];
extern bool ModbusCoil[NUMBER_OF_COIL];

extern uint8_t SLAVEID;
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
void Init_Modbus(void);


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

// DC
#define MB_DC1 16
#define MB_DC2 17
#define MB_DC3 18
#define MB_DC4 19
#define MB_DC5 20
#define MB_DC6 21
#define MB_DC7 22
#define MB_DC8 23

// Старт АЦП (1-Старт 0-Стоп)
#define MB_ADC_START 24

// Запись в LOG (1-Логировать канал 0-Выключить логирования канала) Через битовую маску
#define MB_LOG_ch 25

// Объём памяти LOGа
#define MB_LOG_SISE 26

// Заданный Адресс устройства
#define MB_add 27

// Описание устройства(ХЗ зачем, серега(Омск) сказал добавить)
#define MB_info_device 28

// ХЗ зачем, серега(Омск) сказал добавить
#define MB_LOG_protocol_info 29

// Ethernet config
#define MB_Ethernet_conf_1  28
#define MB_Ethernet_conf_2  29
#define MB_Ethernet_conf_3  30
#define MB_Ethernet_conf_4  31
#define MB_Ethernet_conf_5  32
#define MB_Ethernet_conf_6  33
#define MB_Ethernet_conf_7  34
#define MB_Ethernet_conf_8  35
#define MB_Ethernet_conf_9  36
#define MB_Ethernet_conf_10 37
#define MB_Ethernet_conf_11 38
#define MB_Ethernet_conf_12 39
#define MB_Ethernet_conf_13 40
#define MB_Ethernet_conf_14 41
#define MB_Ethernet_conf_15 42
#define MB_Ethernet_conf_16 43




#endif /* INC_MODBUSRTU_SLAVE_H_ */


