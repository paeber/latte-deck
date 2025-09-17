/*
  ups_hid_core.h
  UPS HID Core Implementation for LatteDeck

  Copyright (c) 2015, Arduino LLC
  Original code (pre-library): Copyright (c) 2011, Peter Barrett
  Modified code: Copyright (c) 2020, Aleksandr Bratchik
  Modified for LatteDeck project

  Permission to use, copy, modify, and/or distribute this software for
  any purpose with or without fee is hereby granted, provided that the
  above copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
  SOFTWARE.
 */

#ifndef UPS_HID_CORE_H
#define UPS_HID_CORE_H

#include <stdint.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <PluggableUSB.h>

// Forward declaration to avoid circular includes
class UPSHIDCore;

// HID 'Driver'
// ------------
#define UPS_HID_GET_REPORT        0x01
#define UPS_HID_GET_IDLE          0x02
#define UPS_HID_GET_PROTOCOL      0x03
#define UPS_HID_SET_REPORT        0x09
#define UPS_HID_SET_IDLE          0x0A
#define UPS_HID_SET_PROTOCOL      0x0B

#define UPS_HID_HID_DESCRIPTOR_TYPE         0x21
#define UPS_HID_REPORT_DESCRIPTOR_TYPE      0x22
#define UPS_HID_PHYSICAL_DESCRIPTOR_TYPE    0x23
#define UPS_HID_STRING_DESCRIPTOR_TYPE      0x03

// HID subclass HID1.11 Page 8 4.2 Subclass
#define UPS_HID_SUBCLASS_NONE 0
#define UPS_HID_SUBCLASS_BOOT_INTERFACE 1

// HID Keyboard/Mouse bios compatible protocols HID1.11 Page 9 4.3 Protocols
#define UPS_HID_PROTOCOL_NONE 0
#define UPS_HID_PROTOCOL_KEYBOARD 1
#define UPS_HID_PROTOCOL_MOUSE 2

// Normal or bios protocol (Keyboard/Mouse) HID1.11 Page 54 7.2.5 Get_Protocol Request
// "protocol" variable is used for this purpose.
#define UPS_HID_BOOT_PROTOCOL	0
#define UPS_HID_REPORT_PROTOCOL	1

// HID Request Type HID1.11 Page 51 7.2.1 Get_Report Request
#define UPS_HID_REPORT_TYPE_INPUT   1
#define UPS_HID_REPORT_TYPE_OUTPUT  2
#define UPS_HID_REPORT_TYPE_FEATURE 3

// UPS HID Interface definitions - use different interface numbers to avoid conflicts
#define UPS_HID_INTERFACE		(CDC_ACM_INTERFACE + CDC_INTERFACE_COUNT + 3)		// UPS HID Interface (after Mouse/Keyboard)
#define UPS_HID_FIRST_ENDPOINT	(CDC_FIRST_ENDPOINT + CDC_ENPOINT_COUNT + 2)		// After Mouse/Keyboard endpoints
#define UPS_HID_ENDPOINT_INT	(UPS_HID_FIRST_ENDPOINT)
#define UPS_HID_ENDPOINT_OUT	(UPS_HID_FIRST_ENDPOINT+1)   

#define UPS_HID_TX UPS_HID_ENDPOINT_INT
#define UPS_HID_RX UPS_HID_ENDPOINT_OUT

typedef struct
{
  uint8_t len;      // 9
  uint8_t dtype;    // 0x21
  uint8_t addr;
  uint8_t versionL; // 0x101
  uint8_t versionH; // 0x101
  uint8_t country;
  uint8_t desctype; // 0x22 report
  uint8_t descLenL;
  uint8_t descLenH;
} UPSHIDDescDescriptor;

typedef struct 
{
  InterfaceDescriptor hid;
  UPSHIDDescDescriptor   desc;
  EndpointDescriptor  in;
  EndpointDescriptor  out;                  //added
} UPSHIDDescriptor;

class UPSHIDReport {
public:
    UPSHIDReport *next = NULL;
    UPSHIDReport(uint16_t i, const void *d, uint8_t l) : id(i), data(d), length(l) {}
    
    uint16_t id;
    const void* data;
    uint16_t length;
    bool lock;
};

class UPSHIDSubDescriptor {
public:
  UPSHIDSubDescriptor *next = NULL;
  UPSHIDSubDescriptor(const void *d, uint16_t l) : data(d), length(l) { }

  const void* data;
  const uint16_t length;
};

class UPSHIDCore : public PluggableUSBModule
{
public:
    UPSHIDCore(void);
    int begin(void);
    int SendReport(uint16_t id, const void* data, int len);
    int SetFeature(uint16_t id, const void* data, int len);
    bool LockFeature(uint16_t id, bool lock);
    
    void AppendDescriptor(UPSHIDSubDescriptor* node);
    
    void setOutput(Serial_& out) {
        dbg = &out;
    }
    
    void setSerial(const char* s) {
        serial = s;
    }
    
    UPSHIDReport* GetFeature(uint16_t id);
    
protected:
    // Implementation of the PluggableUSBModule
    int getInterface(uint8_t* interfaceCount) override;
    int getDescriptor(USBSetup& setup) override;
    bool setup(USBSetup& setup) override;
    uint8_t getShortName(char* name) override;
    
private:
    uint8_t epType[2];

    UPSHIDSubDescriptor* rootNode;
    uint16_t descriptorSize;

    uint8_t protocol;
    uint8_t idle;
  
    // Buffer pointer to hold the feature data
    UPSHIDReport* rootReport;
    uint16_t reportCount;
    
    Serial_ *dbg;
    
    const char *serial;
    
};

// Replacement for global singleton.
// This function prevents static-initialization-order-fiasco
// https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
UPSHIDCore& UPSHIDCore();

#define D_UPS_HIDREPORT(length) { 9, 0x21, 0x01, 0x01, 0x21, 1, 0x22, lowByte(length), highByte(length) }

#endif // UPS_HID_CORE_H
