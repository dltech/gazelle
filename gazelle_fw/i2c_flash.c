/*
 * Part of USB universal flaser.
 * Here is I2C flash access functions.
 *
 * Copyright 2022 Mikhail Belkin <dltech174@gmail.com>
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
#include "../lib/regs/i2c_regs.h"
#include "../lib/regs/dma_regs.h"
#include "flash.h"
#include "i2c_flash.h"

extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];

int stage = 0;
int toutTrace[50];
uint32_t sr21, sr22, sr12, sr13, sr23, sr24, sr14, sr15, sr26, sr16, sr17, sr18;

int i2cTxByteInner(uint8_t byte);
int i2cTxAddrInner(uint8_t addr);
int i2cTxStop(void);
int i2cRxByteInner(void);


int i2cTxAddrInner(uint8_t addr)
{
    sr21 = I2C1_SR2;
    // wait for a busy state of the interface
    int32_t tOut = 1e6;
    while( ((I2C1_SR2 & BUSY) != 0) && (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    toutTrace[stage++] = tOut;

    I2C1_SR1 = 0;
    (void)I2C1_SR2;
    (void)I2C1_SR1;

    sr22 = I2C1_SR2;
    sr12 = I2C1_SR1;
    // sending start bit with control sequence
    I2C1_CR1 |= START;
    tOut = 1e6;
    while( ((I2C1_SR1 & SB) == 0 ) && (--tOut > 0) );

    sr23 = I2C1_SR2;
    sr13 = I2C1_SR1;
    toutTrace[stage++] = tOut;

    // sending address with control sequence
    I2C1_DR = addr;
    (void)I2C1_SR1;
    sr24 = I2C1_SR2;
    sr14 = I2C1_SR1;
    tOut = 1e6;
    while( ((I2C1_SR1 & ADDR) == 0) && ((I2C1_SR1 & AF) == 0) && (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    // if( (I2C1_SR1 & AF) != 0 ) {
    //     return -2;
    // }

    toutTrace[stage] = tOut;
    sr15 = I2C1_SR1;

    // resetting all status bits
    I2C1_SR1 = 0;
    (void)I2C1_SR2;
    (void)I2C1_SR1;

    sr26 = I2C1_SR2;
    sr16 = I2C1_SR1;

    return 0;
}

int i2cTxByteInner(uint8_t byte)
{
    sr17 = I2C1_SR1;
    int32_t tOut = 1e6;
    while(((I2C1_SR1 & ITXE) == 0) && ((I2C1_SR1 & AF) == 0) && (--tOut > 0));
    if(tOut < 0) {
        return -1;
    }
    if((I2C1_SR1 & AF) != 0) {
        return -2;
    }
    I2C1_DR = byte;

    sr18 = I2C1_SR1;
    toutTrace[stage] = tOut;
    return 0;
}

int i2cRxByteInner(void)
{
    int32_t tOut = 1e6;
    while( ((I2C1_SR1 & IRXNE) == 0 ) && (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    toutTrace[stage] = tOut;
    return (uint8_t)I2C1_DR;
}

int i2cTxStop(void)
{
    int32_t tOut = 1e6;
    while( ((I2C1_SR1 & BTF) == 0 ) && (--tOut > 0) );
    I2C1_CR1 |= STOP;
    if(tOut < 0) {
        return -1;
    }
    toutTrace[stage] = tOut;
    return 0;
}


volatile uint32_t clock;
volatile uint32_t clock1;

void i2cFlashInit()
{
    clock = I2C400K;
    clock1 = TRISE_NS(500);
    // module clocking
    RCC_APB1ENR |= I2C1EN;
    RCC_APB2ENR |= IOPBEN;
    RCC_AHBENR  |= DMA1EN;
    // port config
    I2C_PORT = CNF_AF_OPEN_DRAIN(I2C_PIN1) | MODE_OUTPUT50(I2C_PIN1)\
             | CNF_AF_OPEN_DRAIN(I2C_PIN2) | MODE_OUTPUT50(I2C_PIN2);
    // clocking of the interface
    I2C1_CR2 = ITERREN | (36 & FREQ_MSK); // | IDMAEN;
    I2C1_CCR = 180;//F_S | DUTY | I2C400K;
    I2C1_TRISE = TRISE_NS(50);
    I2C1_OAR1 = WTF | ADDR_24CXX_READ;
    // i2c enable
    I2C1_CR1 = PE;
    // dma init
    // tx channel
    DMA1_CPAR6 = (uint32_t) &I2C1_DR;
    DMA1_CMAR6 = (uint32_t) &flashToUsbBuffer;
    DMA1_CNDTR6 = (uint32_t) I2C_BUFFER_SIZE;
    DMA1_CCR6 = MINC | MSIZE_8BIT | PSIZE_8BIT | PL_VERY_HIGH | TEIE | DIR;
    // rx channel
    DMA1_CPAR7 = (uint32_t) &I2C1_DR;
    DMA1_CMAR7 = (uint32_t) &flashToUsbBuffer;
    DMA1_CNDTR7 = (uint32_t) I2C_BUFFER_SIZE;
    DMA1_CCR7 = MINC | MSIZE_8BIT | PSIZE_8BIT | PL_VERY_HIGH | TEIE;
}

uint8_t i2cFlashReadByte(uint16_t address)
{
    address++;
    return 0;
}


int i2cFlashReadPage(uint16_t startAddress, int size)
{
    int err;
    // setting page address
    err =  i2cTxAddrInner(ADDR_24CXX_WRITE);
    ++stage;
    err += i2cTxByteInner((uint8_t)(startAddress >> 8));
    ++stage;
    err += i2cTxByteInner((uint8_t)startAddress);
    ++stage;
    err += i2cTxStop();

    // starting reading
    ++stage;
    err += i2cTxAddrInner(ADDR_24CXX_READ);
    // reading data through DMA
    I2C1_CR1 |= IACK;
    int tmp;
    for(int i=0 ; i<size ; ++i)
    {
        ++stage;
        if (i == size - 1) {
            I2C1_CR1 &= ~((uint32_t)IACK);
        }
        tmp = i2cRxByteInner();
        if( tmp < 0 ) {
            --err;
        } else {
            flashToUsbBuffer[i] = (uint8_t)tmp;
        }
    }
//    err += i2cTxStop();
//    DMA1_CNDTR7 = (uint32_t)size;
//    DMA1_CCR7 |= DMA_EN;
    I2C1_CR1 |= STOP;

    return err;
}

int i2cFlashWriteByte(uint16_t address)
{
    return address;
}


int i2cFlashWritePage(uint16_t startAddress, int size)
{
    if(size > PAGE_SIZE) {
        return -1;
    }
    int err;
    err =  i2cTxAddrInner(ADDR_24CXX_WRITE);
    ++stage;
    err += i2cTxByteInner((uint8_t)(startAddress >> 8));
    ++stage;
    err += i2cTxByteInner((uint8_t)startAddress);
    ++stage;
    err += i2cTxByteInner(0xeb);
    ++stage;
    err += i2cTxByteInner(0xec);
    ++stage;
    err += i2cTxByteInner(0xed);
    ++stage;
    err += i2cTxByteInner(0xee);
    ++stage;
    err += i2cTxStop();

    // DMA1_CNDTR6 = (uint32_t)size;
    // flashToUsbBuffer[0] = ADDR_24CXX_WRITE;
    // flashToUsbBuffer[1] = (uint8_t)(startAddress >> 8);
    // flashToUsbBuffer[2] = (uint8_t) startAddress;
    // DMA1_CCR6 |= DIR | DMA_EN;
    // I2C1_CR1 |= START;
    return err;
}

void I2C1_EV_Handler(void)
{

}

void I2C1_ER_Handler(void)
{

}
