/*
 * 95160 EEPROM Manager
 * 2020 Cedric PAILLE
 *
 */

#include "Arduino.h"
#include "errno.h"
#include "eeprom.h"

EEPROM_95160 eeprom;

#define LINE_BUF_SIZE 128   //Maximum input string length
#define ARG_BUF_SIZE 64     //Maximum argument string length
#define MAX_NUM_ARGS 4      //Maximum number of arguments

bool g_error_flag = false;
char g_line[LINE_BUF_SIZE];
char g_args[MAX_NUM_ARGS][ARG_BUF_SIZE];
uint32_t g_arg_count;

int StrToHex(char str[])
{
  char * endptr;
  int ret = (int) strtol(str, &endptr, 16);
  errno = 0;
  if (endptr == str){
	  errno = EINVAL;
  }
  return ret;
}

int cmd_help()
{
	Serial.println("** Help **");
	Serial.println("* read [address_begin] [size]");
	Serial.println("* protect [mode]");
	Serial.println("* write [address] [value]");
	Serial.println("* fill [address] [value] [data to fill]");
	Serial.println("* status (memory bank protection)");
	return 0;
}

int cmd_status()
{
	uint8_t bank_status = eeprom.block_protect_status();
	if (bank_status == 0){
		Serial.println("[MODE 0] No memory protection.");
	}
	if (bank_status == 1){
		Serial.println("[MODE 1] Upper quarter memory protect (0x600 to 0x7FF)");
	}
	if (bank_status == 2){
		Serial.println("[MODE 2] Upper half memory protect (0x400 to 0x7FF)");
	}
	if (bank_status == 3){
		Serial.println("[MODE 3] Whole memory protect (0x400 to 0x7FF)");
	}
	return 0;
}

int cmd_protect()
{
	if (g_arg_count != 1){
		Serial.println("ERROR: Invalid argument count.");
		return -1;
	}
	uint8_t arg = atoi(g_args[1]);
	bool bp0 = arg & 0b10;
	bool bp1 = arg & 0b01;
	eeprom.reg_write(bp0, bp1, false);
	return 0;

}

int cmd_read()
{
	char * ascii = "....................";
	if (g_arg_count != 3){
		Serial.println("ERROR: Invalid argument count.");
		return -1;
	}
	uint16_t mem_begin = StrToHex(g_args[1]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 1.");
		return -1;
	}
	uint16_t mem_end = StrToHex(g_args[2]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 2.");
		return -1;
	}

	if (mem_begin > 0x7ff){
		Serial.println("ERROR: Invalid memory address.");
		return -1;
	}

	if (mem_end > 0x7ff){
		Serial.println("ERROR: Invalid size (overflow).");
		return -1;
	}

	int j = 0;
	for(int i = mem_begin; i < mem_end + 1; ++i){
		if (j == 0){
			Serial.print(i, HEX);
			Serial.print(" ; ");
		}
		uint8_t data = eeprom.read(i);
		Serial.print(data, HEX);
		if (data > 32 && data <126){
			ascii[j] = data;
		} else {
			ascii[j] = '.';
		}
		if (j > 19){
			Serial.print("  ");
			Serial.println(ascii);
			j = 0;
			continue;
		}
		Serial.print(" ");
		j++;
	}
	Serial.println();
	return 0;
}

int cmd_read2()
{
	if (g_arg_count != 3){
		Serial.println("ERROR: Invalid argument count.");
		return -1;
	}
	uint16_t mem_begin = StrToHex(g_args[1]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 1.");
		return -1;
	}
	uint16_t mem_end = StrToHex(g_args[2]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 2.");
		return -1;
	}

	if (mem_begin > 0x7ff){
		Serial.println("ERROR: Invalid memory address.");
		return -1;
	}

	if (mem_end > 0x7ff){
		Serial.println("ERROR: Invalid size (overflow).");
		return -1;
	}

	for(int i = mem_begin; i < mem_end + 1; ++i){
		uint8_t data = eeprom.read(i);
		Serial.print(data, HEX);
		Serial.print(" ");
	}
	Serial.println();
	return 0;
}

int cmd_write()
{
	if (g_arg_count != 3){
		Serial.println("ERROR: Invalid argument count.");
		return -1;
	}
	uint16_t mem_address = StrToHex(g_args[1]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 1.");
		return -1;
	}
	uint8_t data = StrToHex(g_args[2]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 2.");
		return -1;
	}

	if (mem_address > 0x7ff){
		Serial.println("ERROR: Invalid memory address.");
		return -1;
	}
	eeprom.write(mem_address, data);
	return 0;
}

int cmd_fill()
{
	if (g_arg_count != 4){
		Serial.println("ERROR: Invalid argument count.");
		return -1;
	}
	uint16_t mem_begin = StrToHex(g_args[1]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 1.");
		return -1;
	}
	uint16_t mem_end = StrToHex(g_args[2]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 2.");
		return -1;
	}
	uint8_t mem_data = StrToHex(g_args[3]);
	if (errno != 0){
		Serial.println("ERROR: Invalid argument 3.");
		return -1;
	}

	if (mem_begin > 0x7ff){
		Serial.println("ERROR: Invalid memory address.");
		return -1;
	}

	if (mem_end > 0x7ff){
		Serial.println("ERROR: Invalid size (overflow).");
		return -1;
	}

	for(int i = mem_begin; i < mem_end + 1; ++i){
		eeprom.write(i, mem_data);
	}
	return 0;
}


const char *commands_str[] = {
    "help",
	"readh",
	"status",
	"protect",
	"fill",
	"write",
	"read"
};

int (*commands_func[])(){
    &cmd_help,
	&cmd_read,
	&cmd_status,
	&cmd_protect,
	&cmd_fill,
	&cmd_write,
	&cmd_read2
};

int num_commands = sizeof(commands_str) / sizeof(char *);

void
read_line()
{
    String line_string;
    while(!Serial.available());

    if(Serial.available()){
        line_string = Serial.readStringUntil('\n');
        if(line_string.length() < LINE_BUF_SIZE){
          line_string.toCharArray(g_line, LINE_BUF_SIZE);
          Serial.println(line_string);
        }
        else{
          Serial.println("ERROR: Input string too long.");
          g_error_flag = true;
        }
    }
}

void
parse_line()
{
    char *argument;
    argument = strtok(g_line, " ");
    g_arg_count = 0;

    while((argument != NULL)){
        if(g_arg_count < MAX_NUM_ARGS){
            if(strlen(argument) < ARG_BUF_SIZE){
                strcpy(g_args[g_arg_count],argument);
                argument = strtok(NULL, " ");
                g_arg_count++;
            }
            else{
                Serial.println("ERROR: Input string too long.");
                g_error_flag = true;
                break;
            }
        }
        else{
            break;
        }
    }
}

int
execute()
{
    for(int i=0; i<num_commands; i++){
        if(strcmp(g_args[0], commands_str[i]) == 0){
            return(*commands_func[i])();
        }
    }

    Serial.println("ERROR: command. Type \"help\" for more.");
    return 0;
}

bool
get_error()
{
	return g_error_flag;
}

void
clear()
{
	memset(g_line, 0, LINE_BUF_SIZE);
	memset(g_args, 0, sizeof(g_args[0][0]) * MAX_NUM_ARGS * ARG_BUF_SIZE);

	g_error_flag = false;
}
