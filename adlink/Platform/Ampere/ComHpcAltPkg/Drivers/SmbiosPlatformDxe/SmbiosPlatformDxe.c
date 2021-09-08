/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Guid/PlatformInfoHobGuid.h>
#include <Guid/SmBios.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <PlatformInfoHob.h>
#include <Protocol/IpmiProtocol.h>
#include <Protocol/Smbios.h>

// Type0 Data
#define VENDOR_TEMPLATE       "ADLINK\0"
#define BIOS_VERSION_TEMPLATE "TianoCore EDKII\0"
#define RELEASE_DATE_TEMPLATE "MM/DD/YYYY\0"

#define TYPE0_ADDITIONAL_STRINGS                    \
  VENDOR_TEMPLATE       /* Vendor */         \
  BIOS_VERSION_TEMPLATE /* BiosVersion */    \
  RELEASE_DATE_TEMPLATE /* BiosReleaseDate */

// Type1 Data
#define MANUFACTURER_TEMPLATE "ADLINK\0"
#define PRODUCT_NAME_TEMPLATE "COM-HPC-ALT\0"
#define SYS_VERSION_TEMPLATE  "ES1\0"
#define SERIAL_TEMPLATE       "123456789ABCDEFF123456789ABCDEFF\0"
#define SKU_TEMPLATE          "FEDCBA9876543211FEDCBA9876543211\0"
#define FAMILY_TEMPLATE       "COM-HPC\0"

#define TYPE1_ADDITIONAL_STRINGS                  \
  MANUFACTURER_TEMPLATE /* Manufacturer */  \
  PRODUCT_NAME_TEMPLATE /* Product Name */  \
  SYS_VERSION_TEMPLATE  /* Version */       \
  SERIAL_TEMPLATE       /* Serial Number */ \
  SKU_TEMPLATE          /* SKU Number */    \
  FAMILY_TEMPLATE       /* Family */

#define TYPE2_ADDITIONAL_STRINGS                   \
  MANUFACTURER_TEMPLATE /* Manufacturer */   \
  PRODUCT_NAME_TEMPLATE /* Product Name */   \
  "ES1\0"              /* Version */        \
  "Serial Not Set\0"    /* Serial */         \
  "Base of Chassis\0"   /* board location */ \
  "FF\0"                /* Version */        \
  "FF\0"                /* Version */

#define CHASSIS_VERSION_TEMPLATE    "None               \0"
#define CHASSIS_SERIAL_TEMPLATE     "Serial Not Set     \0"
#define CHASSIS_ASSET_TAG_TEMPLATE  "Asset Tag Not Set  \0"

#define TYPE3_ADDITIONAL_STRINGS                 \
  MANUFACTURER_TEMPLATE      /* Manufacturer */ \
  CHASSIS_VERSION_TEMPLATE   /* Version */      \
  CHASSIS_SERIAL_TEMPLATE    /* Serial  */      \
  CHASSIS_ASSET_TAG_TEMPLATE /* Asset Tag */    \
  SKU_TEMPLATE               /* SKU Number */

#define TYPE8_ADDITIONAL_STRINGS      \
  "VGA1 - Rear VGA Connector\0"       \
  "DB-15 Male (VGA)         \0"

#define TYPE9_ADDITIONAL_STRINGS       \
  "Socket 0 Riser 1 x32 - Slot 1\0"

#define TYPE11_ADDITIONAL_STRINGS       \
  "www.adlinktech.com\0"

#define TYPE13_ADDITIONAL_STRINGS       \
  "en|US|iso8859-1\0"

#define TYPE41_ADDITIONAL_STRINGS       \
  "Onboard VGA\0"

//
// Type definition and contents of the default SMBIOS table.
// This table covers only the minimum structures required by
// the SMBIOS specification (section 6.2, version 3.0)
//
#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE0 Base;
  CHAR8              Strings[sizeof (TYPE0_ADDITIONAL_STRINGS)];
} ARM_TYPE0;

typedef struct {
  SMBIOS_TABLE_TYPE1 Base;
  CHAR8              Strings[sizeof (TYPE1_ADDITIONAL_STRINGS)];
} ARM_TYPE1;

typedef struct {
  SMBIOS_TABLE_TYPE2 Base;
  CHAR8              Strings[sizeof (TYPE2_ADDITIONAL_STRINGS)];
} ARM_TYPE2;

typedef struct {
  SMBIOS_TABLE_TYPE3 Base;
  CHAR8              Strings[sizeof (TYPE3_ADDITIONAL_STRINGS)];
} ARM_TYPE3;

typedef struct {
  SMBIOS_TABLE_TYPE8 Base;
  CHAR8              Strings[sizeof (TYPE8_ADDITIONAL_STRINGS)];
} ARM_TYPE8;

typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  CHAR8              Strings[sizeof (TYPE9_ADDITIONAL_STRINGS)];
} ARM_TYPE9;

typedef struct {
  SMBIOS_TABLE_TYPE11 Base;
  CHAR8               Strings[sizeof (TYPE11_ADDITIONAL_STRINGS)];
} ARM_TYPE11;

typedef struct {
  SMBIOS_TABLE_TYPE13 Base;
  CHAR8               Strings[sizeof (TYPE13_ADDITIONAL_STRINGS)];
} ARM_TYPE13;

typedef struct {
  SMBIOS_TABLE_TYPE41 Base;
  CHAR8               Strings[sizeof (TYPE41_ADDITIONAL_STRINGS)];
} ARM_TYPE41;

#pragma pack()

//-------------------------------------
//        SMBIOS Platform Common
//-------------------------------------
enum {
  ADDITIONAL_STR_INDEX_1 = 1,
  ADDITIONAL_STR_INDEX_2,
  ADDITIONAL_STR_INDEX_3,
  ADDITIONAL_STR_INDEX_4,
  ADDITIONAL_STR_INDEX_5,
  ADDITIONAL_STR_INDEX_6,
  ADDITIONAL_STR_INDEX_MAX
};


// Type 0 BIOS information
STATIC ARM_TYPE0 mArmDefaultType0 = {
  {
    {                                   // Header
      EFI_SMBIOS_TYPE_BIOS_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE0),      // UINT8 Length, The length of the structure's string-set is not included.
      SMBIOS_HANDLE_PI_RESERVED,
    },

    ADDITIONAL_STR_INDEX_1,     // SMBIOS_TABLE_STRING       Vendor
    ADDITIONAL_STR_INDEX_2,     // SMBIOS_TABLE_STRING       BiosVersion
    0,                          // UINT16                    BiosSegment
    ADDITIONAL_STR_INDEX_3,     // SMBIOS_TABLE_STRING       BiosReleaseDate
    0,                          // UINT8                     BiosSize

    // MISC_BIOS_CHARACTERISTICS BiosCharacteristics
    {
      0,0,0,0,0,0,
      1, // PCI supported
      0,
      1, // PNP supported
      0,
      1, // BIOS upgradable
      0, 0, 0,
      0, // Boot from CD
      1, // selectable boot
    },

    // BIOSCharacteristicsExtensionBytes[2]
    {
      0,
      0,
    },

    0,     // UINT8                     SystemBiosMajorRelease
    0,     // UINT8                     SystemBiosMinorRelease

    // If the system does not have field upgradeable embedded controller
    // firmware, the value is 0FFh
    0xFF,  // UINT8                     EmbeddedControllerFirmwareMajorRelease
    0xFF   // UINT8                     EmbeddedControllerFirmwareMinorRelease
  },

  // Text strings (unformatted area)
  TYPE0_ADDITIONAL_STRINGS
};

// Type 1 System information
STATIC ARM_TYPE1 mArmDefaultType1 = {
  {
    { // Header
      EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,
      sizeof (SMBIOS_TABLE_TYPE1),
      SMBIOS_HANDLE_PI_RESERVED,
    },

    ADDITIONAL_STR_INDEX_1,                                                     // Manufacturer
    ADDITIONAL_STR_INDEX_2,                                                     // Product Name
    ADDITIONAL_STR_INDEX_3,                                                     // Version
    ADDITIONAL_STR_INDEX_4,                                                     // Serial Number
    { 0x12345678, 0x9ABC, 0xDEFF, { 0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xFF }}, // UUID
    SystemWakeupTypePowerSwitch,                                                // Wakeup type
    ADDITIONAL_STR_INDEX_5,                                                     // SKU Number
    ADDITIONAL_STR_INDEX_6,                                                     // Family
  },

  // Text strings (unformatted)
  TYPE1_ADDITIONAL_STRINGS
};

// Type 2 Baseboard
STATIC ARM_TYPE2 mArmDefaultType2 = {
  {
    {                                        // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE2),           // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1, // Manufacturer
    ADDITIONAL_STR_INDEX_2, // Product Name
    ADDITIONAL_STR_INDEX_3, // Version
    ADDITIONAL_STR_INDEX_4, // Serial
    0,                      // Asset tag
    {1},                    // motherboard, not replaceable
    ADDITIONAL_STR_INDEX_5, // location of board
    0xFFFF,                 // chassis handle
    BaseBoardTypeMotherBoard,
    0,
    {0},
  },
  TYPE2_ADDITIONAL_STRINGS
};

// Type 3 Enclosure
STATIC CONST ARM_TYPE3 mArmDefaultType3 = {
  {
    {                                   // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE3),      // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,          // Manufacturer
    MiscChassisTypeRackMountChassis, // Rack-mounted chassis
    ADDITIONAL_STR_INDEX_2,          // version
    ADDITIONAL_STR_INDEX_3,          // serial
    ADDITIONAL_STR_INDEX_4,          // asset tag
    ChassisStateUnknown,             // boot chassis state
    ChassisStateSafe,                // power supply state
    ChassisStateSafe,                // thermal state
    ChassisSecurityStatusNone,       // security state
    {0,0,0,0},                       // OEM defined
    1,                               // 1U height
    2,                               // number of power cords
    0,                               // no contained elements
    3,                               // ContainedElementRecordLength;
  },
  TYPE3_ADDITIONAL_STRINGS
};

// Type 8 Port Connector Information
STATIC CONST ARM_TYPE8 mArmDefaultType8Vga = {
  {
    {                                             // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE8),                // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,       // InternalReferenceDesignator String
    PortConnectorTypeDB15Female,  // InternalConnectorType;
    ADDITIONAL_STR_INDEX_2,       // ExternalReferenceDesignator String
    PortTypeOther,                // ExternalConnectorType;
    PortTypeVideoPort,            // PortType;
  },
  "VGA1 - Rear VGA Connector\0" \
  "DB-15 Male (VGA)\0"
};

// Type 8 Port Connector Information
STATIC CONST ARM_TYPE8 mArmDefaultType8USBFront = {
  {
    {                                             // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE8),                // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,       // InternalReferenceDesignator String
    PortConnectorTypeUsb,         // InternalConnectorType;
    ADDITIONAL_STR_INDEX_2,       // ExternalReferenceDesignator String
    PortTypeOther,                // ExternalConnectorType;
    PortTypeUsb,                  // PortType;
  },
  "Front Panel USB 3.0\0"  \
  "USB\0"
};

// Type 8 Port Connector Information
STATIC CONST ARM_TYPE8 mArmDefaultType8USBRear = {
  {
    {                                             // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE8),                // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,       // InternalReferenceDesignator String
    PortConnectorTypeUsb,         // InternalConnectorType;
    ADDITIONAL_STR_INDEX_2,       // ExternalReferenceDesignator String
    PortTypeOther,                // ExternalConnectorType;
    PortTypeUsb,                  // PortType;
  },
  "Rear Panel USB 3.0\0"   \
  "USB\0"
};

// Type 8 Port Connector Information
STATIC CONST ARM_TYPE8 mArmDefaultType8NetRJ45 = {
  {
    {                                             // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE8),                // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,       // InternalReferenceDesignator String
    PortConnectorTypeRJ45,        // InternalConnectorType;
    ADDITIONAL_STR_INDEX_2,       // ExternalReferenceDesignator String
    PortConnectorTypeRJ45,        // ExternalConnectorType;
    PortTypeNetworkPort,          // PortType;
  },
  "RJ1 - BMC RJ45 Port\0" \
  "RJ45 Connector\0"
};

// Type 8 Port Connector Information
STATIC CONST ARM_TYPE8 mArmDefaultType8NetOcp = {
  {
    {                                             // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE8),                // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,       // InternalReferenceDesignator String
    PortTypeOther,                // InternalConnectorType;
    ADDITIONAL_STR_INDEX_2,       // ExternalReferenceDesignator String
    PortTypeOther,                // ExternalConnectorType;
    PortTypeNetworkPort,          // PortType;
  },
  "OCP1 - OCP NIC 3.0 Connector\0"  \
  "OCP NIC 3.0\0"
};

// Type 8 Port Connector Information
STATIC CONST ARM_TYPE8 mArmDefaultType8Uart = {
  {
    {                                             // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE8),                // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,        // InternalReferenceDesignator String
    PortTypeOther,                 // InternalConnectorType;
    ADDITIONAL_STR_INDEX_2,        // ExternalReferenceDesignator String
    PortConnectorTypeDB9Female,    // ExternalConnectorType;
    PortTypeSerial16550Compatible, // PortType;
  },
  "UART1 - BMC UART5 Connector\0"  \
  "DB-9 female\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk0RiserX32Slot1 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth16X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    0,
    0,
    0,
  },
  "S0 Riser 1 x32 - Slot 1\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk0RiserX32Slot2 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth8X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    4,
    0,
    0,
  },
  "S0 Riser x32 - Slot 2\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk0RiserX32Slot3 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth8X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    5,
    0,
    0,
  },
  "S0 Riser x32 - Slot 3\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk1RiserX24Slot1 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth8X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    7,
    0,
    0,
  },
  "S1 Riser x24 - Slot 1\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk1RiserX24Slot2 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth8X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    8,
    0,
    0,
  },
  "S1 Riser x24 - Slot 2\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk1RiserX24Slot3 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth8X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    9,
    0,
    0,
  },
  "S1 Riser x24 - Slot 3\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk1RiserX8Slot1 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth8X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    8,
    0,
    0,
  },
  "S1 Riser x8 - Slot 1\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk0OcpNic = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth16X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    1,
    0,
    0,
  },
  "S0 OCP NIC 3.0\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk1NvmeM2Slot1 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth4X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    5,
    0,
    0,
  },
  "S1 NVMe M.2 - Slot 1\0"
};

// Type 9 System Slots
STATIC ARM_TYPE9 mArmDefaultType9Sk1NvmeM2Slot2 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1,
    SlotTypePciExpressGen3,
    SlotDataBusWidth4X,
    SlotUsageAvailable,
    SlotLengthLong,
    0,
    {0, 0, 1}, // Provides 3.3 Volts
    {1},       // PME
    5,
    0,
    0,
  },
  "S1 NVMe M.2 - Slot 2\0"
};

// Type 11 OEM Strings
STATIC ARM_TYPE11 mArmDefaultType11 = {
  {
    {                               // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_OEM_STRINGS,  // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE11), // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    ADDITIONAL_STR_INDEX_1
  },
  TYPE11_ADDITIONAL_STRINGS
};

// Type 13 BIOS Language Information
STATIC ARM_TYPE13 mArmDefaultType13 = {
  {
    {                                            // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE13),              // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    1,
    0,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    1,
  },
  TYPE13_ADDITIONAL_STRINGS
};

// Type 24 Hardware Security
STATIC SMBIOS_TABLE_TYPE24 mArmDefaultType24 = {
  {                                    // SMBIOS_STRUCTURE Hdr
    EFI_SMBIOS_TYPE_HARDWARE_SECURITY, // UINT8 Type
    sizeof (SMBIOS_TABLE_TYPE24),      // UINT8 Length
    SMBIOS_HANDLE_PI_RESERVED,
  },
  0
};

// Type 32 System Boot Information
STATIC SMBIOS_TABLE_TYPE32 mArmDefaultType32 = {
  {                                          // SMBIOS_STRUCTURE Hdr
    EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION, // UINT8 Type
    sizeof (SMBIOS_TABLE_TYPE32),            // UINT8 Length
    SMBIOS_HANDLE_PI_RESERVED,
  },
  {0, 0, 0, 0, 0, 0},
  0
};

// Type 38 IPMI Device Information
STATIC SMBIOS_TABLE_TYPE38 mArmDefaultType38 = {
  {                                          // SMBIOS_STRUCTURE Hdr
    EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION, // UINT8 Type
    sizeof (SMBIOS_TABLE_TYPE38),            // UINT8 Length
    SMBIOS_HANDLE_PI_RESERVED,
  },
  IPMIDeviceInfoInterfaceTypeSSIF,
  0x20,
  0x20,
  0xFF,
  0x20
};

// Type 41 Onboard Devices Extended Information
STATIC ARM_TYPE41 mArmDefaultType41 = {
  {
    { // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION,
      sizeof (SMBIOS_TABLE_TYPE41),
      SMBIOS_HANDLE_PI_RESERVED,
    },
    1,
    0x83,  // OnBoardDeviceExtendedTypeVideo, Enabled
    1,
    4,
    2,
    0,
  },
  TYPE41_ADDITIONAL_STRINGS
};

// Type 42 System Boot Information
STATIC SMBIOS_TABLE_TYPE42 mArmDefaultType42 = {
  { // SMBIOS_STRUCTURE Hdr
    EFI_SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE,
    sizeof (SMBIOS_TABLE_TYPE42),
    SMBIOS_HANDLE_PI_RESERVED,
  },
  MCHostInterfaceTypeOemDefined,
  4,
  {0xFF, 0, 0, 0}
};

STATIC CONST VOID *DefaultCommonTables[] =
{
  &mArmDefaultType0,
  &mArmDefaultType1,
  &mArmDefaultType2,
  &mArmDefaultType8Vga,
  &mArmDefaultType8USBFront,
  &mArmDefaultType8USBRear,
  &mArmDefaultType8NetRJ45,
  &mArmDefaultType8NetOcp,
  &mArmDefaultType8Uart,
  &mArmDefaultType9Sk0RiserX32Slot1,
  &mArmDefaultType9Sk0RiserX32Slot2,
  &mArmDefaultType9Sk0RiserX32Slot3,
  &mArmDefaultType9Sk1RiserX24Slot1,
  &mArmDefaultType9Sk1RiserX24Slot2,
  &mArmDefaultType9Sk1RiserX24Slot3,
  &mArmDefaultType9Sk1RiserX8Slot1,
  &mArmDefaultType9Sk0OcpNic,
  &mArmDefaultType9Sk1NvmeM2Slot1,
  &mArmDefaultType9Sk1NvmeM2Slot2,
  &mArmDefaultType11,
  &mArmDefaultType13,
  &mArmDefaultType24,
  &mArmDefaultType32,
  &mArmDefaultType38,
  &mArmDefaultType41,
  &mArmDefaultType42,
  NULL
};

typedef struct {
  CHAR8 MonthNameStr[4]; // example "Jan", Compiler build date, month
  CHAR8 DigitStr[3];     // example "01", Smbios date format, month
} MonthStringDig;

STATIC MonthStringDig MonthMatch[12] = {
  { "Jan", "01" },
  { "Feb", "02" },
  { "Mar", "03" },
  { "Apr", "04" },
  { "May", "05" },
  { "Jun", "06" },
  { "Jul", "07" },
  { "Aug", "08" },
  { "Sep", "09" },
  { "Oct", "10" },
  { "Nov", "11" },
  { "Dec", "12" }
};

EFI_STATUS
IpmiReadFruInfo (
  VOID
  );

STATIC
VOID
ConstructBuildDate (
  OUT CHAR8 *DateBuf
  )
{
  UINTN i;

  // GCC __DATE__ format is "Feb  2 1996"
  // If the day of the month is less than 10, it is padded with a space on the left
  CHAR8 *BuildDate = __DATE__;

  // SMBIOS spec date string: MM/DD/YYYY
  CHAR8 SmbiosDateStr[sizeof (RELEASE_DATE_TEMPLATE)] = { 0 };

  SmbiosDateStr[sizeof (RELEASE_DATE_TEMPLATE) - 1] = '\0';

  SmbiosDateStr[2] = '/';
  SmbiosDateStr[5] = '/';

  // Month
  for (i = 0; i < sizeof (MonthMatch) / sizeof (MonthMatch[0]); i++) {
    if (AsciiStrnCmp (&BuildDate[0], MonthMatch[i].MonthNameStr, AsciiStrLen (MonthMatch[i].MonthNameStr)) == 0) {
      CopyMem (&SmbiosDateStr[0], MonthMatch[i].DigitStr, AsciiStrLen (MonthMatch[i].DigitStr));
      break;
    }
  }

  // Day
  CopyMem (&SmbiosDateStr[3], &BuildDate[4], 2);
  if (BuildDate[4] == ' ') {
    // day is less then 10, SAPCE filed by compiler, SMBIOS requires 0
    SmbiosDateStr[3] = '0';
  }

  // Year
  CopyMem (&SmbiosDateStr[6], &BuildDate[7], 4);

  CopyMem (DateBuf, SmbiosDateStr, AsciiStrLen (RELEASE_DATE_TEMPLATE));
}

STATIC
UINT8
GetBiosVerMajor (
  VOID
  )
{
  return (PcdGet8 (PcdSmbiosTables1MajorVersion));
}

STATIC
UINT8
GetBiosVerMinor (
  VOID
  )
{
  return (PcdGet8 (PcdSmbiosTables1MinorVersion));
}

STATIC
EFI_STATUS
UpdateSmbiosType0 (
  PLATFORM_INFO_HOB  *PlatformHob
  )
{
  EFI_STATUS                          Status        = EFI_SUCCESS;
  MISC_BIOS_CHARACTERISTICS_EXTENSION *MiscExt      = NULL;
  CHAR8                               *ReleaseDateBuf = NULL;
  CHAR8                               *PcdReleaseDate = NULL;
  CHAR8                               AsciiVersion[32];
  UINTN                               Index;

  //
  //  Update Type0 information
  //
  ReleaseDateBuf = &mArmDefaultType0.Strings[0]
                   + sizeof (VENDOR_TEMPLATE) - 1
                   + sizeof (BIOS_VERSION_TEMPLATE) - 1;
  PcdReleaseDate = (CHAR8 *)PcdGetPtr (PcdSmbiosTables0BiosReleaseDate);

  if (AsciiStrnCmp (PcdReleaseDate, RELEASE_DATE_TEMPLATE, AsciiStrLen (RELEASE_DATE_TEMPLATE)) == 0) {
    // If PCD is still template date MM/DD/YYYY, use compiler date
    ConstructBuildDate (ReleaseDateBuf);
  } else {
    // PCD is updated somehow, use PCD date
    CopyMem (ReleaseDateBuf, PcdReleaseDate, AsciiStrLen (PcdReleaseDate));
  }

  if (PcdGet32 (PcdFdSize) < SIZE_16MB) {
    mArmDefaultType0.Base.BiosSize = (PcdGet32 (PcdFdSize) / SIZE_64KB) - 1;

    mArmDefaultType0.Base.ExtendedBiosSize.Size = 0;
    mArmDefaultType0.Base.ExtendedBiosSize.Unit = 0;
  } else {
    // TODO: Need to update Extended BIOS ROM Size
    mArmDefaultType0.Base.BiosSize = 0xFF;

    // As a reminder
    ASSERT (FALSE);
  }

  // Type0 BIOS Characteristics Extension Byte 1
  MiscExt = (MISC_BIOS_CHARACTERISTICS_EXTENSION *)&(mArmDefaultType0.Base.BIOSCharacteristicsExtensionBytes);

  MiscExt->BiosReserved.AcpiIsSupported = 1;

  // Type0 BIOS Characteristics Extension Byte 2
  MiscExt->SystemReserved.BiosBootSpecIsSupported = 1;
  MiscExt->SystemReserved.FunctionKeyNetworkBootIsSupported = 1;
  MiscExt->SystemReserved.UefiSpecificationSupported = 1;

  // Type0 BIOS Release
  // TODO: to decide another way: If the system does not support the use of this
  // field, the value is 0FFh
  mArmDefaultType0.Base.SystemBiosMajorRelease = GetBiosVerMajor ();
  mArmDefaultType0.Base.SystemBiosMinorRelease = GetBiosVerMinor ();

  /* Update SMBIOS Type 0 EC Info */
  CopyMem (
    (VOID *)&AsciiVersion,
    (VOID *)&PlatformHob->SmPmProVer,
    sizeof (PlatformHob->SmPmProVer)
    );
  /* The AsciiVersion is formated as "major.minor" */
  for (Index = 0; Index < (UINTN)AsciiStrLen (AsciiVersion); Index++) {
    if (AsciiVersion[Index] == '.') {
      AsciiVersion[Index] = '\0';
      break;
    }
  }

  mArmDefaultType0.Base.EmbeddedControllerFirmwareMajorRelease =
    (UINT8)AsciiStrDecimalToUintn (AsciiVersion);
  mArmDefaultType0.Base.EmbeddedControllerFirmwareMinorRelease =
    (UINT8)AsciiStrDecimalToUintn (AsciiVersion + Index + 1);

  return Status;
}

STATIC
EFI_STATUS
InstallType3Structure (
  IN EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE   SmbiosHandle;
  SMBIOS_TABLE_TYPE3  *InputData;
  SMBIOS_TABLE_TYPE3  *SmbiosRecord;
  UINTN               ManuStrLen;
  UINTN               VerStrLen;
  UINTN               AssertTagStrLen;
  UINTN               SerialNumStrLen;
  UINTN               ChaNumStrLen;
  UINT8               ContainedElementCount;
  UINT8               ExtendLength;
  SMBIOS_TABLE_STRING *SkuNumberPtr;
  UINTN               StringOffset;

  ASSERT (Smbios != NULL);

  ManuStrLen = AsciiStrLen (MANUFACTURER_TEMPLATE);
  VerStrLen = AsciiStrLen (CHASSIS_VERSION_TEMPLATE);
  SerialNumStrLen = AsciiStrLen (CHASSIS_SERIAL_TEMPLATE);
  AssertTagStrLen = AsciiStrLen (CHASSIS_ASSET_TAG_TEMPLATE);
  ChaNumStrLen = AsciiStrLen (SKU_TEMPLATE);

  InputData = (SMBIOS_TABLE_TYPE3 *)&mArmDefaultType3;

  ContainedElementCount = InputData->ContainedElementCount;

  ExtendLength = 0;
  if (ContainedElementCount > 1) {
    ExtendLength = (ContainedElementCount - 1) * sizeof (CONTAINED_ELEMENT);
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocateZeroPool (
                   sizeof (SMBIOS_TABLE_TYPE3)
                   + ExtendLength     + 1
                   + ManuStrLen       + 1
                   + VerStrLen        + 1
                   + SerialNumStrLen  + 1
                   + AssertTagStrLen  + 1
                   + ChaNumStrLen     + 1 + 1
                   );
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (SmbiosRecord, InputData, sizeof (SMBIOS_TABLE_TYPE3));

  SmbiosRecord->Hdr.Length = OFFSET_OF (SMBIOS_TABLE_TYPE3, ContainedElements) + ExtendLength + sizeof (SMBIOS_TABLE_STRING);

  //
  // TODO: Create ContainedElements and copy them into SMBIOS record if it is supported
  //
  // if (ExtendLength > 0) {
  //   CopyMem ((UINT8 *)SmbiosRecord + OFFSET_OF (SMBIOS_TABLE_TYPE3, ContainedElements), &ContainedElements, ExtendLength);
  // }

  SkuNumberPtr = (UINT8 *)SmbiosRecord + SmbiosRecord->Hdr.Length - sizeof (SMBIOS_TABLE_STRING);
  *SkuNumberPtr = 5;

  StringOffset = SmbiosRecord->Hdr.Length;
  CopyMem ((UINT8 *)SmbiosRecord + StringOffset, (UINT8 *)MANUFACTURER_TEMPLATE, ManuStrLen);
  StringOffset += ManuStrLen + 1;
  CopyMem ((UINT8 *)SmbiosRecord + StringOffset, (UINT8 *)CHASSIS_VERSION_TEMPLATE, VerStrLen);
  StringOffset += VerStrLen + 1;
  CopyMem ((UINT8 *)SmbiosRecord + StringOffset, (UINT8 *)CHASSIS_SERIAL_TEMPLATE, SerialNumStrLen);
  StringOffset += SerialNumStrLen + 1;
  CopyMem ((UINT8 *)SmbiosRecord + StringOffset, (UINT8 *)CHASSIS_ASSET_TAG_TEMPLATE, AssertTagStrLen);
  StringOffset += AssertTagStrLen + 1;
  CopyMem ((UINT8 *)SmbiosRecord + StringOffset, (UINT8 *)SKU_TEMPLATE, ChaNumStrLen);

  SmbiosHandle = ((EFI_SMBIOS_TABLE_HEADER *)SmbiosRecord)->Handle;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER *)SmbiosRecord
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "adding SMBIOS type 3 failed\n"));
    // stop adding rather than continuing
    return Status;
  }

  // Save this handle to type 2 table
  mArmDefaultType2.Base.ChassisHandle = SmbiosHandle;

  FreePool (SmbiosRecord);

  return Status;
}

/**
   Install a whole table worth of structures

   @param  Smbios               SMBIOS protocol.
   @param  DefaultTables        A pointer to the default SMBIOS table structure.
**/
EFI_STATUS
InstallStructures (
  IN       EFI_SMBIOS_PROTOCOL *Smbios,
  IN CONST VOID                *DefaultTables[]
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE SmbiosHandle;
  UINTN             TableIndex;

  ASSERT (Smbios != NULL);

  for (TableIndex = 0; DefaultTables[TableIndex] != NULL; TableIndex++) {
    SmbiosHandle = ((EFI_SMBIOS_TABLE_HEADER *)DefaultTables[TableIndex])->Handle;
    Status = Smbios->Add (
                       Smbios,
                       NULL,
                       &SmbiosHandle,
                       (EFI_SMBIOS_TABLE_HEADER *)DefaultTables[TableIndex]
                       );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: adding %d failed\n", __FUNCTION__, TableIndex));

      // stop adding rather than continuing
      return Status;
    }
  }

  return EFI_SUCCESS;
}

STATIC
VOID
UpdateSmbiosInfo (
  VOID
  )
{
  VOID               *Hob;
  PLATFORM_INFO_HOB  *PlatformHob;

  /* Get the Platform HOB */
  Hob = GetFirstGuidHob (&gPlatformHobGuid);
  ASSERT (Hob != NULL);
  if (Hob == NULL) {
    return;
  }

  PlatformHob = (PLATFORM_INFO_HOB *)GET_GUID_HOB_DATA (Hob);

  //
  //  Update Type0 information
  //
  UpdateSmbiosType0 (PlatformHob);

}

/**
   Install all structures from the DefaultTables structure

   @param  Smbios               SMBIOS protocol

**/
EFI_STATUS
InstallAllStructures (
  IN EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  ASSERT (Smbios != NULL);

  // Update SMBIOS Tables
  UpdateSmbiosInfo ();

  // Install Type 3 table
  InstallType3Structure (Smbios);

  // Install Tables
  Status = InstallStructures (Smbios, DefaultCommonTables);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SmbiosUpdateString (
  IN EFI_SMBIOS_PROTOCOL *Smbios,
  IN EFI_SMBIOS_HANDLE   SmbiosHandle,
  IN SMBIOS_TABLE_STRING StringNumber,
  IN CHAR8               *String
  )
{
  EFI_STATUS Status;
  UINTN      StringIndex;

  if (String == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*String == '\0') {
    // A string with no data is not legal in SMBIOS
    return EFI_INVALID_PARAMETER;
  }

  StringIndex = StringNumber;
  Status = Smbios->UpdateString (Smbios, &SmbiosHandle, &StringIndex, String);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

VOID
ConvertIpmiGuidToSmbiosGuid (
  IN OUT UINT8 *SmbiosGuid,
  IN     UINT8 *IpmiGuid
  )
{
  UINT8 Index;

  //
  // Node and clock seq field within the GUID
  // are stored most-significant byte first in
  // SMBIOS spec but LSB first in IPMI spec
  // ->change its offset and byte-order
  //
  for (Index = 0; Index < 8; Index++) {
    *(SmbiosGuid + 15 - Index) = *(IpmiGuid + Index);
  }
  //
  // Time high, time mid and time low field
  // are stored LSB first in both IPMI spec
  // and SMBIOS spec
  // ->only need change offset
  //
  *(SmbiosGuid + 6) = *(IpmiGuid + 8);
  *(SmbiosGuid + 7) = *(IpmiGuid + 9);
  *(SmbiosGuid + 4) = *(IpmiGuid + 10);
  *(SmbiosGuid + 5) = *(IpmiGuid + 11);
  *SmbiosGuid       = *(IpmiGuid + 12);
  *(SmbiosGuid + 1) = *(IpmiGuid + 13);
  *(SmbiosGuid + 2) = *(IpmiGuid + 14);
  *(SmbiosGuid + 3) = *(IpmiGuid + 15);
}

VOID
UpdateSmbiosType123 (
  EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_STATUS              Status;
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER *Record;
  UINTN                   StringIndex;
  UINT8                   *GuidPtr;

  ASSERT (Smbios != NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
  while (!EFI_ERROR (Status)) {
    //
    // Update SMBIOS Type 1
    //
    if (Record->Type == SMBIOS_TYPE_SYSTEM_INFORMATION) {
      GuidPtr = (UINT8 *)&((SMBIOS_TABLE_TYPE1 *)Record)->Uuid;
      ConvertIpmiGuidToSmbiosGuid (GuidPtr, (UINT8 *)PcdGetPtr (PcdFruSystemUniqueID));
      StringIndex = ((SMBIOS_TABLE_TYPE1 *)Record)->Manufacturer;
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruProductManufacturerName));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruProductName));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruProductVersion));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruProductSerialNumber));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruProductExtra));
    }

    //
    // Update SMBIOS Type 2
    //
    if (Record->Type == SMBIOS_TYPE_BASEBOARD_INFORMATION) {
      StringIndex = ((SMBIOS_TABLE_TYPE2 *)Record)->Manufacturer;
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruBoardManufacturerName));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruBoardProductName));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruBoardPartNumber));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruBoardSerialNumber));
    }

    //
    // Update SMBIOS Type 3
    //
    if (Record->Type == SMBIOS_TYPE_SYSTEM_ENCLOSURE) {
      StringIndex = ((SMBIOS_TABLE_TYPE3 *)Record)->Manufacturer;
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruBoardManufacturerName));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruChassisPartNumber));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruChassisSerialNumber));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruProductAssetTag));
      SmbiosUpdateString (Smbios, SmbiosHandle, StringIndex++, (CHAR8 *)PcdGetPtr (PcdFruChassisExtra));
    }

    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
  }
}

VOID
EFIAPI
IpmiInstalledCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS          Status;
  IPMI_PROTOCOL       *IpmiProtocol;
  EFI_SMBIOS_PROTOCOL *Smbios;

  Status = gBS->LocateProtocol (
                  &gIpmiProtocolGuid,
                  NULL,
                  (VOID **)&IpmiProtocol
                  );
  if (EFI_ERROR (Status) || IpmiProtocol == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: IPMI protocol not installed\n", __FUNCTION__));
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&Smbios
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: SMBIOS protocol not installed\n", __FUNCTION__));
    return;
  }

  Status = IpmiReadFruInfo ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to read FRU Information!\n", __FUNCTION__));
    return;
  }

  //
  // Update SMBIOS Type 1, 2 and 3 based on FRU data that were read from BMC.
  //
  UpdateSmbiosType123 (Smbios);

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
}

/**
   Installs SMBIOS information for ARM platforms

   @param ImageHandle     Module's image handle
   @param SystemTable     Pointer of EFI_SYSTEM_TABLE

   @retval EFI_SUCCESS    Smbios data successfully installed
   @retval Other          Smbios data was not installed

**/
EFI_STATUS
EFIAPI
SmbiosPlatformDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_SMBIOS_PROTOCOL *Smbios;
  EFI_EVENT           IpmiInstalledEvent;
  VOID                *Registration;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&Smbios
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InstallAllStructures (Smbios);
  DEBUG ((DEBUG_ERROR, "SmbiosPlatform install - %r\n", Status));

  IpmiInstalledEvent = EfiCreateProtocolNotifyEvent (
                         &gIpmiProtocolGuid,
                         TPL_CALLBACK,
                         IpmiInstalledCallback,
                         NULL,
                         &Registration
                         );
  ASSERT (IpmiInstalledEvent != NULL);

  return Status;
}
