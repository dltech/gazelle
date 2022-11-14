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

gazelleHeaderTyp head;
uint8_t flashToUsbBuffer[SPI_MAX_BYTES_TO_WRITE] = {0x00};

const uint8_t commands[numOfCmds][cfgStrSize] = {{'m','2','4','c','x','x','w','r'},
                                                {'m','2','4','c','x','x','r','d'},
                                                {'w','2','5','q','x','x','e','r'},
                                                {'w','2','5','q','x','x','w','r'},
                                                {'w','2','5','q','x','x','r','d'} };

const uint8_t msg[numOfMsg][cfgStrSize] = {{'o','k',' ',' ',' ',' ',' ',0},
                                           {'n','o',' ','a','c','k',' ',0},
                                           {'s','p','i',' ','e','r','r',0}};

int findCmd(uint8_t *inputMsg);
void parseHeader(uint8_t *data);

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

void parseHeader(uint8_t *data)
{
    int newCommand = findCmd(data);
    if(newCommand != 0) {
        head.command = newCommand;
        head.addr = (((uint32_t)data[cfgStrSize])<<16) + \
                  (((uint32_t)data[cfgStrSize+1])<<8) + \
                   ((uint32_t)data[cfgStrSize+2]);
        head.payloadSize = (uint16_t)((((uint16_t)data[cfgStrSize+3])<<8) + \
                            ((uint16_t)data[cfgStrSize+4]));
        head.packCnt = 0;
    }
}

void flasher(uint8_t *data, int size)
{
    if(size < cmdHeaderOffs) return;
    // try to catch new command
    parseHeader(data);

    // main of multifunction flasher
    switch (head.command) {
        case I2C_FLASH_WRITE:
            for(int i=0 ; i<head.payloadSize ; ++i) {
                flashToUsbBuffer[i] = data[i+cmdHeaderOffs];
            }
            i2cFlashWritePageBlocking((uint16_t)head.addr,(uint8_t)head.payloadSize);
            if( waitWriteOp() >= 0) {
                vcpTx((uint8_t*)msg[MSG_OK],cfgStrSize);
            } else {
                vcpTx((uint8_t*)msg[MSG_NO_ACK],cfgStrSize);
            }
            head.command = CMD_FINISHED;
            break;
        case I2C_FLASH_READ:
            if( i2cFlashReadPageBlocking((uint16_t)head.addr,PAGE_SIZE) == 0 ) {
                vcpTx(flashToUsbBuffer,head.payloadSize);
            } else {
                vcpTx((uint8_t*)msg[MSG_NO_ACK],cfgStrSize);
            }
            head.command = CMD_FINISHED;
            break;
        case SPI_FLASH_ERASE:
            if( spiFlashErase() >= 0 ) {
                vcpTx((uint8_t*)msg[MSG_OK],cfgStrSize);
            } else {
                vcpTx((uint8_t*)msg[SPI_ERROR],cfgStrSize);
            }
            head.command = CMD_FINISHED;
            break;
        case SPI_FLASH_WRITE:
            if(head.packCnt > head.payloadSize) break;
            if( head.packCnt == 0 ) {
                for(int i=cmdHeaderOffs ; i < size ; ++i) {
                    flashToUsbBuffer[head.packCnt++] = data[i];
                }
            } else {
                for(int i=0 ; i < size ; ++i) {
                    flashToUsbBuffer[head.packCnt++] = data[i];
                }
            }
            if(head.packCnt == head.payloadSize) {
                spiFlashWritePage(head.addr,head.payloadSize);
                if( spiFlashWaitForBusy() >= 0 ) {
                    vcpTx((uint8_t*)msg[MSG_OK],cfgStrSize);
                } else {
                    vcpTx((uint8_t*)msg[SPI_ERROR],cfgStrSize);
                }
                head.packCnt = 0;
                head.command = CMD_FINISHED;
            }
            break;
        case SPI_FLASH_READ:
            if( spiFlashReadAll() < 0 ) {
                vcpTx((uint8_t*)msg[SPI_ERROR],cfgStrSize);
            }
            head.command = CMD_FINISHED;
            break;
    }
}
