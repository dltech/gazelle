#include "gazelle.h"
#include "i2c_flash.h"

int err = 0;
extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];
int compl = 0;

int main(void) {
	gazelleInit();
//	err += i2cFlashWritePage(0x0002,1);
    err += i2cFlashReadPage(0x01ff,30);
	compl = 1;
	while(1);
}
