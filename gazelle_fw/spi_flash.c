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
#include "../lib/regs/dma_regs.h"
#include "usb_core.h"
#include "delay.h"
//#include "flash.h"
#include "spi_flash.h"

extern uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE];
uint8_t null = 0;

int spiStart(uint8_t command, uint8_t data1);
uint8_t spiReadWrite(uint8_t data);
int waitForDmaTransfer(void);
void spiDmaRx(int size);
int spiFinalize(void);
int spiWriteByte(uint8_t data);
void qpiGpioInit(void);
void qpiWrite(uint8_t data);


volatile int tout[1000] = {[0 ... 999 ] = 55}, toutcnt = 0;

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
    SPI1_CR1  = BR_DIV128 | MSTR;
    SPI1_CR2  = 0;
    // resolving of not working nss pin problem
    SPI1_CR1 |= SSM | SSI;
    // switching on the interface
    SPI1_CR1 |= SPE;
    NSS_SET_PORT |= NSS_GPIO;
    // configuration of the addictive pins
    SPI_PORT |= CNF_PUSH_PULL(IO2_PIN)     | MODE_OUTPUT50(IO2_PIN) | \
                CNF_PUSH_PULL(IO3_PIN)     | MODE_OUTPUT50(IO3_PIN);
    NSS_SET_PORT   |= WP_GPIO;
    NSS_RESET_PORT |= HOLD_GPIO;
}

void qpiGpioInit()
{
    SPI_PORT = CNF_PUSH_PULL(IO0_PIN)     | MODE_OUTPUT50(IO0_PIN) | \
               CNF_PUSH_PULL(IO1_PIN)     | MODE_OUTPUT50(IO1_PIN) | \
               CNF_PUSH_PULL(IO2_PIN)     | MODE_OUTPUT50(IO2_PIN) | \
               CNF_PUSH_PULL(IO3_PIN)     | MODE_OUTPUT50(IO3_PIN) | \
               CNF_PUSH_PULL(SCK_PIN)     | MODE_OUTPUT50(SCK_PIN) | \
               CNF_PUSH_PULL(NSS_PIN)     | MODE_OUTPUT50(NSS_PIN);
    NSS_SET_PORT   |= NSS_GPIO;
    NSS_RESET_PORT |= IO0_PIN | IO1_PIN | IO2_PIN | IO3_PIN | SCK_PIN;
}

void qpiWrite(uint8_t data)
{
    NSS_RESET_PORT |= NSS_GPIO;
    if(data & (1<<4)) {
        NSS_SET_PORT |= IO0_GPIO;
    } else {
        NSS_RESET_PORT |= IO0_GPIO;
    }
    if(data & (1<<5)) {
        NSS_SET_PORT |= IO1_GPIO;
    } else {
        NSS_RESET_PORT |= IO1_GPIO;
    }
    if(data & (1<<6)) {
        NSS_SET_PORT |= IO2_GPIO;
    } else {
        NSS_RESET_PORT |= IO2_GPIO;
    }
    if(data & (1<<7)) {
        NSS_SET_PORT |= IO3_GPIO;
    } else {
        NSS_RESET_PORT |= IO3_GPIO;
    }
    rough_delay_us(100);
    NSS_SET_PORT |= SCK_GPIO;
    rough_delay_us(100);
    if(data & (1<<0)) {
        NSS_SET_PORT |= IO0_GPIO;
    } else {
        NSS_RESET_PORT |= IO0_GPIO;
    }
    if(data & (1<<1)) {
        NSS_SET_PORT |= IO1_GPIO;
    } else {
        NSS_RESET_PORT |= IO1_GPIO;
    }
    if(data & (1<<2)) {
        NSS_SET_PORT |= IO2_GPIO;
    } else {
        NSS_RESET_PORT |= IO2_GPIO;
    }
    if(data & (1<<3)) {
        NSS_SET_PORT |= IO3_GPIO;
    } else {
        NSS_RESET_PORT |= IO3_GPIO;
    }
    NSS_RESET_PORT |= SCK_GPIO;
    rough_delay_us(100);
    NSS_SET_PORT |= SCK_GPIO;
    rough_delay_us(100);
    NSS_RESET_PORT |= SCK_GPIO;
}

int spiWriteByte(uint8_t data)
{
    NSS_RESET_PORT |= NSS_GPIO;
    // writing
    int tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & TXE) == 0) && (--tOut>0) );
    SPI1_DR = data;
    // waiting for transfer
    tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & BSY) != 0) && (--tOut>0) );
    NSS_SET_PORT |= NSS_GPIO;
    if(tOut <= 0) {
        return -1;
    }
    return 0;
}

int spiStart(uint8_t command, uint8_t data1)
{
    NSS_RESET_PORT |= NSS_GPIO;
    int tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & TXE) == 0) && (--tOut>0) );
    SPI1_DR = command;
    tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & TXE) == 0) && (--tOut>0) );
    SPI1_DR = data1;
    if(tOut <= 0) return -1;
    return 0;
}

uint8_t spiReadWrite(uint8_t data)
{
    // receiving
    int tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & RXNE) == 0) && (--tOut>0) );
    uint8_t rxData = (uint8_t)SPI1_DR;
    // writing
    tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & TXE) == 0) && (--tOut>0) );
    SPI1_DR = data;
    return rxData;
}


int spiFinalize()
{
    int tOut = SPI_TIMEOUT;
    while( ((SPI1_SR & BSY) != 0) && (--tOut>0) );
    NSS_SET_PORT |= NSS_GPIO;
    if(tOut <= 0) {
        return -1;
    }
    return 0;
}

int spiFlashReadPage(uint32_t address, int size)
{
    if( spiStart(READ_DATA, (uint8_t)(address >> 16)) < 0 ) return -1;
    spiReadWrite((uint8_t)(address >> 8));
    spiReadWrite((uint8_t)address);
    for(int i=0 ; i<size ; ++i) {
        flashToUsbBuffer[i] = spiReadWrite(0);
    }
    return spiFinalize();
}

int spiFlashReadAll()
{
    if( spiStart(READ_DATA, 0x00) < 0 ) return -1;
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    //escaping dummy bytes
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);

    int cnt = 0;
    flashToUsbBuffer[cnt++] = spiReadWrite(0x00);
    for(int i=1 ; i<(W25Q64_SIZE+1) ; ++i)
//    for(int i=1 ; i<(1024) ; ++i)
    {
        flashToUsbBuffer[cnt++] = spiReadWrite(0x00);
        if(cnt == 64) {
            vcpTx(flashToUsbBuffer, VCP_MAX_SIZE);
            cnt = 0;
        }
    }
    return spiFinalize();
}


int spiFlashWritePage(uint32_t address, int size)
{
    spiWriteByte(WRITE_ENABLE);
    rough_delay_us(1);
    if( spiStart(PAGE_PROGRAM, (uint8_t)(address >> 16)) < 0 ) return -1;
    spiReadWrite((uint8_t)(address >> 8));
    spiReadWrite((uint8_t)address);
    for(int i=0 ; i<size ; ++i) {
        spiReadWrite(flashToUsbBuffer[i]);
    }
    return spiFinalize();
}

int spiFlashDisableWriteProtect()
{
    spiWriteByte(ENABLE_RESET);
    rough_delay_us(1);
    spiWriteByte(RESET);
    delay_ms(10);

    spiWriteByte(VOLATILE_SR_WRITE_EN);
    rough_delay_us(1);
    spiStart(WRITE_STATUS_REGISTER, 0x00);
    spiReadWrite(0x00);
    spiFinalize();
    delay_ms(30);

    spiWriteByte(WRITE_ENABLE);
    rough_delay_us(1);
    spiStart(WRITE_STATUS_REGISTER, 0x00);
    spiReadWrite(0x00);
    spiFinalize();
    delay_ms(30);

    return spiWriteByte(WRITE_ENABLE);
}

int spiFlashWaitForBusy()
{
    uint8_t statusReg = 0x99;
    int tOut = SPI_TIMEOUT;

    if( spiStart(READ_STATUS_REGISTER1, 0x00) < 0 ) return -1;
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    while(((statusReg & W25Q64_BUSY) != 0) && (--tOut>0)) {
        statusReg = spiReadWrite(0x00);
    }
    spiFinalize();
    if( tOut <= 0 ) {
        return -1;
    }
    return 0;
}

int spiFlashErase()
{
    spiWriteByte(WRITE_ENABLE);
    rough_delay_us(1);
    if( spiWriteByte(CHIP_ERASE) < 0 ) return -1;
    rough_delay_us(1);
    return spiFlashWaitForBusy();
}

int spiFlashErasePage(uint32_t address)
{
    spiWriteByte(WRITE_ENABLE);
    rough_delay_us(1);
    if( spiStart(SECTOR_ERASE, (uint8_t)(address >> 16)) < 0 ) return -1;
    spiReadWrite((uint8_t)(address >> 8));
    spiReadWrite((uint8_t)address);
    spiFinalize();
    rough_delay_us(1);

    return spiFlashWaitForBusy();
}


uint16_t spiFlashReadStatus()
{
    uint16_t status1, status2;
    spiStart(READ_STATUS_REGISTER1, 0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    status1 = spiReadWrite(0x00);
    spiFinalize();
    rough_delay_us(1);
    spiStart(READ_STATUS_REGISTER2, 0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    spiReadWrite(0x00);
    status2 = spiReadWrite(0x00);
    spiFinalize();
    return (status1 | (uint16_t)(status2 << 8));
}

uint64_t spiFlashReadId()
{
    spiStart(READ_UNIQUE_ID, 0x00);
    for(int i=0 ; i<4 ; ++i) {
        spiReadWrite(0x00);
    }
    uint64_t id = 0;
    for(int i=1 ; i<=8 ; ++i) {
        id |= (uint64_t)(((uint64_t)spiReadWrite(0x00)) << ((8-i)*8));
    }
    spiFinalize();
    return id;
}

void spiFlashDisableQpi()
{
    spiWriteByte(ENABLE_QPI);
    rough_delay_us(200);
    qpiGpioInit();
    rough_delay_us(200);
    qpiWrite(WRITE_STATUS_REGISTER);
    qpiWrite(0x00);
    qpiWrite(0x00);
    NSS_SET_PORT |= NSS_GPIO;
    rough_delay_us(200);
    qpiWrite(DISABLE_QPI);
    NSS_SET_PORT |= NSS_GPIO;
    rough_delay_us(200);

    spiFlashInit();
}

void spiFlashWriteEnable()
{
    spiWriteByte(WRITE_ENABLE);
}

//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
