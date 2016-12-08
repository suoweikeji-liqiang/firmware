/* Copyright (c) 2015-2016 Ivan Grokhotkov. All rights reserved.
 * This file is part of eboot bootloader.
 *
 * Redistribution and use is permitted according to the conditions of the
 * 3-clause BSD license to be found in the LICENSE file.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "flash.h"
#include "eboot_command.h"

#define SWRST do { (*((volatile uint32_t*) 0x60000700)) |= 0x80000000; } while(0);

extern void ets_wdt_enable(void);
extern void ets_wdt_disable(void);

//没什么具体含义 只是为了编译-bs .data 能通过。
uint8_t tempData[16] = {0x34, 0x18, 0x38, 0x14, 0x3C, 0x40, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x04, 0x08, 0x0C, 0x10};

/*从flash中导入应用*/
int load_app_from_flash_raw(const uint32_t flash_addr)
{
    image_header_t image_header;
    uint32_t pos = flash_addr + APP_START_OFFSET;

    if (SPIRead(pos, &image_header, sizeof(image_header))){
        return 1;
    }
    pos += sizeof(image_header);

    for (uint32_t section_index = 0;
        section_index < image_header.num_segments;
        ++section_index){
        section_header_t section_header = {0};
        if (SPIRead(pos, &section_header, sizeof(section_header))){
            return 2;
        }
        pos += sizeof(section_header);

        const uint32_t address = section_header.address;

        bool load = false;

        if (address < 0x40000000){
            load = true;
        }

        if (address >= 0x40100000 && address < 0x40108000){
            load = true;
        }

        if (address >= 0x60000000){
            load = true;
        }

        if (!load){
            pos += section_header.size;
            continue;
        }

        if (SPIRead(pos, (void*)address, section_header.size)){
            return 3;
        }

        pos += section_header.size;
    }
    register uint32_t sp asm("a1") = 0x3ffffff0;
    register uint32_t pc asm("a3") = image_header.entry;
    __asm__  __volatile__ ("jx a3");

    return 0;
}

/*从flash起始地址复制至目标地址*/
int copy_raw(const uint32_t src_addr,
             const uint32_t dst_addr,
             const uint32_t size)
{
    // require regions to be aligned
    if (src_addr & 0xfff != 0 ||
        dst_addr & 0xfff != 0){
        return 1;
    }

    const uint32_t buffer_size = FLASH_SECTOR_SIZE;
    uint8_t buffer[buffer_size];
    uint32_t left = ((size+buffer_size-1) & ~(buffer_size-1));
    uint32_t saddr = src_addr;
    uint32_t daddr = dst_addr;

    while (left){
        if (SPIEraseSector(daddr/buffer_size)){
            return 2;
        }
        if (SPIRead(saddr, buffer, buffer_size)){
            return 3;
        }
        if (SPIWrite(daddr, buffer, buffer_size)){
            return 4;
        }
        saddr += buffer_size;
        daddr += buffer_size;
        left  -= buffer_size;
    }

    return 0;
}

void main()
{
    int res = 9, count=3;
    struct eboot_command cmd;

    eboot_command_read(&cmd);
    if (cmd.action == ACTION_COPY_RAW){
        ets_putc('c'); ets_putc('p'); ets_putc(':');
        if(cmd.esp8266_app_size && cmd.default_stm32_app_size){
            ets_wdt_disable();
            while(count--)
            {
                //wifi update
                res = copy_raw(cmd.esp8266_app_addr[0], cmd.esp8266_app_addr[1], cmd.esp8266_app_size);
                //default app update
                if (res == 0){
                    res = copy_raw(cmd.default_stm32_app_addr[0], cmd.default_stm32_app_addr[1], cmd.default_stm32_app_size);
                }
                if(res == 0){
                    break;
                }
            }
            ets_wdt_enable();
        }
        ets_putc('0'+res); ets_putc('\n');
        cmd.action = ACTION_LOAD_APP;
        cmd.esp8266_app_size = 0;
        cmd.online_stm32_app_size = 0;
        eboot_command_write(&cmd);
    }

    cmd.action = ACTION_LOAD_APP;
    if (cmd.action == ACTION_LOAD_APP){
        ets_putc('l'); ets_putc('d'); ets_putc('\n');
        res = load_app_from_flash_raw(0);
        //we will get to this only on load fail
        ets_putc('e'); ets_putc(':'); ets_putc('0'+res); ets_putc('\n');
    }

    if (res){
        SWRST;
    }

    while(true)
    {}
}
