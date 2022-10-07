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

uint8_t flashToUsbBuffer[I2C_BUFFER_SIZE] = {64, 64, 64, 64, 64, 64, 64, 64,
                                             64, 64, 64, 64, 64, 64, 64, 64,
                                             64, 64, 64, 64, 64, 64, 64, 64,
                                             64, 64, 64, 64, 64, 64, 64, 64,
                                             64, 64};
volatile int command;
const uint8_t commands[numOfCmds][cfgStrSize] = {{'m','2','4','c','x','x','w','r',0},
                                                {'m','2','4','c','x','x','r','d',0},
                                                {'w','2','5','q','x','x','w','r',0},
                                                {'w','2','5','q','x','x','r','d',0} };


void flashInit()
{
    // initialization of all interfaces
    i2cFlashInit();
    command = FLASH_DISABLED;
}

void flasher(uint8_t *data, int size)
{
    // try to catch new command
    char* pch;
    for( int i=0 ; i<numOfCmds ; i++ )
    {
        pch = strstr((char*)data,(char*)commands[i]);
        if (pch != NULL) {
            command = i;
            break;
        }
    }
    command = size;
    // main of multifunction flasher
/*    switch (command) {
        case I2C_FLASH_WRITE:
            break;
        case I2C_FLASH_WRITE:
            break;
        case I2C_FLASH_WRITE:
            break;
        case I2C_FLASH_WRITE:
            break;
    } */
}
