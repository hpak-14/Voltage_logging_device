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
        ReadInputRegister = 0x04,
	WriteSingleCoil = 0x05,
	WriteSingleRegister = 0x06,
	WriteMultipleCoils = 0x0F,
	WriteMultipleResisters = 0x10,
};
   

#define NUMBER_OF_REGISTER 100
#define NUMBER_OF_COIL 100
#define BUFFERSIZE (NUMBER_OF_REGISTER * 2 + 5)
#define HOLDING_START     0x0000
#define HOLDING_COUNT     100
#define INPUT_START       0x0000
#define INPUT_COUNT       100

extern uint16_t ModbusRegister[NUMBER_OF_REGISTER];
extern bool ModbusCoil[NUMBER_OF_COIL];

extern uint8_t SLAVEID;
extern uint8_t uartRxData;
extern uint8_t DataCounter;
extern uint8_t RxInterruptFlag;
extern uint8_t uartTimeCounter;
extern uint8_t uartPacketComplatedFlag;
extern uint8_t ModbusRx[BUFFERSIZE];

/* Receiver functions */
void uartDataHandler(void);
void uartTimer(void);
void transmitDataMake(uint8_t *msg, uint8_t Lenght);


/* Modbus Functions */
void makePacket_01(uint8_t *msg, uint8_t Lenght);
void makePacket_03(uint8_t *msg, uint8_t Lenght);
void makePacket_04(uint8_t *msg, uint8_t Lenght); // Кастом
void makePacket_05(uint8_t *msg, uint8_t Lenght);
void makePacket_06(uint8_t *msg, uint8_t Lenght);
void makePacket_15(uint8_t *msg, uint8_t Lenght);
void makePacket_16(uint8_t *msg, uint8_t Lenght);

void ILLEGAL_FUNCTION(uint8_t *msg);
void ILLEGAL_DATA_ADDRESS(uint8_t *msg);
void ILLEGAL_DATA_VALUE(uint8_t *msg);
bool  Modbus_CheckIllegalDataAddress(uint8_t *msg);
bool Modbus_CheckIllegalDataValue(uint8_t *msg);

void sendMessage(uint8_t *msg, uint8_t len);
uint8_t findByte(int16_t NumberOfCoil);
uint16_t MODBUS_CRC16(uint8_t *buf, uint8_t len );
void Init_Modbus(void);

void modbus_flash(void);
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
#define MB_LOG_SISE_1 26 //Не знаю в каком формате измеряется данная величина (В мегабайтах или в кол-ве доступных измеренний)
#define MB_LOG_SISE_2 27

// Заданный Адресс устройства
#define MB_add 28

// Описание устройства(ХЗ что здесь должно быть)
//#define MB_info_device 29
//#define MB_info_device 30
//#define MB_info_device 31
//#define MB_info_device 32

// ХЗ зачем
//#define MB_LOG_protocol_info 33
//#define MB_LOG_protocol_info 34
//#define MB_LOG_protocol_info 35
//#define MB_LOG_protocol_info 36

// Номер считываемой подзаписи в данный момент
#define MB_LOG_read_number 37

// Количество записей в логе
#define MB_LOG_record 38


// Ethernet config
#define MB_Ethernet_conf_1  39
#define MB_Ethernet_conf_2  40
#define MB_Ethernet_conf_3  41
#define MB_Ethernet_conf_4  42
#define MB_Ethernet_conf_5  43
#define MB_Ethernet_conf_6  44
#define MB_Ethernet_conf_7  45
#define MB_Ethernet_conf_8  46
#define MB_Ethernet_conf_9  47
#define MB_Ethernet_conf_10 48
#define MB_Ethernet_conf_11 49
#define MB_Ethernet_conf_12 50
#define MB_Ethernet_conf_13 51
#define MB_Ethernet_conf_14 52
#define MB_Ethernet_conf_15 53
#define MB_Ethernet_conf_16 54







#endif /* INC_MODBUSRTU_SLAVE_H_ */


