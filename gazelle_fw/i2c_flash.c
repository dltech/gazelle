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

extern uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];

void i2cFlashInit()
{
    // module clocking
    RCC_APB1ENR |= I2C1EN;
    RCC_AHBENR  |= DMA1EN;
    // clocking of the interface
    I2C1_CR2 = IDMAEN | ITERREN | (36 & FREQ_MSK);
    I2C1_CCR = F_S | DUTY | I2C400K;
    I2C1_TRISE = TRISE_NS(50);
    I2C1_OAR1 = ADDR_24CXX_READ;
    I2C1_CR1 = PE;
    // dma init
    DMA1_CPAR1 = (uint32_t) &I2C1_DR;
    DMA1_CMAR1 = (uint32_t) &flashToUsbBuffer;
    DMA1_CNDTR1 = (uint32_t) USBPACKSIZE;
    DMA1_CCR1 = MINC | MSIZE_8BIT | PSIZE_8BIT | PL_VERY_HIGH | TEIE;
}

int i2cFlashReadByte(uint16_t address)
{


}

int i2cFlashReadPage(uint16_t startAddress, int size)
{
    // setting page address
    I2C1_CR1 |= START;
    // sending slave address procedure
    I2C1_DR = ADDR_24CXX_READ;
    int32_t tOut = 1e6;
    while( ((I2C1_SR1 & ADDR) != 0 ) || (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    uint32_t temp = I2C1_SR1;
    temp = I2C1_SR2;
    // sending page address
    I2C1_DR = (uint8_t)(startAddress >> 8);
    while( ((I2C1_SR1 & ITXE) != 0 ) || (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    I2C1_DR = (uint8_t)(startAddress);
    while( ((I2C1_SR1 & ITXE) != 0 ) || (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    I2C1_CR1 |= STOP;

    // starting reading
    I2C1_CR1 |= START;
    // sending slave address procedure
    I2C1_DR = ADDR_24CXX_READ;
    int32_t tOut = 1e6;
    while( ((I2C1_SR1 & ADDR) != 0 ) || (--tOut > 0) );
    if(tOut < 0) {
        return -1;
    }
    uint32_t temp = I2C1_SR1;
    temp = I2C1_SR2;
    // reading data through DMA
    DMA1_CCR1 &= ~(DIR);
    DMA1_CCR1 |= DMA_EN;
}

int i2cFlashWriteByte(uint16_t address)
{
}

int i2cFlashWritePage(uint16_t startAddress, int size)
{
    if(size > PAGE_SIZE) {
        return -1;
    }
    DMA1_CNDTR1 = size;
    flashToUsbBuffer[0] = ADDR_24CXX_WRITE;
    flashToUsbBuffer[1] = (uint8_t)(startAddress >> 8);
    flashToUsbBuffer[2] = (uint8_t) startAddress;
    DMA1_CCR1 |= DIR | DMA_EN;
    I2C1_CR1 |= START;
}

void I2C1_EV_Handler(void)
{

}

void I2C1_ER_Handler(void)
{

}
