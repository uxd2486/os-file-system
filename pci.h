#ifndef PCI_H_
#define PCI_H_

#include "common.h"

typedef struct tagahciController {
   uint8_t bus;
   uint8_t device;
   uint8_t function;
   uint32_t address;
   uint8_t intPIN;
   uint8_t intLine;
} ahciController_t;

void _enumerate_pci_devices( void );

ahciController_t _get_controller( void );

#endif
