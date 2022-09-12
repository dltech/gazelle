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

void i2cFlashInit()
{
    // module clocking
    RCC_APB1ENR |= I2C1EN;
    RCC_AHBENR  |= DMA1EN;
    // clocking of the interface
    I2C1_CR2 = 36 & FREQ_MSK;
    I2C1_CCR = F_S | DUTY | I2C400K;
    I2C1_TRISE = TRISE_NS(50);
    //
}

uint8_t i2cFlashReadByte(uint16_t address);
int i2cFlashReadPage(uint16_t startAddress, int size);

uint8_t i2cFlashWriteByte(uint16_t address);
int i2cFlashWritePage(uint16_t startAddress, int size);
