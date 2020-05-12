#pragma once
#include "io.h"

// S Chip select
// D Data input
// Q Data output
// C Clock

class EEPROM_95160
{
	//int select_pin, d_pin, q_pin, clock_pin, w_pin, hold_pin;
	DigitalPin s_pin, d_pin, q_pin, clock_pin, w_pin, hold_pin;
	void send_data8(const uint8_t command);
	uint8_t read_data8();
	void send_data16(const uint16_t command);
	bool is_reg_write_protect();
	void select_chip();
	void unselect_chip();
	void we_chip(bool e);
	bool is_write_in_progress();
public:
	EEPROM_95160(int s = 9, int d = 10, int q = 11, int c = 12, int w = 13, int h= 8);
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);
	bool is_write_enable_latch();
	uint8_t block_protect_status();
	void reg_write(bool bp0, bool bp1, bool srwd);
};
