#ifndef H_I2C_REGS
#define H_I2C_REGS
/*
 * Part of Belkin STM32 HAL, I2C register definitions of STM32F1xx MCUs.
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

#include "memorymap.h"

/* I2C Control register 1 */
#define I2C1_CR1   MMIO32(I2C1_BASE + 0x00)
#define I2C2_CR1   MMIO32(I2C2_BASE + 0x00)
// Software reset
#define SWRST       0x8000
// SMBus alert
#define ALERT       0x2000
// Packet error checking
#define PEC         0x1000
// Acknowledge/PEC Position (for data reception)
#define POS         0x0800
// Acknowledge enable
#define IACK        0x0400
// Stop generation
#define STOP        0x0200
// Start generation
#define START       0x0100
// Clock stretching disable (Slave mode)
#define NOSTRETCH   0x0080
// General call enable
#define ENGC        0x0040
// PEC enable
#define ENPEC       0x0020
// ARP enable
#define ENARP       0x0010
// SMBus type
#define SMBTYPE     0x0008
// SMBus mode
#define SMBUS       0x0002
// Peripheral enable
#define PE          0x0001

/* I2C Control register 2 */
#define I2C1_CR2   MMIO32(I2C1_BASE + 0x04)
#define I2C2_CR2   MMIO32(I2C2_BASE + 0x04)
// DMA last transfer
#define LAST        0x1000
// DMA requests enable
#define IDMAEN      0x0800
// Buffer interrupt enable
#define ITBUFEN     0x0400
// Event interrupt enable
#define ITEVTEN     0x0200
// Error interrupt enable
#define ITERREN     0x0100
// Peripheral clock frequency
#define FREQ_MSK    0x003f
#define FREQ_MIN    0x0002
#define FREQ_MAX    0x0032

/* I2C Own address register 1 */
#define I2C1_OAR1  MMIO32(I2C1_BASE + 0x08)
#define I2C2_OAR1  MMIO32(I2C2_BASE + 0x08)
// Addressing mode (slave mode)
#define ADDMODE         0x8000
// Interface address
#define ADD_7BIT_OFFS   1
#define ADD_7BIT_MSK    0x00fe
#define ADD_10BIT_MSK   0x03ff

/* I2C Own address register 2 */
#define I2C1_OAR2  MMIO32(I2C1_BASE + 0x0c)
#define I2C2_OAR2  MMIO32(I2C2_BASE + 0x0c)
// Interface address
#define ADD2_OFFS   1
#define ADD2_MSK    0x00fe
// Dual addressing mode enable
#define ENDUAL      0x0001

/* I2C Data register */
#define I2C1_DR    MMIO32(I2C1_BASE + 0x10)
#define I2C2_DR    MMIO32(I2C2_BASE + 0x10)
// 8-bit data register DR

/* I2C Status register 1 */
#define I2C1_SR1   MMIO32(I2C1_BASE + 0x14)
#define I2C2_SR1   MMIO32(I2C2_BASE + 0x14)
// SMBus alert
#define SMBALERT    0x8000
// Timeout or Tlow error
#define TIMEOUT     0x4000
// PEC Error in reception
#define PECERR      0x1000
// Overrun/Underrun
#define IOVR         0x0800
// Acknowledge failure
#define AF          0x0400
// Arbitration lost (master mode)
#define ARLO        0x0200
// Bus error
#define IBERR       0x0100
// Data register empty (transmitters)
#define ITXE        0x0080
// Data register not empty (receivers)
#define IRXNE       0x0040
// Stop detection (slave mode)
#define STOPF       0x0010
// 10-bit header sent (Master mode)
#define ADD10       0x0008
// Byte transfer finished
#define BTF         0x0004
// Address sent (master mode)/matched (slave mode)
#define ADDR        0x0002
// Start bit (Master mode)
#define SB          0x0001

/* I2C Status register 2 */
#define I2C1_SR2   MMIO32(I2C1_BASE + 0x18)
#define I2C2_SR2   MMIO32(I2C2_BASE + 0x18)
// Packet error checking register
#define PEC_OFFS    8
// Dual flag (Slave mode)
#define DUALF       0x80
// SMBus host header (Slave mode)
#define SMBHOST     0x40
// SMBus device default address (Slave mode)
#define SMBDEFAULT  0x20
// General call address (Slave mode)
#define GENCALL     0x10
// Transmitter/receiver
#define TRA         0x04
// Bus busy
#define BUSY        0x02
// Master/slave
#define MSL         0x01

/* I2C Clock control register */
#define I2C1_CCR   MMIO32(I2C1_BASE + 0x1c)
#define I2C2_CCR   MMIO32(I2C2_BASE + 0x1c)
// I2C master mode selection
#define F_S     0x8000
// Fm mode duty cycle
#define DUTY    0x4000
// CCR[11:0]: Clock control register in Fm/Sm mode (Master mode)
#define I2C400K SYSTEM_CLOCK/2/400/25
#define I2C100K SYSTEM_CLOCK/2/100/2

/* I2C TRISE register */
#define I2C1_TRISE MMIO32(I2C1_BASE + 0x20)
#define I2C2_TRISE MMIO32(I2C2_BASE + 0x20)
// Maximum rise time in Fm/Sm mode (Master mode)
#define TRISE_NS(t) (((SYSTEM_CLOCK/2)*t)/1000000000)+1

#endif
