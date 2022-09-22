/*
 * STM32 flasher. Call init, enjoy.
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

#include "../lib/STM32F103_CMSIS/stm32f103.h"
#include "usb_core.h"
#include "gazelle_desc.h"
#include "flash.h"
#include "gazelle.h"

volatile int confSize;

void gazelleInit()
{
    flashInit();
    confSize = gazelleDesc.confTotalSize;
    usbCoreInit(&gazelleDesc);
}

// all usb events handled in interrupt
void usb_lp_can_rx_Handler()
{
    usbCore();
}
