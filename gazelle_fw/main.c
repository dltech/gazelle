#include "gazelle.h"
#include "spi_flash.h"
#include "delay.h"

int err = 0, err0 = 0;
int compl = 0;

extern uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE];

int main(void) {
	gazelleInit();
	// rough_delay_us(100);
	// spiFlashWaitForBusy();
	// rough_delay_us(100);
	//
	// delay_ms(100);
	//spiFlashDisableWriteProtect();
	// delay_ms(100);
	//
	// spiFlashWriteEnable();
	// spiFlashErase();
	// rough_delay_us(100);
	//


	// spiFlashErasePage(0);
	// rough_delay_us(1);
	// spiFlashWaitForBusy();
	// for(int i=0 ; i<256 ; ++i) {
	//  	flashToUsbBuffer[i] = (uint8_t)(i);
	// }
	// flashToUsbBuffer[0] = 0x22;
	// spiFlashWritePage(0x000000,256);
	// rough_delay_us(1);
	// spiFlashWaitForBusy();
	// spiFlashWritePage(0x000100,256);
	// rough_delay_us(1);
	// spiFlashWaitForBusy();

	//
	// err0 =  spiFlashReadStatus();
	//
	//
	//
	// spiFlashWriteEnable();
	rough_delay_us(100);
	//
	// delay_s(1);
	err =  spiFlashReadStatus();
	// rough_delay_us(100);
	// id = spiFlashReadId();


	while(1);
}
