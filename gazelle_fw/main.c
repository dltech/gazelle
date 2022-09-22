#include "gazelle.h"
#include "i2c_flash.h"

int err;
extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];

int main(void) {
	gazelleInit();
	err = i2cFlashReadPage(0x0000,30);
	while(1);
}
