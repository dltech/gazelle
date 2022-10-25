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

#include "string.h"
#include "i2c_flash.h"
#include "spi_flash.h"
#include "usb_core.h"
#include "flash.h"

int command;
uint32_t address;
int payloadSize;

uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE] = {0x00};

const uint8_t commands[numOfCmds][cfgStrSize] = {{'m','2','4','c','x','x','w','r'},
                                                {'m','2','4','c','x','x','r','d'},
                                                {'w','2','5','q','x','x','w','r'},
                                                {'w','2','5','q','x','x','r','d'} };

const uint8_t msg[numOfMsg][cfgStrSize] = {{'o','k',' ',' ',' ',' ',' ',0},
                                           {'n','o',' ','a','c','k',' ',0} };

int findCmd(uint8_t *inputMsg);


void flashInit()
{
    // initialization of all interfaces
    i2cFlashInit();
    spiFlashInit();
}

int findCmd(uint8_t *inputMsg)
{
    int match = 0;
    for(int i=0 ; i<numOfCmds ; ++i) {
        for(int j=0 ; j<cfgStrSize ; ++j) {
            if(inputMsg[j] == commands[i][j]) ++match;
        }
        if(match == cfgStrSize) return i+1;
        match = 0;
    }
    return 0;
}



void flasher(uint8_t *data, int size)
{
    static int cnt = 0;
    if(size < cmdHeaderOffs) return;
    // try to catch new command
    int newCommand = findCmd(data);
    if(newCommand != 0) {
        command = newCommand;
        address = (((uint16_t)data[cfgStrSize])<<16) + \
                  (((uint16_t)data[cfgStrSize+1])<<8) + \
                  (uint16_t)data[cfgStrSize+2];
        payloadSize = data[cfgStrSize+3];
        cnt = 0;
    }
    // main of multifunction flasher
    switch (command) {
        case I2C_FLASH_WRITE:
            for(int i=0 ; i<payloadSize ; ++i) {
                flashToUsbBuffer[i] = data[i+cmdHeaderOffs];
            }
            i2cFlashWritePageBlocking(address,payloadSize);
            if( waitWriteOp() == 0) {
                vcpTx((uint8_t*)msg[MSG_OK],cfgStrSize);
            } else {
                vcpTx((uint8_t*)msg[MSG_NO_ACK],cfgStrSize);
            }
            command = CMD_FINISHED;
            break;
        case I2C_FLASH_READ:
            if( i2cFlashReadPageBlocking(address,PAGE_SIZE) == 0 ) {
                vcpTx(flashToUsbBuffer,payloadSize);
            } else {
                vcpTx((uint8_t*)msg[MSG_NO_ACK],cfgStrSize);
            }
            command = CMD_FINISHED;
            break;
        case SPI_FLASH_WRITE:
            for(cnt ; (cnt%VCP_MAX_SIZE) != 0 ; ++cnt) {
                flashToUsbBuffer[cnt] = data[cnt+cmdHeaderOffs];
            }

            spiFlashWritePage(address,size);
            command = CMD_FINISHED;
            break;
        case SPI_FLASH_READ:
            spiFlashReadAll();
            command = CMD_FINISHED;
            break;
    }
}
