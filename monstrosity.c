/*
 * Copyright 2013, winocm. <winocm@icloud.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 *   If you are going to use this software in any form that does not involve
 *   releasing the source to this project or improving it, let me know beforehand.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * THIS THING IS A PIECE OF /CRAP/. (it's been fixed in newer iboots...)
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define IMAGE_SIZE  0x2C000

#define DFU_IDENTIFIER  0x1227
#define DFU_VENDOR      0x05AC

#define DebugPrintf     printf

struct libusb_device_handle* usb_subsystem_init(struct libusb_context* context,
                                                int devid)
{
    struct libusb_device            **device_list;
    struct libusb_device_handle     *handle = NULL;
    int devicecount;
    int i;

    devicecount = libusb_get_device_list(context, &device_list);

    for(i = 0; i < devicecount; i++) {
        struct libusb_device*               device = device_list[i];
        struct libusb_device_descriptor     desc;

        libusb_get_device_descriptor(device, &desc);

        if(desc.idVendor == DFU_VENDOR && desc.idProduct == devid) {
            libusb_open(device, &handle);
            break;
        }
    }

    libusb_free_device_list(device_list, 1);

    return handle;
}

void usb_close(struct libusb_device_handle* handle)
{
    if(handle) {
        libusb_close(handle);
    }
    return;
}

void usb_deinitialize(void)
{
    libusb_exit(NULL);
}

typedef struct _dfu_status {
    uint8_t     status[6];
} dfu_status_t;

void usb_dfu_get_status(struct libusb_device_handle *handle,
                        dfu_status_t* status)
{
    int bytes;
    
    if(!status)
        return;

    memset(status, 0, sizeof(dfu_status_t));

    bytes = libusb_control_transfer(handle, 0xA1, 3, 0, 0, status->status, 6,
                                    100);

    return;
}

void usb_dfu_notify_finished(struct libusb_device_handle *handle)
{
    int i;

    libusb_control_transfer(handle, 0x21, 1, 0, 0, 0, 0, 100);

    for(i = 0; i < 3; i ++) {
        dfu_status_t status;
        usb_dfu_get_status(handle, &status);
    }

    libusb_reset_device(handle);

    return;
}

int main(int argc, char* argv[])
{
    struct libusb_context*          context;
    struct libusb_device_handle     *handle = NULL;

    /*! Initialize USB. */
    libusb_init(&context);
    libusb_set_debug(NULL, 3);
    handle = usb_subsystem_init(context, DFU_IDENTIFIER);
    if(!handle) {
        abort();
    }

    /*! Send things */
    libusb_control_transfer(handle, 0x21, 4, 0, 0, data, 64, 1000);

    usb_dfu_notify_finished(handle);
    usb_dfu_notify_finished(handle);

    usb_close(handle);
    usb_deinitialize();

    return 0;

}
