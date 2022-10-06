#include "gazelle.h"
#include "i2c_flash.h"
#include "../lib/regs/i2c_regs.h"
#include "delay.h"

int err = 0;
extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];
int compl = 0;

int main(void) {
	gazelleInit();
	for(int i=0 ; i<I2C_BUFFER_SIZE ; ++i) {
		flashToUsbBuffer[i] = (uint8_t)(i+0x20);
	}
	err += i2cFlashWritePage(0x0100,32);
	delay_ms(100);

	for(int i=0 ; i<I2C_BUFFER_SIZE ; ++i) {
		flashToUsbBuffer[i] = 0;
	}
	err += i2cFlashReadPage(0x0100,32);
	compl = 1;
	while(1);
}
