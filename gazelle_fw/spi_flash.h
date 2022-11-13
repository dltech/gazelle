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

#define TIMEOUT                 1e7
#define SPI_MAX_BYTES_TO_WRITE  256
#define W25Q64_SIZE             8388608

// interface GPIO config
#define SPI_PORT    GPIOA_CRL
#define SCK_PIN     5
#define MISO_PIN    6
#define MOSI_PIN    7
#define NSS_PIN     3
#define IO0_PIN     MOSI_PIN
#define IO1_PIN     MISO_PIN
#define IO2_PIN     2
#define IO3_PIN     1

#define NSS_SET_PORT    GPIOA_BSRR
#define NSS_RESET_PORT  GPIOA_BRR
#define IO0_GPIO        GPIO(IO0_PIN)
#define IO1_GPIO        GPIO(IO1_PIN)
#define IO2_GPIO        GPIO(IO2_PIN)
#define IO3_GPIO        GPIO(IO3_PIN)
#define NSS_GPIO        GPIO(NSS_PIN)
#define SCK_GPIO        GPIO(SCK_PIN)
#define WP_GPIO         IO2_GPIO
#define HOLD_GPIO       IO3_GPIO





// command system of windbond flash
#define WRITE_ENABLE            0x06
#define VOLATILE_SR_WRITE_EN    0x50
#define WRITE_DISABLE           0x04
#define READ_STATUS_REGISTER1   0x05
#define READ_STATUS_REGISTER2   0x35
#define WRITE_STATUS_REGISTER   0x01
#define PAGE_PROGRAM            0x02
#define SECTOR_ERASE            0x20
#define BLOCK_ERASE32K          0x52
#define BLOCK_ERASE64K          0xd8
#define CHIP_ERASE              0xc7
#define ERASE_SUSPEND           0x75
#define ERASE_RESUME            0x7a
#define POWER_DOWN              0xb9
#define READ_DATA               0x03
#define FAST_READ               0x0b
#define RELEASE_POWERDOWN       0xab
#define MANUFACTER              0x90
#define JEDEC_ID                0x9f
#define READ_UNIQUE_ID          0x4b
#define READ_SFDP_REGISTER      0x5a
#define ERASE_SECURITY_REGS     0x44
#define PROGRAM_SECURITY_REGS   0x42
#define READ_SECURITY_REGS      0x48
#define ENABLE_QPI              0x38
#define DISABLE_QPI             0xff
#define ENABLE_RESET            0x66
#define RESET                   0x99

// Status register 1 control bits
// Status register protect 0
#define W25Q64_SRP0 0x80
// Sector protect
#define W25Q64_SEC  0x40
// Top/bottom protect
#define W25Q64_TB   0x20
// Block protect_bits
#define W25Q64_BP0  0x04
#define W25Q64_BP1  0x08
#define W25Q64_BP2  0x10
// Write enable latch
#define W25Q64_WEL  0x02
// Erase/write in progress
#define W25Q64_BUSY 0x01
// Status register 2 control bits
// Suspend status
#define W25Q64_SUS  0x80
// Complement protect
#define W25Q64_CMP  0x40
// Security register lock bits
#define W25Q64_LB1  0x08
#define W25Q64_LB2  0x10
#define W25Q64_LB3  0x20
// Quad enable
#define W25Q64_QE   0x02
// Status register protect 1
#define W25Q64_SRP1 0x01

void spiFlashInit(void);
int spiFlashReadPage(uint32_t address, int size);
int spiFlashReadAllOld(void);
int spiFlashReadAll(void);
int spiFlashWritePage(uint32_t address, int size);
int spiFlashWaitForBusy(void);
int spiFlashDisableWriteProtect(void);
int spiFlashErase(void);
int spiFlashErasePage(uint32_t address);
uint16_t spiFlashReadStatus(void);
uint64_t spiFlashReadId(void);
void spiFlashDisableQpi(void);
void spiFlashWriteEnable(void);



#endif
