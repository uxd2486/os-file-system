#include "ahci.h"
#include "pci.h"
#include "common.h"
#include "klib.h"
#include "cio.h"
#include "x86pic.h"
#include "support.h"
#include "kmem.h"

static hbaMem_t* _abar;
static hbaPort_t* _portsList[32];
static uint8_t _portsAvail;
static hddDeviceList_t _hddDevs;


// Find a free command list slot
static int find_cmdslot(hbaPort_t *port)
{
   // If not set in SACT and CI, the slot is free
   uint32_t slots = (port->sact | port->ci);
   for (int i=0; i<32; i++)
   {
      if ((slots&1) == 0)
         return i;
      slots >>= 1;
   }
   __cio_printf("Cannot find free command list entry\n");
   return -1;
}


static bool_t get_drive_info(hbaPort_t* port, void* buf)
{
   port->is = (uint32_t) -1;     // Clear pending interrupt bits
   int spin = 0; // Spin lock timeout counter
   int slot = find_cmdslot(port);
   if (slot == -1)
      return false;

   hbaCmdHeader_t *cmdheader = (hbaCmdHeader_t*)port->clb;
   cmdheader += slot;
   cmdheader->cfl = sizeof(fisRegH2d_t)/sizeof(uint32_t);   // Command FIS size
   cmdheader->w = 0;    // Read from device
   cmdheader->prdtl = (uint16_t) 1;   // PRDT entries count
 
   hbaCmdTbl_t *cmdtbl = (hbaCmdTbl_t*)(cmdheader->ctba);
   __memset(cmdtbl, 0, sizeof(hbaCmdTbl_t) +
      (cmdheader->prdtl-1)*sizeof(hbaPrdtEntry_t));

   // Last entry
   cmdtbl->prdt_entry[0].dba = (uint32_t) buf;
   cmdtbl->prdt_entry[0].dbc = 511; // 512 bytes per sector
   cmdtbl->prdt_entry[0].i = 1;
 
   // Setup command
   fisRegH2d_t *cmdfis = (fisRegH2d_t*)(&cmdtbl->cfis);
 
   cmdfis->fis_type = fis_type_reg_h2d;
   cmdfis->c = 1; // Command
   cmdfis->command = ATA_CMD_IDENTIFY;
   cmdfis->device = 0;  // Master device
 
   // The below loop waits until the port is no longer busy before issuing a new command
   while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
   {
      spin++;
   }
   if (spin == 1000000)
   {
      __cio_printf("\nPort is hung");
      //return false;
   }

   port->ci = 1<<slot;  // Issue command
 
   // Wait for completion
   while (1)
   {
      // In some longer duration writes, it may be helpful to spin on the DPS bit 
      // in the PxIS port field as well (1 << 5)
      if ((port->ci & (1<<slot)) == 0) 
         break;
      if (port->is & HBA_PxIS_TFES) // Task file error
      {
         __cio_printf("\nRead disk error");
         return false;
      }
   }
 
   // Check again
   if (port->is & HBA_PxIS_TFES)
   {
      __cio_printf("\nRead disk error");
      return false;
   }
 
   return true;
}

static bool_t ahci_write(hbaPort_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
   port->is = (uint32_t) -1;     // Clear pending interrupt bits
   int spin = 0; // Spin lock timeout counter
   int slot = find_cmdslot(port);
   if (slot == -1)
      return false;
 
   hbaCmdHeader_t *cmdheader = (hbaCmdHeader_t*)port->clb;
   cmdheader += slot;
   cmdheader->cfl = sizeof(fisRegH2d_t)/sizeof(uint32_t);   // Command FIS size
   cmdheader->w = 1;    // Write to device
   cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;   // PRDT entries count
 
   hbaCmdTbl_t *cmdtbl = (hbaCmdTbl_t*)(cmdheader->ctba);
   __memset(cmdtbl, 0, sizeof(hbaCmdTbl_t) +
      (cmdheader->prdtl-1)*sizeof(hbaPrdtEntry_t));
 
   // 8K bytes (16 sectors) per PRDT
   int i = 0;
   for (; i<cmdheader->prdtl-1; i++)
   {
      cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
      cmdtbl->prdt_entry[i].dbc = 8*1024-1;  // 8K bytes (this value should always be set to 1 less than the actual value)
      cmdtbl->prdt_entry[i].i = 1;
      buf += 4*1024; // 4K words
      count -= 16;   // 16 sectors
   }
   // Last entry
   cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
   cmdtbl->prdt_entry[i].dbc = (count<<9)-1; // 512 bytes per sector
   cmdtbl->prdt_entry[i].i = 1;
 
   // Setup command
   fisRegH2d_t *cmdfis = (fisRegH2d_t*)(&cmdtbl->cfis);
 
   cmdfis->fis_type = fis_type_reg_h2d;
   cmdfis->c = 1; // Command
   cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
   cmdfis->lba0 = (uint8_t)startl;
   cmdfis->lba1 = (uint8_t)(startl>>8);
   cmdfis->lba2 = (uint8_t)(startl>>16);
   cmdfis->device = 1<<6;  // LBA mode
 
   cmdfis->lba3 = (uint8_t)(startl>>24);
   cmdfis->lba4 = (uint8_t)starth;
   cmdfis->lba5 = (uint8_t)(starth>>8);
 
   cmdfis->countl = count & 0xFF;
   cmdfis->counth = (count >> 8) & 0xFF;
 
   // The below loop waits until the port is no longer busy before issuing a new command
   while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
   {
      spin++;
   }
   if (spin == 1000000)
   {
      __cio_printf("\nPort is hung");
      //return false;
   }
 
   port->ci = 1<<slot;  // Issue command
 
   // Wait for completion
   while (1)
   {
      // In some longer duration writes, it may be helpful to spin on the DPS bit 
      // in the PxIS port field as well (1 << 5)
      if ((port->ci & (1<<slot)) == 0) 
         break;
      if (port->is & HBA_PxIS_TFES) // Task file error
      {
         __cio_printf("\nRead disk error");
         return false;
      }
   }
 
   // Check again
   if (port->is & HBA_PxIS_TFES)
   {
      __cio_printf("\nRead disk error");
      return false;
   }
 
   return true;
}

static bool_t ahci_read(hbaPort_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
   port->is = (uint32_t) -1;     // Clear pending interrupt bits
   int spin = 0; // Spin lock timeout counter
   int slot = find_cmdslot(port);
   if (slot == -1)
      return false;
 
   hbaCmdHeader_t *cmdheader = (hbaCmdHeader_t*)port->clb;
   cmdheader += slot;
   cmdheader->cfl = sizeof(fisRegH2d_t)/sizeof(uint32_t);   // Command FIS size
   cmdheader->w = 0;    // Read from device
   cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;   // PRDT entries count
 
   hbaCmdTbl_t *cmdtbl = (hbaCmdTbl_t*)(cmdheader->ctba);
   __memset(cmdtbl, 0, sizeof(hbaCmdTbl_t) +
      (cmdheader->prdtl-1)*sizeof(hbaPrdtEntry_t));
 
   // 8K bytes (16 sectors) per PRDT
   int i = 0;
   for (; i<cmdheader->prdtl-1; i++)
   {
      cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
      cmdtbl->prdt_entry[i].dbc = 8*1024-1;  // 8K bytes (this value should always be set to 1 less than the actual value)
      cmdtbl->prdt_entry[i].i = 1;
      buf += 4*1024; // 4K words
      count -= 16;   // 16 sectors
   }
   // Last entry
   cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
   cmdtbl->prdt_entry[i].dbc = (count<<9)-1; // 512 bytes per sector
   cmdtbl->prdt_entry[i].i = 1;
 
   // Setup command
   fisRegH2d_t *cmdfis = (fisRegH2d_t*)(&cmdtbl->cfis);
 
   cmdfis->fis_type = fis_type_reg_h2d;
   cmdfis->c = 1; // Command
   cmdfis->command = ATA_CMD_READ_DMA_EX;
 
   cmdfis->lba0 = (uint8_t)startl;
   cmdfis->lba1 = (uint8_t)(startl>>8);
   cmdfis->lba2 = (uint8_t)(startl>>16);
   cmdfis->device = 1<<6;  // LBA mode
 
   cmdfis->lba3 = (uint8_t)(startl>>24);
   cmdfis->lba4 = (uint8_t)starth;
   cmdfis->lba5 = (uint8_t)(starth>>8);
 
   cmdfis->countl = count & 0xFF;
   cmdfis->counth = (count >> 8) & 0xFF;
 
   // The below loop waits until the port is no longer busy before issuing a new command
   while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
   {
      spin++;
   }
   if (spin == 1000000)
   {
      __cio_printf("\nPort is hung");
      //return false;
   }
 
   port->ci = 1<<slot;  // Issue command
 
   // Wait for completion
   while (1)
   {
      // In some longer duration reads, it may be helpful to spin on the DPS bit 
      // in the PxIS port field as well (1 << 5)
      if ((port->ci & (1<<slot)) == 0) 
         break;
      if (port->is & HBA_PxIS_TFES) // Task file error
      {
         __cio_printf("\nRead disk error");
         return false;
      }
   }
 
   // Check again
   if (port->is & HBA_PxIS_TFES)
   {
      __cio_printf("\nRead disk error");
      return false;
   }
 
   return true;
}

// Start command engine
static void start_cmd(hbaPort_t *port)
{
   // Wait until CR (bit15) is cleared
   while (port->cmd & HBA_PxCMD_CR)
      ;
 
   // Set FRE (bit4) and ST (bit0)
   port->cmd |= HBA_PxCMD_FRE;
   //port->cmd |= HBA_PxCMD_CLO;
   port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
static void stop_cmd(hbaPort_t *port)
{
   // Clear ST (bit0)
   port->cmd &= ~HBA_PxCMD_ST;
 
   // Clear FRE (bit4)
   port->cmd &= ~HBA_PxCMD_FRE;
 
   // Wait until FR (bit14), CR (bit15) are cleared
   while(1)
   {
      if (port->cmd & HBA_PxCMD_FR)
         continue;
      if (port->cmd & HBA_PxCMD_CR)
         continue;
      break;
   }
 
}

static void port_rebase(hbaPort_t *port, int portno)
{
   stop_cmd(port);   // Stop command engine
 
   // Command list offset: 1K*portno
   // Command list entry size = 32
   // Command list entry maxim count = 32
   // Command list maxim size = 32*32 = 1K per port
   port->clb = AHCI_BASE + (portno<<10);
   port->clbu = 0;
   __memset((void*)(port->clb), 0, 1024);
 
   // FIS offset: 32K+256*portno
   // FIS entry size = 256 bytes per port
   port->fb = AHCI_BASE + (32<<10) + (portno<<8);
   port->fbu = 0;
   __memset((void*)(port->fb), 0, 256);
 
   // Command table offset: 40K + 8K*portno
   // Command table size = 256*32 = 8K per port
   hbaCmdHeader_t *cmdheader = (hbaCmdHeader_t*)(port->clb);
   for (int i=0; i<32; i++)
   {
      cmdheader[i].prdtl = 8; // 8 prdt entries per command table
               // 256 bytes per command table, 64+16+48+16*8
      // Command table offset: 40K + 8K*portno + cmdheader_index*256
      cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
      cmdheader[i].ctbau = 0;
      __memset((void*)cmdheader[i].ctba, 0, 256);
   }
 
   start_cmd(port);  // Start command engine
}

static void _ahci_isr(int vector, int code)
{
   //__cio_printf("\nIN AHCI ISR\n");
   uint32_t pi = _abar->pi;
   uint32_t is = _abar->is;
   for(uint8_t i = 0; i < 32; i++){
      if(pi & 1 && is & 1){
         _abar->pi |= 1 << i;
         //uint32_t port_is = _abar->ports[i].is;
         //Never Here
         //check port
         //writeback
         //if error bit set, reset port/retry commands as necessary.
         //Compare issued commands register to the commands you have recorded as issuing. For any bits where a command was issued but is no longer running, this means that the command has completed.
      }
      pi >>= 1;
      is >>= 1;
   }
   __outb( PIC_PRI_CMD_PORT, PIC_EOI );
}

static int check_type(hbaPort_t *port)
{
   uint32_t ssts = port->ssts;
 
   uint8_t ipm = (ssts >> 8) & 0x0F;
   uint8_t det = ssts & 0x0F;
 
   if (det != hbaPort_t_DET_PRESENT) // Check drive status
      return AHCI_DEV_NULL;
   if (ipm != hbaPort_t_IPM_ACTIVE)
      return AHCI_DEV_NULL;
 
   switch (port->sig)
   {
   case SATA_SIG_ATAPI:
      return AHCI_DEV_SATAPI;
   case SATA_SIG_SEMB:
      return AHCI_DEV_SEMB;
   case SATA_SIG_PM:
      return AHCI_DEV_PM;
   default:
      return AHCI_DEV_SATA;
   }
}


hddDeviceList_t _get_device_list()
{
   return _hddDevs;
}

bool_t _write_disk(hddDevice_t device, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
   uint64_t absaddr = ((uint64_t)starth << 32) | startl;
   if((absaddr + count) > device.sector_count){
      return false;
   }
   return ahci_write(device.port, startl, starth, count, buf);
}

bool_t _read_disk(hddDevice_t device, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{  
   uint64_t absaddr = ((uint64_t)starth << 32) | startl;
   if((absaddr + count) > device.sector_count){
      return false;
   }
   return ahci_read(device.port, startl, starth, count, buf);
}


void _ahci_init()
{
   __cio_printf(" AHCI:");
   //find device
   _enumerate_pci_devices();
   _abar = (hbaMem_t*)_get_controller().address;

   //check if device supports legacy modes
   if((_abar->cap & 0x00040000) == 0){
      //set ahci enable
      //__cio_printf("\nAHCI First Enable");
      _abar->ghc |= 0x80000000;
   }

   //Check if need to perform handoff
   if(_abar->cap2 & 0x0001){
      //__cio_printf("\nBIOS/OS Handoff");
      //handoff
      //reset controller
      __cio_printf(" Fail");
      return;
   }
   else{
      //__cio_printf("\nNo BIOS/OS Handoff");
   }

   //Install isr and given line
   __install_isr(PIC_PRI_CMD_PORT | _get_controller().intLine, _ahci_isr);

   for(uint8_t i = 0; i < 32; i++){
      _abar->ports[i].cmd &= 0xFFFFFFEE;
   }

   _abar->ghc |= 0x01;

   while(_abar->ghc & 0x01){
      ;
   }

   if((_abar->cap & 0x00040000) == 0){
      //set ahci enable
      //__cio_printf("\nAHCI Second Enable");
      _abar->ghc |= 0x80000000;
   }

   int o = 0;
   while(o < 700000){
      o++;
   }

   //enable interrupts
   _abar->ghc |= 0x00000002;

   //get ports and numbers
   __memset(_portsList, 32 * 4, 0);
   _portsAvail = 0;
   hbaPort_t* port = _abar->ports;
   uint32_t pi = _abar->pi;
   for(uint8_t i = 0; i < 32; i++){
      if(pi & 1){
         uint8_t deviceType = check_type(&port[i]);
         if(deviceType == AHCI_DEV_SATA){
            _portsList[_portsAvail++] = &port[i];
            //__cio_printf("\nFound Sata Device at Port: %d", i);
         }
      }
      pi >>= 1;
   }
   for(uint8_t i = 0; i < _portsAvail; i++){
      port_rebase(_portsList[i], 0);
      int count = 0;
      while((_portsList[i]->ssts & 0xF) != 3 && count < 1000000){
         count++;
      }
      if(count == 1000000){
         //__cio_printf("\nFail:");
         continue;
      }
      _portsList[i]->serr = 0xFFFFFFFF;
      _hddDevs.devices[count].port = _portsList[i];
      _hddDevs.count++;
   }

   identifyDeviceData_t* tempIDData = (identifyDeviceData_t*)_km_page_alloc(1);
   __memset(tempIDData, 4000, 0);

   for(int i = 0; i < _hddDevs.count; i++){
      get_drive_info(_hddDevs.devices[i].port, tempIDData);

      uint32_t secl = 0;
      uint32_t sech = 0;
      uint8_t shift = 0;

      if(tempIDData->CommandSetSupport.BigLba) {
         secl = (uint32_t)tempIDData->Max48BitLBA[0] + ((uint32_t)tempIDData->Max48BitLBA[1] << 16);
         sech = (uint32_t)tempIDData->Max48BitLBA[2] + ((uint32_t)tempIDData->Max48BitLBA[3] << 16);
         shift = 32;
      }
      else {
         secl = (uint32_t)tempIDData->UserAddressableSectors[0] + ((uint32_t)tempIDData->UserAddressableSectors[1] << 16);
         shift = 0;
      }

      _hddDevs.devices[i].sector_count = (uint64_t)secl + ((uint64_t)sech << shift) - 1;

      if(tempIDData->PhysicalLogicalSectorSize.Reserved1 == 0) {
         _hddDevs.devices[i].sector_size = 512;
      }
      else {
         if(tempIDData->PhysicalLogicalSectorSize.LogicalSectorLongerThan256Words) {
            _hddDevs.devices[i].sector_size = (uint32_t)tempIDData->WordsPerLogicalSector[0] + ((uint32_t)tempIDData->WordsPerLogicalSector[1] << 16);
         }
         else {
            _hddDevs.devices[i].sector_size = 256;
         }
      }
      _hddDevs.devices[i].total_bytes = _hddDevs.devices[i].sector_count * _hddDevs.devices[i].sector_size;
   }
   _km_page_free(tempIDData);

   __cio_printf("\n%d, %d", _hddDevs.devices[0].total_bytes);

   //DEMO CODE
   //CAREFULL ON REAL HARDWARE. WILL OVERWRITE DISK.
   //Reads the drive (sector 5) then writes to the drive in the same place.
   /*void* buffer = _km_page_alloc(1);
   __memset(buffer, 512, 0xFF);
   __cio_printf("\nClear buffer: %08x", *(uint32_t*)buffer);
   _read_disk(_hddDevs.devices[0], 5, 0, 1, buffer);
   __cio_printf("\nRead drive: %08x", *(uint32_t*)buffer);
   __memset(buffer, 512, 0xFF);
   __cio_printf("\nClear buffer: %08x", *(uint32_t*)buffer);
   _write_disk(_hddDevs.devices[0], 5, 0, 1, buffer);
   __cio_printf("\nWrote to drive");
   _read_disk(_hddDevs.devices[0], 5, 0, 1, buffer);
   __cio_printf("\nRead drive: %08x", *(uint32_t*)buffer);
   _km_page_free(buffer);*/
   //END DEMO CODE

   
   __cio_printf(" done");
}

