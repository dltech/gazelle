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
#include "flash.h"
#include "string.h"
#include "i2c_flash.h"

uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE];
volatile int command;
void uint8_t commands[numOfCmds][cfgStrSize] = {{'m','2','4','c','x','x','w','r'},
                                                {'m','2','4','c','x','x','r','d'},
                                                {'w','2','5','q','x','x','w','r'},
                                                {'w','2','5','q','x','x','r','d'} };

void i2cInit(void);
void spiInit(void);

void i2cInit()
{

}

void spiInit()
{

}

void flashInit()
{
    // initialization of all interfaces
    i2cInit();
    spiInit();
    command = FLASH_DISABLED;
}

void flasher(uint8_t *data, int size)
{
    // try to catch new command
    for( int=0 ; i<numOfCmds ; i++ )
    {
        pch = strstr(data,commands[i]);
        if (pch != NULL) {
            command = i;
            break;
        }
    }
    // main of multifunction flasher
    switch (command) {
        case I2C_FLASH_WRITE:
            break;
        case I2C_FLASH_WRITE:
            break;
        case I2C_FLASH_WRITE:
            break;
        case I2C_FLASH_WRITE:
            break;
    }
}
