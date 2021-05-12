#ifndef AHCI_H_
#define AHCI_H_

#include "common.h"
#include "pci.h"

#define AHCI_BASE       0x00400000  // 4 Megabyte offset

#define SATA_SIG_ATA    0x00000101  // SATA drive
#define SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB   0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM     0x96690101  // Port multiplier
 
#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SEMB   2
#define AHCI_DEV_PM     3
#define AHCI_DEV_SATAPI 4
 
#define hbaPort_t_IPM_ACTIVE     1
#define hbaPort_t_DET_PRESENT    3
 
#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_CLO   0x0008
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

#define HBA_PxIS_TFES   0x400000

#define ATA_DEV_BUSY    0x80
#define ATA_DEV_DRQ     0x08

#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX    0x35
#define ATA_CMD_IDENTIFY        0xEC


/* FIS */

typedef enum
{
    fis_type_reg_h2d        = 0x27, // Register FIS - host to device
    fis_type_reg_d2h        = 0x34, // Register FIS - device to host
    fis_type_dma_act        = 0x39, // DMA activate FIS - device to host
    fis_type_dma_setup      = 0x41, // DMA setup FIS - bidirectional
    fis_type_data           = 0x46, // Data FIS - bidirectional
    fis_type_bist           = 0x58, // BIST activate FIS - bidirectional
    fis_type_pio_setup      = 0x5F, // PIO setup FIS - device to host
    fis_type_dev_bits       = 0xA1, // Set device bits FIS - device to host
} fisType_t;

typedef struct tagfisRegH2d
{
    // DWORD 0
    uint8_t  fis_type;  // fis_type_reg_h2d
 
    uint8_t  pmport:4;  // Port multiplier
    uint8_t  rsv0:3;        // Reserved
    uint8_t  c:1;       // 1: Command, 0: Control
 
    uint8_t  command;   // Command register
    uint8_t  featurel;  // Feature register, 7:0
 
    // DWORD 1
    uint8_t  lba0;      // LBA low register, 7:0
    uint8_t  lba1;      // LBA mid register, 15:8
    uint8_t  lba2;      // LBA high register, 23:16
    uint8_t  device;        // Device register
 
    // DWORD 2
    uint8_t  lba3;      // LBA register, 31:24
    uint8_t  lba4;      // LBA register, 39:32
    uint8_t  lba5;      // LBA register, 47:40
    uint8_t  featureh;  // Feature register, 15:8
 
    // DWORD 3
    uint8_t  countl;        // Count register, 7:0
    uint8_t  counth;        // Count register, 15:8
    uint8_t  icc;       // Isochronous command completion
    uint8_t  control;   // Control register
 
    // DWORD 4
    uint8_t  rsv1[4];   // Reserved
} fisRegH2d_t;

typedef struct tagfisRegD2h
{
    // DWORD 0
    uint8_t  fis_type;    // fis_type_reg_d2h
 
    uint8_t  pmport:4;    // Port multiplier
    uint8_t  rsv0:2;      // Reserved
    uint8_t  i:1;         // Interrupt bit
    uint8_t  rsv1:1;      // Reserved
 
    uint8_t  status;      // Status register
    uint8_t  error;       // Error register
 
    // DWORD 1
    uint8_t  lba0;        // LBA low register, 7:0
    uint8_t  lba1;        // LBA mid register, 15:8
    uint8_t  lba2;        // LBA high register, 23:16
    uint8_t  device;      // Device register
 
    // DWORD 2
    uint8_t  lba3;        // LBA register, 31:24
    uint8_t  lba4;        // LBA register, 39:32
    uint8_t  lba5;        // LBA register, 47:40
    uint8_t  rsv2;        // Reserved
 
    // DWORD 3
    uint8_t  countl;      // Count register, 7:0
    uint8_t  counth;      // Count register, 15:8
    uint8_t  rsv3[2];     // Reserved
 
    // DWORD 4
    uint8_t  rsv4[4];     // Reserved
} fisRegD2h_t;

typedef struct tagfisData
{
    // DWORD 0
    uint8_t  fis_type;  // fis_type_data
 
    uint8_t  pmport:4;  // Port multiplier
    uint8_t  rsv0:4;        // Reserved
 
    uint8_t  rsv1[2];   // Reserved
 
    // DWORD 1 ~ N
    uint32_t data[1];   // Payload
} fisData_t;

typedef struct tagfisPioSetup
{
    // DWORD 0
    uint8_t  fis_type;  // fis_type_pio_setup
 
    uint8_t  pmport:4;  // Port multiplier
    uint8_t  rsv0:1;        // Reserved
    uint8_t  d:1;       // Data transfer direction, 1 - device to host
    uint8_t  i:1;       // Interrupt bit
    uint8_t  rsv1:1;
 
    uint8_t  status;        // Status register
    uint8_t  error;     // Error register
 
    // DWORD 1
    uint8_t  lba0;      // LBA low register, 7:0
    uint8_t  lba1;      // LBA mid register, 15:8
    uint8_t  lba2;      // LBA high register, 23:16
    uint8_t  device;        // Device register
 
    // DWORD 2
    uint8_t  lba3;      // LBA register, 31:24
    uint8_t  lba4;      // LBA register, 39:32
    uint8_t  lba5;      // LBA register, 47:40
    uint8_t  rsv2;      // Reserved
 
    // DWORD 3
    uint8_t  countl;        // Count register, 7:0
    uint8_t  counth;        // Count register, 15:8
    uint8_t  rsv3;      // Reserved
    uint8_t  e_status;  // New value of status register
 
    // DWORD 4
    uint16_t tc;        // Transfer count
    uint8_t  rsv4[2];   // Reserved
} fisPioSetup_t;

typedef struct tagfisDmaSetup
{
    // DWORD 0
    uint8_t  fis_type;  // fis_type_dma_setup
 
    uint8_t  pmport:4;  // Port multiplier
    uint8_t  rsv0:1;        // Reserved
    uint8_t  d:1;       // Data transfer direction, 1 - device to host
    uint8_t  i:1;       // Interrupt bit
    uint8_t  a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed
 
        uint8_t  rsved[2];       // Reserved
 
    //DWORD 1&2
 
        uint64_t DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.
 
        //DWORD 3
        uint32_t rsvd;           //More reserved
 
        //DWORD 4
        uint32_t DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0
 
        //DWORD 5
        uint32_t TransferCount;  //Number of bytes to transfer. Bit 0 must be 0
 
        //DWORD 6
        uint32_t resvd;          //Reserved
 
} fisDmaSetup_t;

/* FIS */


/* Access */

typedef volatile struct taghbaPort
{
    uint32_t clb;       // 0x00, command list base address, 1K-byte aligned
    uint32_t clbu;      // 0x04, command list base address upper 32 bits
    uint32_t fb;        // 0x08, FIS base address, 256-byte aligned
    uint32_t fbu;       // 0x0C, FIS base address upper 32 bits
    uint32_t is;        // 0x10, interrupt status
    uint32_t ie;        // 0x14, interrupt enable
    uint32_t cmd;       // 0x18, command and status
    uint32_t rsv0;      // 0x1C, Reserved
    uint32_t tfd;       // 0x20, task file data
    uint32_t sig;       // 0x24, signature
    uint32_t ssts;      // 0x28, SATA status (SCR0:SStatus)
    uint32_t sctl;      // 0x2C, SATA control (SCR2:SControl)
    uint32_t serr;      // 0x30, SATA error (SCR1:SError)
    uint32_t sact;      // 0x34, SATA active (SCR3:SActive)
    uint32_t ci;        // 0x38, command issue
    uint32_t sntf;      // 0x3C, SATA notification (SCR4:SNotification)
    uint32_t fbs;       // 0x40, FIS-based switch control
    uint32_t rsv1[11];  // 0x44 ~ 0x6F, Reserved
    uint32_t vendor[4]; // 0x70 ~ 0x7F, vendor specific
} hbaPort_t;

typedef volatile struct taghbaMem
{
    // 0x00 - 0x2B, Generic Host Control
    uint32_t cap;       // 0x00, Host capability
    uint32_t ghc;       // 0x04, Global host control
    uint32_t is;        // 0x08, Interrupt status
    uint32_t pi;        // 0x0C, Port implemented
    uint32_t vs;        // 0x10, Version
    uint32_t ccc_ctl;   // 0x14, Command completion coalescing control
    uint32_t ccc_pts;   // 0x18, Command completion coalescing ports
    uint32_t em_loc;        // 0x1C, Enclosure management location
    uint32_t em_ctl;        // 0x20, Enclosure management control
    uint32_t cap2;      // 0x24, Host capabilities extended
    uint32_t bohc;      // 0x28, BIOS/OS handoff control and status
 
    // 0x2C - 0x9F, Reserved
    uint8_t  rsv[0xA0-0x2C];
 
    // 0xA0 - 0xFF, Vendor specific registers
    uint8_t  vendor[0x100-0xA0];
 
    // 0x100 - 0x10FF, Port control registers
    hbaPort_t    ports[32];   // 1 ~ 32
} hbaMem_t;

/* Access */


/* Commands */

typedef volatile struct taghbaFis
{
    // 0x00
    fisDmaSetup_t   dsfis;      // DMA Setup FIS
    uint8_t         pad0[4];
 
    // 0x20
    fisPioSetup_t   psfis;      // PIO Setup FIS
    uint8_t         pad1[12];
 
    // 0x40
    fisRegD2h_t     rfis;       // Register – Device to Host FIS
    uint8_t         pad2[4];
 
    // 0x58
    uint8_t         sdbfis[0x08];// Set Device Bit FIS
 
    // 0x60
    uint8_t         ufis[64];
 
    // 0xA0
    uint8_t     rsv[0x100-0xA0];
} hbaFis_t;

typedef struct taghbaCmdHeader
{
    // DW0
    uint8_t  cfl:5;     // Command FIS length in DWORDS, 2 ~ 16
    uint8_t  a:1;       // ATAPI
    uint8_t  w:1;       // Write, 1: H2D, 0: D2H
    uint8_t  p:1;       // Prefetchable
 
    uint8_t  r:1;       // Reset
    uint8_t  b:1;       // BIST
    uint8_t  c:1;       // Clear busy upon R_OK
    uint8_t  rsv0:1;        // Reserved
    uint8_t  pmp:4;     // Port multiplier port
 
    uint16_t prdtl;     // Physical region descriptor table length in entries
 
    // DW1
    volatile
    uint32_t prdbc;     // Physical region descriptor byte count transferred
 
    // DW2, 3
    uint32_t ctba;      // Command table descriptor base address
    uint32_t ctbau;     // Command table descriptor base address upper 32 bits
 
    // DW4 - 7
    uint32_t rsv1[4];   // Reserved
} hbaCmdHeader_t;

typedef struct taghbaPrdtEntry
{
    uint32_t dba;       // Data base address
    uint32_t dbau;      // Data base address upper 32 bits
    uint32_t rsv0;      // Reserved
 
    // DW3
    uint32_t dbc:22;        // Byte count, 4M max
    uint32_t rsv1:9;        // Reserved
    uint32_t i:1;       // Interrupt on completion
} hbaPrdtEntry_t;

typedef struct taghbaCmdTbl
{
    // 0x00
    uint8_t  cfis[64];  // Command FIS
 
    // 0x40
    uint8_t  acmd[16];  // ATAPI command, 12 or 16 bytes
 
    // 0x50
    uint8_t  rsv[48];   // Reserved
 
    // 0x80
    hbaPrdtEntry_t  prdt_entry[1];  // Physical region descriptor table entries, 0 ~ 65535
} hbaCmdTbl_t;
 
/*Commands*/

typedef struct taghddDevice
{
    hbaPort_t* port;
    uint64_t sector_count;
    uint64_t total_bytes;
    uint16_t sector_size;
} hddDevice_t;

typedef struct taghddDeviceList
{
    uint8_t count;
    hddDevice_t devices[32];
} hddDeviceList_t;

typedef struct tagidentifyDeviceData {
    //Word 0
    struct {
        uint16_t Reserved1 : 1;
        uint16_t Retired3 : 1;
        uint16_t ResponseIncomplete : 1;
        uint16_t Retired2 : 3;
        uint16_t FixedDevice : 1;
        uint16_t RemovableMedia : 1;
        uint16_t Retired1 : 7;
        uint16_t DeviceType : 1;
    } GeneralConfiguration;
    //Word 1
    uint16_t NumCylinders;
    uint16_t SpecificConfiguration;
    uint16_t NumHeads;
    uint16_t Retired1[2];
    uint16_t NumSectorsPerTrack;
    uint16_t VendorUnique1[3];
    //Word 10-19
    uint8_t  SerialNumber[20];
    uint16_t Retired2[2];
    uint16_t Obsolete1;
    //Word 23-26
    uint8_t  FirmwareRevision[8];
    //Word 27-46
    uint8_t  ModelNumber[40];
    uint8_t  MaximumBlockTransfer;
    uint8_t  VendorUnique2;
    //Word 48
    struct {
        uint16_t FeatureSupported : 1;
        uint16_t Reserved : 15;
    } TrustedComputing;
    //Word 49-50
    struct {
        uint8_t  CurrentLongPhysicalSectorAlignment : 2;
        uint8_t  ReservedByte49 : 6;
        uint8_t  DmaSupported : 1;
        uint8_t  LbaSupported : 1;
        uint8_t  IordyDisable : 1;
        uint8_t  IordySupported : 1;
        uint8_t  Reserved1 : 1;
        uint8_t  StandybyTimerSupport : 1;
        uint8_t  Reserved2 : 2;
        uint16_t ReservedWord50;
    } Capabilities;
    uint16_t ObsoleteWords51[2];
    //Word 53
    uint16_t TranslationFieldsValid : 3;
    uint16_t Reserved3 : 5;
    uint16_t FreeFallControlSensitivity : 8;
    //Word 54
    uint16_t NumberOfCurrentCylinders;
    uint16_t NumberOfCurrentHeads;
    uint16_t CurrentSectorsPerTrack;
    //Word 57-58
    uint16_t CurrentSectorCapacity[2];
    //Word 59
    uint8_t  CurrentMultiSectorSetting;
    uint8_t  MultiSectorSettingValid : 1;
    uint8_t  ReservedByte59 : 3;
    uint8_t  SanitizeFeatureSupported : 1;
    uint8_t  CryptoScrambleExtCommandSupported : 1;
    uint8_t  OverwriteExtCommandSupported : 1;
    uint8_t  BlockEraseExtCommandSupported : 1;
    //Word 60-61
    uint16_t UserAddressableSectors[2];
    uint16_t ObsoleteWord62;
    //Word 63
    uint16_t MultiWordDMASupport : 8;
    uint16_t MultiWordDMAActive : 8;
    //Word 64
    uint16_t AdvancedPIOModes : 8;
    uint16_t ReservedByte64 : 8;
    //Word 65
    uint16_t MinimumMWXferCycleTime;
    uint16_t RecommendedMWXferCycleTime;
    uint16_t MinimumPIOCycleTime;
    uint16_t MinimumPIOCycleTimeIORDY;
    //Word 69
    struct {
        uint16_t ZonedCapabilities : 2;
        uint16_t NonVolatileWriteCache : 1;
        uint16_t ExtendedUserAddressableSectorsSupported : 1;
        uint16_t DeviceEncryptsAllUserData : 1;
        uint16_t ReadZeroAfterTrimSupported : 1;
        uint16_t Optional28BitCommandsSupported : 1;
        uint16_t IEEE1667 : 1;
        uint16_t DownloadMicrocodeDmaSupported : 1;
        uint16_t SetMaxSetPasswordUnlockDmaSupported : 1;
        uint16_t WriteBufferDmaSupported : 1;
        uint16_t ReadBufferDmaSupported : 1;
        uint16_t DeviceConfigIdentifySetDmaSupported : 1;
        uint16_t LPSAERCSupported : 1;
        uint16_t DeterministicReadAfterTrimSupported : 1;
        uint16_t CFastSpecSupported : 1;
    } AdditionalSupported;
    //Word 70-74
    uint16_t ReservedWords70[5];
    //Word 75
    uint16_t QueueDepth : 5;
    uint16_t ReservedWord75 : 11;
    //Word 76-77
    struct {
        uint16_t Reserved0 : 1;
        uint16_t SataGen1 : 1;
        uint16_t SataGen2 : 1;
        uint16_t SataGen3 : 1;
        uint16_t Reserved1 : 4;
        uint16_t NCQ : 1;
        uint16_t HIPM : 1;
        uint16_t PhyEvents : 1;
        uint16_t NcqUnload : 1;
        uint16_t NcqPriority : 1;
        uint16_t HostAutoPS : 1;
        uint16_t DeviceAutoPS : 1;
        uint16_t ReadLogDMA : 1;
        uint16_t Reserved2 : 1;
        uint16_t CurrentSpeed : 3;
        uint16_t NcqStreaming : 1;
        uint16_t NcqQueueMgmt : 1;
        uint16_t NcqReceiveSend : 1;
        uint16_t DEVSLPtoReducedPwrState : 1;
        uint16_t Reserved3 : 8;
    } SerialAtaCapabilities;
    //Word 78
    struct {
        uint16_t Reserved0 : 1;
        uint16_t NonZeroOffsets : 1;
        uint16_t DmaSetupAutoActivate : 1;
        uint16_t DIPM : 1;
        uint16_t InOrderData : 1;
        uint16_t HardwareFeatureControl : 1;
        uint16_t SoftwareSettingsPreservation : 1;
        uint16_t NCQAutosense : 1;
        uint16_t DEVSLP : 1;
        uint16_t HybridInformation : 1;
        uint16_t Reserved1 : 6;
    } SerialAtaFeaturesSupported;
    //Word 79
    struct {
        uint16_t Reserved0 : 1;
        uint16_t NonZeroOffsets : 1;
        uint16_t DmaSetupAutoActivate : 1;
        uint16_t DIPM : 1;
        uint16_t InOrderData : 1;
        uint16_t HardwareFeatureControl : 1;
        uint16_t SoftwareSettingsPreservation : 1;
        uint16_t DeviceAutoPS : 1;
        uint16_t DEVSLP : 1;
        uint16_t HybridInformation : 1;
        uint16_t Reserved1 : 6;
    } SerialAtaFeaturesEnabled;
    //Word 80
    uint16_t MajorRevision;
    uint16_t MinorRevision;
    //Word 82-84
    struct {
        //82
        uint16_t SmartCommands : 1;
        uint16_t SecurityMode : 1;
        uint16_t RemovableMediaFeature : 1;
        uint16_t PowerManagement : 1;
        uint16_t Reserved1 : 1;
        uint16_t WriteCache : 1;
        uint16_t LookAhead : 1;
        uint16_t ReleaseInterrupt : 1;
        uint16_t ServiceInterrupt : 1;
        uint16_t DeviceReset : 1;
        uint16_t HostProtectedArea : 1;
        uint16_t Obsolete1 : 1;
        uint16_t WriteBuffer : 1;
        uint16_t ReadBuffer : 1;
        uint16_t Nop : 1;
        uint16_t Obsolete2 : 1;
        //83
        uint16_t DownloadMicrocode : 1;
        uint16_t DmaQueued : 1;
        uint16_t Cfa : 1;
        uint16_t AdvancedPm : 1;
        uint16_t Msn : 1;
        uint16_t PowerUpInStandby : 1;
        uint16_t ManualPowerUp : 1;
        uint16_t Reserved2 : 1;
        uint16_t SetMax : 1;
        uint16_t Acoustics : 1;
        uint16_t BigLba : 1;
        uint16_t DeviceConfigOverlay : 1;
        uint16_t FlushCache : 1;
        uint16_t FlushCacheExt : 1;
        uint16_t WordValid83 : 2;
        //84
        uint16_t SmartErrorLog : 1;
        uint16_t SmartSelfTest : 1;
        uint16_t MediaSerialNumber : 1;
        uint16_t MediaCardPassThrough : 1;
        uint16_t StreamingFeature : 1;
        uint16_t GpLogging : 1;
        uint16_t WriteFua : 1;
        uint16_t WriteQueuedFua : 1;
        uint16_t WWN64Bit : 1;
        uint16_t URGReadStream : 1;
        uint16_t URGWriteStream : 1;
        uint16_t ReservedForTechReport : 2;
        uint16_t IdleWithUnloadFeature : 1;
        uint16_t WordValid : 2;
    } CommandSetSupport;
    //Word 85-87
    struct {
        //85
        uint16_t SmartCommands : 1;
        uint16_t SecurityMode : 1;
        uint16_t RemovableMediaFeature : 1;
        uint16_t PowerManagement : 1;
        uint16_t Reserved1 : 1;
        uint16_t WriteCache : 1;
        uint16_t LookAhead : 1;
        uint16_t ReleaseInterrupt : 1;
        uint16_t ServiceInterrupt : 1;
        uint16_t DeviceReset : 1;
        uint16_t HostProtectedArea : 1;
        uint16_t Obsolete1 : 1;
        uint16_t WriteBuffer : 1;
        uint16_t ReadBuffer : 1;
        uint16_t Nop : 1;
        uint16_t Obsolete2 : 1;
        //86
        uint16_t DownloadMicrocode : 1;
        uint16_t DmaQueued : 1;
        uint16_t Cfa : 1;
        uint16_t AdvancedPm : 1;
        uint16_t Msn : 1;
        uint16_t PowerUpInStandby : 1;
        uint16_t ManualPowerUp : 1;
        uint16_t Reserved2 : 1;
        uint16_t SetMax : 1;
        uint16_t Acoustics : 1;
        uint16_t BigLba : 1;
        uint16_t DeviceConfigOverlay : 1;
        uint16_t FlushCache : 1;
        uint16_t FlushCacheExt : 1;
        uint16_t Resrved3 : 1;
        uint16_t Words119_120Valid : 1;
        //87
        uint16_t SmartErrorLog : 1;
        uint16_t SmartSelfTest : 1;
        uint16_t MediaSerialNumber : 1;
        uint16_t MediaCardPassThrough : 1;
        uint16_t StreamingFeature : 1;
        uint16_t GpLogging : 1;
        uint16_t WriteFua : 1;
        uint16_t WriteQueuedFua : 1;
        uint16_t WWN64Bit : 1;
        uint16_t URGReadStream : 1;
        uint16_t URGWriteStream : 1;
        uint16_t ReservedForTechReport : 2;
        uint16_t IdleWithUnloadFeature : 1;
        uint16_t Reserved4 : 2;
    } CommandSetActive;
    //Word 88
    uint16_t UltraDMASupport : 8;
    uint16_t UltraDMAActive : 8;
    //Word 89
    struct {
        uint16_t TimeRequired : 15;
        uint16_t ExtendedTimeReported : 1;
    } NormalSecurityEraseUnit;
    //Word 90
    struct {
        uint16_t TimeRequired : 15;
        uint16_t ExtendedTimeReported : 1;
    } EnhancedSecurityEraseUnit;
    //Word 91
    uint16_t CurrentAPMLevel : 8;
    uint16_t ReservedWord91 : 8;
    //Word 92
    uint16_t MasterPasswordID;
    uint16_t HardwareResetResult;
    //Word 94
    uint16_t CurrentAcousticValue : 8;
    uint16_t RecommendedAcousticValue : 8;
    //Word 95
    uint16_t StreamMinRequestSize;
    uint16_t StreamingTransferTimeDMA;
    uint16_t StreamingAccessLatencyDMAPIO;
    //Word 98-99
    uint16_t StreamingPerfGranularity[2];
    //Word 100-103
    uint16_t Max48BitLBA[4];
    //Word 104
    uint16_t StreamingTransferTime;
    uint16_t DsmCap;
    //Word 106
    struct {
        uint16_t LogicalSectorsPerPhysicalSector : 4;
        uint16_t Reserved0 : 8;
        uint16_t LogicalSectorLongerThan256Words : 1;
        uint16_t MultipleLogicalSectorsPerPhysicalSector : 1;
        uint16_t Reserved1 : 2;
    } PhysicalLogicalSectorSize;
    //Word 107
    uint16_t InterSeekDelay;
    //Word 108-111
    uint16_t WorldWideName[4];
    //Word 112-115
    uint16_t ReservedForWorldWideName128[4];
    //Word 116
    uint16_t ReservedForTlcTechnicalReport;
    //Word 117-118
    uint16_t WordsPerLogicalSector[2];
    //Word 119
    struct {
        uint16_t ReservedForDrqTechnicalReport : 1;
        uint16_t WriteReadVerify : 1;
        uint16_t WriteUncorrectableExt : 1;
        uint16_t ReadWriteLogDmaExt : 1;
        uint16_t DownloadMicrocodeMode3 : 1;
        uint16_t FreefallControl : 1;
        uint16_t SenseDataReporting : 1;
        uint16_t ExtendedPowerConditions : 1;
        uint16_t Reserved0 : 6;
        uint16_t WordValid : 2;
    } CommandSetSupportExt;
    //Word 120
    struct {
        uint16_t ReservedForDrqTechnicalReport : 1;
        uint16_t WriteReadVerify : 1;
        uint16_t WriteUncorrectableExt : 1;
        uint16_t ReadWriteLogDmaExt : 1;
        uint16_t DownloadMicrocodeMode3 : 1;
        uint16_t FreefallControl : 1;
        uint16_t SenseDataReporting : 1;
        uint16_t ExtendedPowerConditions : 1;
        uint16_t Reserved0 : 6;
        uint16_t Reserved1 : 2;
    } CommandSetActiveExt;
    //Word 121-126
    uint16_t ReservedForExpandedSupportandActive[6];
    //Word 127
    uint16_t MsnSupport : 2;
    uint16_t ReservedWord127 : 14;
    //Word 128
    struct {
        uint16_t SecuritySupported : 1;
        uint16_t SecurityEnabled : 1;
        uint16_t SecurityLocked : 1;
        uint16_t SecurityFrozen : 1;
        uint16_t SecurityCountExpired : 1;
        uint16_t EnhancedSecurityEraseSupported : 1;
        uint16_t Reserved0 : 2;
        uint16_t SecurityLevel : 1;
        uint16_t Reserved1 : 7;
    } SecurityStatus;
    //Word 129-159
    uint16_t ReservedWord129[31];
    //Word 160
    struct {
        uint16_t MaximumCurrentInMA : 12;
        uint16_t CfaPowerMode1Disabled : 1;
        uint16_t CfaPowerMode1Required : 1;
        uint16_t Reserved0 : 1;
        uint16_t Word160Supported : 1;
    } CfaPowerMode1;
    //Word 161-167
    uint16_t ReservedForCfaWord161[7];
    //Word 168
    uint16_t NominalFormFactor : 4;
    uint16_t ReservedWord168 : 12;
    //Word 169
    struct {
        uint16_t SupportsTrim : 1;
        uint16_t Reserved0 : 15;
    } DataSetManagementFeature;
    //Word 170-173
    uint16_t AdditionalProductID[4];
    //Word 174-175
    uint16_t ReservedForCfaWord174[2];
    //Word 176-205
    uint16_t CurrentMediaSerialNumber[30];
    //Word 206
    struct {
        uint16_t Supported : 1;
        uint16_t Reserved0 : 1;
        uint16_t WriteSameSuported : 1;
        uint16_t ErrorRecoveryControlSupported : 1;
        uint16_t FeatureControlSuported : 1;
        uint16_t DataTablesSuported : 1;
        uint16_t Reserved1 : 6;
        uint16_t VendorSpecific : 4;
    } SCTCommandTransport;
    //Word 207-208
    uint16_t ReservedWord207[2];
    //Word 209
    struct {
        uint16_t AlignmentOfLogicalWithinPhysical : 14;
        uint16_t Word209Supported : 1;
        uint16_t Reserved0 : 1;
    } BlockAlignment;
    //Word 210-211
    uint16_t WriteReadVerifySectorCountMode3Only[2];
    //Word 212-213
    uint16_t WriteReadVerifySectorCountMode2Only[2];
    //Word 214
    struct {
        uint16_t NVCachePowerModeEnabled : 1;
        uint16_t Reserved0 : 3;
        uint16_t NVCacheFeatureSetEnabled : 1;
        uint16_t Reserved1 : 3;
        uint16_t NVCachePowerModeVersion : 4;
        uint16_t NVCacheFeatureSetVersion : 4;
    } NVCacheCapabilities;
    //Word 215
    uint16_t NVCacheSizeLSW;
    uint16_t NVCacheSizeMSW;
    uint16_t NominalMediaRotationRate;
    uint16_t ReservedWord218;
    //Word 219
    struct {
        uint8_t NVCacheEstimatedTimeToSpinUpInSeconds;
        uint8_t Reserved;
    } NVCacheOptions;
    //Word 220
    uint16_t WriteReadVerifySectorCountMode : 8;
    uint16_t ReservedWord220 : 8;
    //Word 221
    uint16_t ReservedWord221;
    //Word 222
    struct {
        uint16_t MajorVersion : 12;
        uint16_t TransportType : 4;
    } TransportMajorVersion;
    //Word 223
    uint16_t TransportMinorVersion;
    //Word 224-229
    uint16_t ReservedWord224[6];
    //Word 230-233
    uint16_t ExtendedNumberOfUserAddressableSectors[4];
    //Word 234
    uint16_t MinBlocksPerDownloadMicrocodeMode03;
    uint16_t MaxBlocksPerDownloadMicrocodeMode03;
    //Word 236-254
    uint16_t ReservedWord236[19];
    //Word 255
    uint16_t Signature : 8;
    uint16_t CheckSum : 8;
} identifyDeviceData_t;



void _ahci_init(void);

hddDeviceList_t _get_device_list(void);

bool_t _write_disk(hddDevice_t device, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf);

bool_t _read_disk(hddDevice_t device, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf);

#endif
