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
//#include "flash.h"
#include "i2c_flash.h"

extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];

int i2cTxByteInner(uint8_t byte);
int i2cTxAddrInner(uint8_t addr);
int i2cTxStop(void);
int i2cRxByteInner(void);


int i2cTxAddrInner(uint8_t addr)
{
    // wait for a busy state of the interface
    int32_t tOut = 1e4;
    while( ((I2C1_SR2 & BUSY) != 0) && (--tOut > 0) );
    if(tOut <= 0) {
        return -1;
    }
    I2C1_SR1 = 0;
    (void)I2C1_SR2;
    (void)I2C1_SR1;
    // sending start bit with control sequence
    I2C1_CR1 |= START;
    tOut = 1e4;
    while( ((I2C1_SR1 & SB) == 0 ) && (--tOut > 0) );
    if(tOut <= 0) {
        return -1;
    }
    // sending address with control sequence
    I2C1_DR = addr;
    (void)I2C1_SR1;
    tOut = 1e6;
    while( ((I2C1_SR1 & ADDR) == 0) && ((I2C1_SR1 & AF) == 0) && (--tOut > 0) );
    if(tOut <= 0) {
        return -1;
    }
    if( (I2C1_SR1 & AF) != 0 ) {
        return -2;
    }

    // resetting all status bits
    I2C1_SR1 = 0;
    (void)I2C1_SR2;
    (void)I2C1_SR1;

    return 0;
}

int i2cTxByteInner(uint8_t byte)
{
    int32_t tOut = 1e4;
    while(((I2C1_SR1 & ITXE) == 0) && ((I2C1_SR1 & AF) == 0) && (--tOut > 0));
    if(tOut <= 0) {
        return -1;
    }
    if((I2C1_SR1 & AF) != 0) {
        return -2;
    }
    I2C1_DR = byte;

    return 0;
}

int i2cRxByteInner(void)
{
    int32_t tOut = 1e4;
    while( ((I2C1_SR1 & IRXNE) == 0 ) && (--tOut > 0) );
    if(tOut <= 0) {
        return -1;
    }
    return (uint8_t)I2C1_DR;
}

int i2cTxStop(void)
{
    int32_t tOut = 1e4;
    while((((I2C1_SR1 & BTF) == 0) || ((I2C1_SR1 & ITXE) == 0)) && (--tOut > 0));
    I2C1_CR1 |= STOP;
    if(tOut <= 0) {
        return -1;
    }

    return 0;
}


volatile uint32_t clock;
volatile uint32_t clock1;

void i2cFlashInit()
{
    // module clocking
    RCC_APB1ENR |= I2C1EN;
    RCC_APB2ENR |= IOPBEN;
    RCC_AHBENR  |= DMA1EN;
    // port config
    I2C_PORT = CNF_AF_OPEN_DRAIN(I2C_PIN1) | MODE_OUTPUT50(I2C_PIN1)\
             | CNF_AF_OPEN_DRAIN(I2C_PIN2) | MODE_OUTPUT50(I2C_PIN2);
    // clocking of the interface
    I2C1_CR2 = ITERREN | (36 & FREQ_MSK) | IDMAEN;
    I2C1_CCR = F_S | DUTY | I2C400K;
    I2C1_TRISE = TRISE_NS(50);
    I2C1_OAR1 = WTF | ADDR_24CXX_READ;
    // i2c enable
    I2C1_CR1 = PE;
    // dma init
    // tx channel
    DMA1_CPAR6 = (uint32_t) &I2C1_DR;
    DMA1_CMAR6 = (uint32_t) &flashToUsbBuffer;
    DMA1_CNDTR6 = (uint32_t) I2C_BUFFER_SIZE;
    DMA1_CCR6 = MINC | MSIZE_8BIT | PSIZE_8BIT | PL_VERY_HIGH | TEIE | DIR | TCIE;
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    // rx channel
    DMA1_CPAR7 = (uint32_t) &I2C1_DR;
    DMA1_CMAR7 = (uint32_t) &flashToUsbBuffer;
    DMA1_CNDTR7 = (uint32_t) I2C_BUFFER_SIZE;
    DMA1_CCR7 = MINC | MSIZE_8BIT | PSIZE_8BIT | PL_VERY_HIGH | TEIE;
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}


int i2cFlashReadPage(uint16_t startAddress, int size)
{
    int err;
    // setting page address
    err =  i2cTxAddrInner(ADDR_24CXX_WRITE);
    err += i2cTxByteInner((uint8_t)(startAddress >> 8));
    err += i2cTxByteInner((uint8_t)startAddress);
    err += i2cTxStop();

    // starting reading
    err += i2cTxAddrInner(ADDR_24CXX_READ);
    // reading data
    I2C1_CR1 |= IACK;
    DMA1_CNDTR7 = (uint32_t)size;
    DMA1_CCR7 |= DMA_EN;

    return err;
}

int i2cFlashReadPageBlocking(uint16_t startAddress, int size)
{
    int err;
    // setting page address
    err =  i2cTxAddrInner(ADDR_24CXX_WRITE);
    err += i2cTxByteInner((uint8_t)(startAddress >> 8));
    err += i2cTxByteInner((uint8_t)startAddress);
    err += i2cTxStop();

    // starting reading
    err += i2cTxAddrInner(ADDR_24CXX_READ);
    // reading data
    I2C1_CR1 |= IACK;

    int tmp;
    for(int i=0 ; i<size ; ++i)
    {
        tmp = i2cRxByteInner();
        if (i == (size - 2)) {
            I2C1_CR1 &= ~((uint32_t)IACK);
            I2C1_CR1 |= STOP;
        }
        if( tmp < 0 ) {
            err += tmp;
        } else {
            flashToUsbBuffer[i] = (uint8_t)tmp;
        }
    }
    return err;
}


int i2cFlashWritePage(uint16_t startAddress, int size)
{
    if(size > PAGE_SIZE) {
        return -1;
    }
    int err;
    // prepare transfer buffer first
    DMA1_CNDTR6 = (uint32_t)size+2;
    flashToUsbBuffer[0] = (uint8_t)(startAddress >> 8);
    flashToUsbBuffer[1] = (uint8_t) startAddress;
    // then initiate a transmittion
    err =  i2cTxAddrInner(ADDR_24CXX_WRITE);
    // enable dma in order to transmit page
    DMA1_CCR6 |= DMA_EN;

    return err;
}

int i2cFlashWritePageBlocking(uint16_t startAddress, int size)
{
    if(size > PAGE_SIZE) {
        return -1;
    }
    int err;

    err =  i2cTxAddrInner(ADDR_24CXX_WRITE);
    err += i2cTxByteInner((uint8_t)(startAddress >> 8));
    err += i2cTxByteInner((uint8_t)startAddress);
    for (int i=0 ; i<size ; ++i) {
        err += i2cTxByteInner(flashToUsbBuffer[i]);
    }
    err += i2cTxStop();

    return err;
}

int waitWriteOp()
{
    int tmp=-2, tout=1e3;
    while( (tmp == -2) && (--tout > 0) ) {
        tmp = i2cTxAddrInner(ADDR_24CXX_WRITE);
        I2C1_CR1 |= STOP;
    }
    if( tout <= 0 ) {
        return -1;
    }
    return 0;
}

void DMA1_Channel6_Handler(void)
{
    I2C1_CR1 |= STOP;
    DMA1_IFCR = 0xfffffff;
}

void DMA1_Channel7_Handler(void)
{
    I2C1_CR1 &= ~((uint32_t)IACK);
    I2C1_CR1 |= STOP;
    DMA1_IFCR = 0xfffffff;
}


void I2C1_EV_Handler(void)
{

}

void I2C1_ER_Handler(void)
{

}
