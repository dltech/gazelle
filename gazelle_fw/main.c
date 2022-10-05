#include "gazelle.h"
#include "i2c_flash.h"
#include "delay.h"

int err = 0;
extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];
int compl = 0;

int main(void) {
	gazelleInit();
	err += i2cFlashWritePage(0x01ff,1);
	delay_ms(100);
	err += i2cFlashReadPage(0x01ff,30);
	compl = 1;
	while(1);
}
