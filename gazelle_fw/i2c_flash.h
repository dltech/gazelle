#ifndef H_I2C_FLASH
#define H_I2C_FLASH
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
#include <inttypes.h>
#include "../lib/regs/gpio_regs.h"

#define I2C_PORT    GPIOB_CRL
#define I2C_PIN1    6
#define I2C_PIN2    7

#define PAGE_SIZE           32
#define HEADER_OFFSET       3
#define I2C_BUFFER_SIZE     PAGE_SIZE + HEADER_OFFSET

#define ADDR_24CXX_WRITE    0xa0
#define ADDR_24CXX_READ     0xa1

void i2cFlashInit(void);
int i2cFlashReadPage(uint16_t startAddress, int size);
int i2cFlashWritePage(uint16_t startAddress, int size);


#endif
