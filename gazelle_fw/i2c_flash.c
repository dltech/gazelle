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

int i2cTxByteInner(uint8_t byte);
int i2cTxAddrInner(uint8_t addr);

int i2cTxAddrInner(uint8_t addr)
{
    I2C1_CR1 |= START;

    I2C1_DR = addr;
    int32_t tOut = 1e6;
    while( ((I2C1_SR1 & ADDR) != 0 ) || (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    tOut = (int32_t)I2C1_SR1;
    tOut = (int32_t)I2C1_SR2;
    return 0;
}

int i2cTxByteInner(uint8_t byte)
{
    I2C1_DR = byte;
    int32_t tOut = 1e6;
    while( ((I2C1_SR1 & ITXE) != 0 ) || (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    return 0;
}

void i2cFlashInit()
{
    // module clocking
    RCC_APB1ENR |= I2C1EN;
    RCC_APB1ENR |= IOPBEN;
    RCC_AHBENR  |= DMA1EN;
    // port config
    I2C_PORT = CNF_AF_PUSH_PULL(I2C_PIN1) | MODE_OUTPUT50(I2C_PIN1)\
             | CNF_AF_PUSH_PULL(I2C_PIN2) | MODE_OUTPUT50(I2C_PIN2);
    // clocking of the interface
    I2C1_CR2 = IDMAEN | ITERREN | (36 & FREQ_MSK);
    I2C1_CCR = F_S | DUTY | I2C400K;
    I2C1_TRISE = TRISE_NS(50);
    I2C1_OAR1 = ADDR_24CXX_READ;
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
    err =  i2cTxAddrInner(ADDR_24CXX_READ);
    err += i2cTxByteInner((uint8_t)(startAddress >> 8));
    err += i2cTxByteInner((uint8_t)startAddress);
    I2C1_CR1 |= STOP;

    // starting reading
    err += i2cTxAddrInner(ADDR_24CXX_READ);
    // reading data through DMA
    DMA1_CNDTR7 = (uint32_t)size;
    DMA1_CCR7 |= DMA_EN;
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
    DMA1_CNDTR6 = (uint32_t)size;
    flashToUsbBuffer[0] = ADDR_24CXX_WRITE;
    flashToUsbBuffer[1] = (uint8_t)(startAddress >> 8);
    flashToUsbBuffer[2] = (uint8_t) startAddress;
    DMA1_CCR6 |= DIR | DMA_EN;
    I2C1_CR1 |= START;
    return 0;
}

void I2C1_EV_Handler(void)
{

}

void I2C1_ER_Handler(void)
{

}
