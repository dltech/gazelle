#ifndef H_FLASH
#define H_FLASH
/*
 * Part of USB universal flaser.
 * Main menu with choosing target.
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

#define cfgStrSize      8
#define cmdHeaderOffs   cfgStrSize + 3 + 2
#define numOfCmds       5
#define numOfMsg        3

void flashInit(void);
void flasher(uint8_t *data, int size);

typedef struct {
    int command;
    uint32_t addr;
    uint16_t payloadSize;
    int packCnt;
} gazelleHeaderTyp;

enum cmdn {
    FLASH_DISABLED = 0,
    I2C_FLASH_WRITE,
    I2C_FLASH_READ,
    SPI_FLASH_ERASE,
    SPI_FLASH_WRITE,
    SPI_FLASH_READ,
    CMD_FINISHED
};

enum mesgn {
    MSG_OK = 0,
    MSG_NO_ACK,
    SPI_ERROR
};


#endif
