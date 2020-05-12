/*
 * 95160 EEPROM Manager
 * 2020 Cedric PAILLE
 *
 */

#pragma once
#include "Arduino.h"

void read_line();
void parse_line();
int execute();
bool get_error();
void clear();
