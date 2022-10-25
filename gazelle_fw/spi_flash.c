/*
 * Part of USB universal flaser.
 * Here is SPI flash access functions.
 *
 * Copyright 2021 Mikhail Belkin <dltech174@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../lib/regs/rcc_regs.h"
#include "../lib/regs/spi_regs.h"
#include "usb_core.h"
#include "delay.h"
//#include "flash.h"
#include "spi_flash.h"

extern uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE];

uint8_t spiReadWrite(uint8_t data);
int spiFinalize(void);

void spiFlashInit()
{
    // module clocking
    RCC_APB2ENR |= SPI1EN | IOPAEN;
    // port config
    SPI_PORT = CNF_AF_PUSH_PULL(SCK_PIN)  | MODE_OUTPUT50(SCK_PIN) | \
               CNF_AF_PUSH_PULL(MOSI_PIN) | MODE_OUTPUT50(MOSI_PIN) | \
               CNF_FLOATING(MISO_PIN)     | MODE_INPUT(MISO_PIN) | \
               CNF_PUSH_PULL(NSS_PIN)     | MODE_OUTPUT50(NSS_PIN);
    // 1 mHz master manually controlled SPI config
    SPI1_CR1  = BR_DIV64 | MSTR;
    SPI1_CR2  = 0;
    SPI1_CR1 |= SPE;
    NSS_SET_PORT |= NSS_PIN;
}

uint8_t spiReadWrite(uint8_t data)
{
    NSS_RESET_PORT |= NSS_PIN;
    // writing
    int tOut = TIMEOUT;
    while( ((SPI1_SR & TXE) == 0) && (--tOut>0) );
    SPI1_DR = data;
    // receiving
    tOut = TIMEOUT;
    while( ((SPI1_SR & RXNE) == 0) && (--tOut>0) );
    return (uint8_t)SPI1_DR;
}

int spiFinalize()
{
    int tOut = TIMEOUT;
    while( ((SPI1_SR & BSY) == 0) && (--tOut>0) );
    NSS_SET_PORT |= NSS_PIN;
    if(tOut <= 0) {
        return -1;
    }
    return 0;
}

int spiFlashReadPage(uint32_t address, int size)
{
    spiReadWrite(READ_DATA);
    spiReadWrite((uint8_t)(address >> 16));
    spiReadWrite((uint8_t)(address >> 8));
    spiReadWrite((uint8_t)address);
    for(int i=0 ; i<size ; ++i) {
        flashToUsbBuffer[i] = spiReadWrite(0);
    }
    return spiFinalize();
}

int spiFlashReadAll()
{
    spiReadWrite(READ_DATA);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);

    for(int i=0 ; i<(W25Q64_SIZE+1) ; ++i)
    {
        if((i > 0) && ((i%VCP_MAX_SIZE) == 0)) {
            vcpTx(flashToUsbBuffer, VCP_MAX_SIZE);
        }
        flashToUsbBuffer[i%64] = spiReadWrite(0x00);
    }
    return spiFinalize();
}

int spiFlashWritePage(uint32_t address, int size)
{
    spiReadWrite(PAGE_PROGRAM);
    spiReadWrite((uint8_t)(address >> 16));
    spiReadWrite((uint8_t)(address >> 8));
    spiReadWrite((uint8_t)address);
    for(int i=0 ; i<size ; ++i) {
        spiReadWrite(flashToUsbBuffer[i]);
    }
    return spiFinalize();
}

int spiFlashDisableWriteProtect()
{
    spiReadWrite(WRITE_ENABLE);
    spiFinalize();
    delay_ms(1);

    spiReadWrite(WRITE_STATUS_REGISTER);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    return spiFinalize();
}

int spiFlashWaitForBusy()
{
    uint8_t statusReg = 0;
    int tOut = TIMEOUT;

    spiReadWrite(READ_STATUS_REGISTER1);
    while(((statusReg & W25Q64_BUSY) != 0) && (--tOut>0))
    {
        statusReg = spiReadWrite(0x00);
    }
    spiFinalize();

    if( tOut <= 0 ) {
        return -1;
    }
    // switched on write protection features I'm consider like an error
    if( ((statusReg & (W25Q64_BP0 | W25Q64_BP1 | W25Q64_BP2)) != 0) || \
        ((statusReg & W25Q64_WEL) == 0) ) {
        return -2;
    }
    return 0;
}

int spiFlashErase()
{
    spiReadWrite(CHIP_ERASE);
    spiFinalize();
    rough_delay_us(1);
    return spiFlashWaitForBusy();
}

//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
