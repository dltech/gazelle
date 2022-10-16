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
//#include "flash.h"
#include "spi_flash.h"

extern uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE];

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

int spiFlashReadAll()
{
    NSS_RESET_PORT |= NSS_PIN;

    SPI1_DR = READ_DATA;
    int32_t tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = 0x00;
    tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = 0x00;
    tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = 0x00;
    // tOut = TIMEOUT;
    // while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    // SPI1_DR = 0x00;
    // (void)SPI1_DR;

    for(int i=0 ; i<W25Q64_SIZE ; ++i)
    {
        if((i > 0) && ((i%VCP_MAX_SIZE) == 0)) {
            vcpTx(flashToUsbBuffer, VCP_MAX_SIZE);
        }
        // dummy write
        tOut = TIMEOUT;
        while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
        SPI1_DR = 0x00;
        // receiving
        tOut = TIMEOUT;
        while( ((SPI1_SR | RXNE) == 0) && (--tOut>0) );
        flashToUsbBuffer[i%64] = SPI1_DR;
    }

    tOut = TIMEOUT;
    while( ((SPI1_SR | BSY) == 0) && (--tOut>0) );
    NSS_SET_PORT |= NSS_PIN;
}



int spiFlashReadPage(uint16_t address, int size)
{
    NSS_RESET_PORT |= NSS_PIN;

    SPI1_DR = READ_DATA;
    int32_t tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = (uint8_t)(addr >> 16);
    tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = (uint8_t)(addr >> 8);
    tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = (uint8_t)addr;
    // tOut = TIMEOUT;
    // while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    // SPI1_DR = 0x00;
    // (void)SPI1_DR;

    for(int i=0 ; i<size ; ++i)
    {
        // dummy write
        tOut = TIMEOUT;
        while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
        SPI1_DR = 0x00;
        // receiving
        tOut = TIMEOUT;
        while( ((SPI1_SR | RXNE) == 0) && (--tOut>0) );
        flashToUsbBuffer[i] = SPI1_DR;
    }

    tOut = TIMEOUT;
    while( ((SPI1_SR | BSY) == 0) && (--tOut>0) );
    NSS_SET_PORT |= NSS_PIN;
}

int spiFlashWritePage(uint32_t address, int size)
{
    NSS_RESET_PORT |= NSS_PIN;

    SPI1_DR = PAGE_PROGRAM;
    int32_t tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = (uint8_t)(addr >> 16);
    tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = (uint8_t)(addr >> 8);
    tOut = TIMEOUT;
    while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
    SPI1_DR = (uint8_t)addr;

    for(int i=0 ; i<size ; ++i)
    {
        tOut = TIMEOUT;
        while( ((SPI1_SR | TXE) == 0) && (--tOut>0) );
        SPI1_DR = flashToUsbBuffer[i];
    }

    tOut = TIMEOUT;
    while( ((SPI1_SR | BSY) == 0) && (--tOut>0) );
    NSS_SET_PORT |= NSS_PIN;
}
//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
