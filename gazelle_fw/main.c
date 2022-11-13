#include "gazelle.h"
#include "delay.h"
#include "spi_flash.h"

int err = 0, err0 = 0;
int compl = 0;
uint64_t id;

extern uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE];

int main(void) {
	gazelleInit();
	// rough_delay_us(100);
	// spiFlashWaitForBusy();
	// rough_delay_us(100);
	//
	// delay_ms(100);
	// spiFlashDisableWriteProtect();
	// delay_ms(100);
	//
	// spiFlashWriteEnable();
	// spiFlashErase();
	// rough_delay_us(100);
	// spiFlashWaitForBusy();
	// rough_delay_us(100);
	//
	// for(int i=0 ; i<128 ; ++i) {
	// 	flashToUsbBuffer[i] = (uint8_t)(i+0xa0);
	// }
	//
	// err0 =  spiFlashReadStatus();
	//
	//
	//
	// spiFlashWriteEnable();
	// spiFlashWritePage(0x000000,32);
	// rough_delay_us(100);
	// spiFlashWaitForBusy();
	// delay_s(1);
	// err =  spiFlashReadStatus();
	// rough_delay_us(100);
	// id = spiFlashReadId();


	while(1);
}
