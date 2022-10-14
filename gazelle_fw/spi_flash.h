#ifndef H_SPI_FLASH
#define H_SPI_FLASH
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

#include <inttypes.h>
#include "../lib/regs/gpio_regs.h"

#define SPI_PORT    GPIOA_CRL

#define SCK_PIN     5
#define MISO_PIN    6
#define MOSI_PIN    7
#define NSS_PIN     3


void spiFlashInit(void);
int spiFlashReadByte(uint16_t address);
int spiFlashReadPage(uint16_t startAddress, int size);


#endif
