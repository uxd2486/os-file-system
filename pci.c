#include "pci.h"
#include "common.h"
#include "cio.h"
#include "klib.h"


static ahciController_t _controller;


static uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    __outl(0xCF8, address);
    /* read in the data */
    tmp = (uint16_t)((__inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

static uint16_t getVendorID(uint8_t bus, uint8_t slot, uint8_t function) {
    return pciConfigReadWord(bus,slot,function,0);
}

static uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t function) {
    return pciConfigReadWord(bus,slot,function,0x0E) & 0x00FF;
}

static void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
    if((pciConfigReadWord(bus, device, function, 0x0a) == 0x0106) & 
      (pciConfigReadWord(bus, device, function, 0x08) >> 8 == 0x01)) {
        _controller.bus = bus;
        _controller.device = device;
        _controller.function = function;
        _controller.intPIN = pciConfigReadWord(bus, device, function, 0x3D);
        _controller.intLine = pciConfigReadWord(bus, device, function, 0x3C);
        uint16_t addressLo = pciConfigReadWord(bus, device, function, 0x24);
        uint16_t addressHi = pciConfigReadWord(bus, device, function, 0x26);
        _controller.address = (addressHi << 16) | addressLo;
    }
}

static void checkDevice(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    uint16_t vendorID = getVendorID(bus, device, function);
    if(vendorID == 0xFFFF) return;        // Device doesn't exist
    checkFunction(bus, device, function);
    uint8_t headerType = getHeaderType(bus, device, function);
    if((headerType & 0x80) != 0) {
        /* It is a multi-function device, so check remaining functions */
        for(function = 1; function < 8; function++) {
            if(getVendorID(bus, device, function) != 0xFFFF) {
                checkFunction(bus, device, function);
            }
        }
    }
}

void _enumerate_pci_devices(void) {
    uint16_t bus;
    uint8_t device;

    for(bus = 0; bus < 256; bus++) {
        for(device = 0; device < 32; device++) {
            checkDevice(bus, device);
        }
    }
}

ahciController_t _get_controller(void) {
    return _controller;
}
