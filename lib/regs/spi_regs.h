#ifndef H_SPI_REGS
#define H_SPI_REGS
/*
 * Part of Belkin STM32 HAL, SPI/I2S register definitions of STM32F1xx MCUs.
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

/* SPI control register 1 */
#define SPI1_CR1     MMIO32(SPI1_BASE + 0x00)
// Bidirectional data mode enable
#define BIDIMODE    0x8000
// Output enable in bidirectional mode
#define BIDIOE      0x4000
// Hardware CRC calculation enable
#define SCRCEN       0x2000
// CRC transfer next
#define CRCNEXT     0x1000
// Data frame format
#define DFF         0x0800
// Receive only
#define RXONLY      0x0400
// Software slave management
#define SSM         0x0200
// Internal slave select
#define SSI         0x0100
// Frame format
#define LSBFIRST    0x0080
// SPI enable
#define SPE         0x0040
// Baud rate control
#define BR_DIV2     0x0000
#define BR_DIV4     0x0008
#define BR_DIV8     0x0010
#define BR_DIV16    0x0018
#define BR_DIV32    0x0020
#define BR_DIV64    0x0028
#define BR_DIV128   0x0030
#define BR_DIV256   0x0038
// Master selection
#define MSTR        0x0004
// Clock polarity
#define CPOL        0x0002
// Clock phase
#define CPHA        0x0001

/* SPI control register 2 */
#define SPI1_CR2     MMIO32(SPI1_BASE + 0x04)
// Tx buffer empty interrupt enable
#define TXEIE   0x80
// RX buffer not empty interrupt enable
#define RXNEIE  0x40
// Error interrupt enable
#define ERRIE   0x20
// SS output enable
#define SSOE    0x04
// Tx buffer DMA enable
#define TXDMAEN 0x02
// Rx buffer DMA enable
#define RXDMAEN 0x01

/* SPI status register */
#define SPI1_SR      MMIO32(SPI1_BASE + 0x08)
// Busy flag
#define BSY         0x80
// Overrun flag
#define OVR         0x40
// Mode fault
#define MODF        0x20
// CRC error flag
#define CRCERR      0x10
// Underrun flag
#define UDR         0x08
// Channel side
#define CHSIDE      0x04
// Transmit buffer empty
#define TXE         0x02
// Receive buffer not empty
#define RXNE        0x01

/* SPI data register */
#define SPI1_DR      MMIO32(SPI1_BASE + 0x0c)

/* SPI CRC polynomial register (not used in I2S mode) */
#define SPI1_CRCPR   MMIO32(SPI1_BASE + 0x10)
/* SPI RX CRC register (not used in I2S mode) */
#define SPI1_RXCRCR  MMIO32(SPI1_BASE + 0x14)
/* SPI TX CRC register (not used in I2S mode) */
#define SPI1_TXCRCR  MMIO32(SPI1_BASE + 0x18)

/* SPI_I2S configuration register */
#define SPI1_I2SCFGR MMIO32(SPI1_BASE + 0x1c)
// I2S mode selection
#define I2SMOD  0x800
// I2S Enable
#define I2SE    0x400
// I2S configuration mode
#define I2SCFG_SLAVE_TX  0x000
#define I2SCFG_SLAVE_RX  0x100
#define I2SCFG_MASTER_TX 0x200
#define I2SCFG_MASTER_RX 0x300
// PCM frame synchronization
#define PCMSYNC 0x080
// I2S standard selection
#define I2SSTD_PHILIPS          0x000
#define I2SSTD_LEFT_JUSTIFIED   0x010
#define I2SSTD_RIGHT_JUSTIFIED  0x020
#define I2SSTD_PCM              0x030
// Steady state clock polarity
#define CKPOL   0x008
// Data length to be transferred
#define DATLEN_16BIT            0x000
#define DATLEN_24BIT            0x002
#define DATLET_32BIT            0x004
// Channel length (number of bits per audio channel)
#define CHLEN   0x001

/* SPI_I2S prescaler register */
#define SPI1_I2SPR   MMIO32(SPI1_BASE + 0x20)
// Master clock output enable
#define MCKOE   0x200
// Odd factor for the prescaler
#define ODD     0x100
// I2S Linear prescaler
#define I2SDIV_MASK 0x0ff


#define SPI2_CR1     MMIO32(SPI2_I2S_BASE + 0x00)
#define SPI3_CR1     MMIO32(SPI3_I2S_BASE + 0x00)
#define SPI2_CR2     MMIO32(SPI2_I2S_BASE + 0x04)
#define SPI3_CR2     MMIO32(SPI3_I2S_BASE + 0x04)
#define SPI2_SR      MMIO32(SPI2_I2S_BASE + 0x08)
#define SPI3_SR      MMIO32(SPI3_I2S_BASE + 0x08)
#define SPI2_DR      MMIO32(SPI2_I2S_BASE + 0x0c)
#define SPI3_DR      MMIO32(SPI3_I2S_BASE + 0x0c)
#define SPI2_CRCPR   MMIO32(SPI2_I2S_BASE + 0x10)
#define SPI3_CRCPR   MMIO32(SPI3_I2S_BASE + 0x10)
#define SPI2_RXCRCR  MMIO32(SPI2_I2S_BASE + 0x14)
#define SPI3_RXCRCR  MMIO32(SPI3_I2S_BASE + 0x14)
#define SPI2_TXCRCR  MMIO32(SPI2_I2S_BASE + 0x18)
#define SPI3_TXCRCR  MMIO32(SPI3_I2S_BASE + 0x18)
#define SPI2_I2SCFGR MMIO32(SPI2_I2S_BASE + 0x1c)
#define SPI3_I2SCFGR MMIO32(SPI3_I2S_BASE + 0x1c)
#define SPI2_I2SPR   MMIO32(SPI2_I2S_BASE + 0x20)
#define SPI3_I2SPR   MMIO32(SPI3_I2S_BASE + 0x20)

#endif
