/*
 * 95160 EEPROM Manager
 * 2020 Cedric PAILLE
 *
 */

#include "eeprom.h"

#define EEPROM_WR_ENABLE 		0b00000110
#define EEPROM_WR_DISABLE 		0b00000100
#define EEPROM_RD_STATUS_REG 	0b00000101
#define EEPROM_WR_STATUS_REG 	0b00000001
#define EEPROM_RD_DATA 			0b00000011
#define EEPROM_WR_DATA 			0b00000010
#define EEPROM_WR_PAGEID 		0b10000010
#define EEPROM_RD_PAGEID 		0b10000011
#define EEPROM_RD_PAGEID_LOCK 	0b10000011
#define EEPROM_LOCK_PAGEID 		0b10000010

// S Chip select
// D Data input
// Q Data output
// C Clock


EEPROM_95160::EEPROM_95160(int s, int d, int q, int c, int w, int h) :
	s_pin(s, DigitalPin::MODE_OUTPUT),
	d_pin(d, DigitalPin::MODE_OUTPUT),
	q_pin(q, DigitalPin::MODE_INPUT),
	clock_pin(c, DigitalPin::MODE_OUTPUT),
	w_pin(w, DigitalPin::MODE_OUTPUT),
	hold_pin(h, DigitalPin::MODE_OUTPUT)
{
	clock_pin.set_fast(0);
	hold_pin.set_fast(1);
	unselect_chip();
	we_chip(false);
}

void
EEPROM_95160::select_chip()
{
	s_pin.set_fast(0);
}

void
EEPROM_95160::we_chip(bool e)
{
	w_pin.set_fast(!e);
}

void
EEPROM_95160::unselect_chip()
{
	s_pin.set_fast(1);
}
void
EEPROM_95160::send_data8(const uint8_t command){
	for (int i = 0; i < 8; ++i){
		char out = (command >> (7-i)) & 0x1;
		d_pin.set_fast(out);
		clock_pin.set_fast(1);
		clock_pin.set_fast(0);
	}
	d_pin.set_fast(0);
}

uint8_t
EEPROM_95160::read_data8(){
	uint8_t in = 0;
	for (int i = 0; i < 8; ++i){
		clock_pin.set_fast(1);
		uint8_t bit = q_pin.get();
		clock_pin.set_fast(0);
		in |= bit << (7-i);
	}
	return in;
}

void
EEPROM_95160::send_data16(const uint16_t command){
	uint8_t data1 = (command >> 8) & 0xff;
	uint8_t data2 = command & 0xff;
	send_data8(data1);
	send_data8(data2);
}

bool
EEPROM_95160::is_reg_write_protect(){
	select_chip();
	send_data8(EEPROM_RD_STATUS_REG);
	uint8_t status = read_data8();
	unselect_chip();
	return status & 0b10000000;
}

uint8_t
EEPROM_95160::read(uint16_t address){
	select_chip();
	send_data8(EEPROM_RD_DATA);
	send_data16(address);
	uint8_t data = read_data8();
	unselect_chip();
	return data;
}

void
EEPROM_95160::write(uint16_t address, uint8_t data){
	while(is_write_in_progress()){
	}
	select_chip();
	we_chip(true);
	send_data8(EEPROM_WR_ENABLE);
	unselect_chip();
	select_chip();
	send_data8(EEPROM_WR_DATA);
	send_data16(address);
	send_data8(data);
	unselect_chip();
	we_chip(false);
	select_chip();
	send_data8(EEPROM_WR_DISABLE);
	unselect_chip();
}

bool
EEPROM_95160::is_write_in_progress(){
	select_chip();
	send_data8(EEPROM_RD_STATUS_REG);
	uint8_t status = read_data8();
	unselect_chip();
	return status & 0b1;
}

bool
EEPROM_95160::is_write_enable_latch(){
	select_chip();
	send_data8(EEPROM_RD_STATUS_REG);
	uint8_t status = read_data8();
	unselect_chip();
	return status & 0b10;
}

uint8_t
EEPROM_95160::block_protect_status(){
	select_chip();
	send_data8(EEPROM_RD_STATUS_REG);
	uint8_t status = read_data8();
	unselect_chip();
	return (status & 01100) >> 2;
}

void
EEPROM_95160::reg_write(bool bp0, bool bp1, bool srwd){
	uint8_t reg = 0;
	reg |= bp0 ? 0b10 : 0;
	reg |= bp1 ? 0b100 : 0;
	reg |= srwd ? 0b10000000 : 0;
	select_chip();
	send_data8(EEPROM_WR_ENABLE);
	send_data8(EEPROM_WR_STATUS_REG);
	send_data8(reg);
	send_data8(EEPROM_WR_DISABLE);
	unselect_chip();
}
