#ifndef USB_DESCRIPTOR_H
#define USB_DESCRIPTOR_H

#include <Arduino.h>

// ============================================================================
// USB Descriptor for Composite HID Device
// ============================================================================
// This file contains the USB descriptor definitions for a composite HID device
// that includes Power Device (UPS), Mouse, and Keyboard interfaces

// ============================================================================
// USB Device Descriptor
// ============================================================================

const uint8_t USB_DEVICE_DESCRIPTOR[] PROGMEM = {
    0x12,       // bLength
    0x01,       // bDescriptorType (Device)
    0x00, 0x02, // bcdUSB (2.00)
    0x00,       // bDeviceClass (Use Interface Descriptors)
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    0x40,       // bMaxPacketSize0 (64)
    0x83, 0x04, // idVendor (0x0483 - STMicroelectronics)
    0x36, 0x80, // idProduct (0x8036 - Arduino Leonardo)
    0x00, 0x01, // bcdDevice (1.00)
    0x01,       // iManufacturer (String Index)
    0x02,       // iProduct (String Index)
    0x03,       // iSerialNumber (String Index)
    0x01        // bNumConfigurations (1)
};

// ============================================================================
// USB Configuration Descriptor
// ============================================================================

const uint8_t USB_CONFIGURATION_DESCRIPTOR[] PROGMEM = {
    // Configuration Descriptor
    0x09,       // bLength
    0x02,       // bDescriptorType (Configuration)
    0xE0, 0x00, // wTotalLength (224)
    0x03,       // bNumInterfaces (3)
    0x01,       // bConfigurationValue
    0x00,       // iConfiguration (String Index)
    0x80,       // bmAttributes (Bus Powered)
    0x32,       // bMaxPower (100mA)
    
    // Interface 0: Power Device (UPS)
    0x09,       // bLength
    0x04,       // bDescriptorType (Interface)
    0x00,       // bInterfaceNumber (0)
    0x00,       // bAlternateSetting
    0x01,       // bNumEndpoints (1)
    0x03,       // bInterfaceClass (HID)
    0x00,       // bInterfaceSubClass
    0x00,       // bInterfaceProtocol
    0x00,       // iInterface (String Index)
    
    // HID Descriptor for Power Device
    0x09,       // bLength
    0x21,       // bDescriptorType (HID)
    0x11, 0x01, // bcdHID (1.11)
    0x00,       // bCountryCode
    0x01,       // bNumDescriptors
    0x22,       // bDescriptorType (Report)
    0x00, 0x00, // wDescriptorLength (will be set at runtime)
    
    // Endpoint 1: Interrupt IN
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x81,       // bEndpointAddress (IN, 1)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize (64)
    0x0A,       // bInterval (10ms)
    
    // Interface 1: Mouse
    0x09,       // bLength
    0x04,       // bDescriptorType (Interface)
    0x01,       // bInterfaceNumber (1)
    0x00,       // bAlternateSetting
    0x01,       // bNumEndpoints (1)
    0x03,       // bInterfaceClass (HID)
    0x01,       // bInterfaceSubClass (Boot Interface)
    0x02,       // bInterfaceProtocol (Mouse)
    0x00,       // iInterface (String Index)
    
    // HID Descriptor for Mouse
    0x09,       // bLength
    0x21,       // bDescriptorType (HID)
    0x11, 0x01, // bcdHID (1.11)
    0x00,       // bCountryCode
    0x01,       // bNumDescriptors
    0x22,       // bDescriptorType (Report)
    0x00, 0x00, // wDescriptorLength (will be set at runtime)
    
    // Endpoint 2: Interrupt IN
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x82,       // bEndpointAddress (IN, 2)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize (64)
    0x0A,       // bInterval (10ms)
    
    // Interface 2: Keyboard
    0x09,       // bLength
    0x04,       // bDescriptorType (Interface)
    0x02,       // bInterfaceNumber (2)
    0x00,       // bAlternateSetting
    0x01,       // bNumEndpoints (1)
    0x03,       // bInterfaceClass (HID)
    0x01,       // bInterfaceSubClass (Boot Interface)
    0x01,       // bInterfaceProtocol (Keyboard)
    0x00,       // iInterface (String Index)
    
    // HID Descriptor for Keyboard
    0x09,       // bLength
    0x21,       // bDescriptorType (HID)
    0x11, 0x01, // bcdHID (1.11)
    0x00,       // bCountryCode
    0x01,       // bNumDescriptors
    0x22,       // bDescriptorType (Report)
    0x00, 0x00, // wDescriptorLength (will be set at runtime)
    
    // Endpoint 3: Interrupt IN
    0x07,       // bLength
    0x05,       // bDescriptorType (Endpoint)
    0x83,       // bEndpointAddress (IN, 3)
    0x03,       // bmAttributes (Interrupt)
    0x40, 0x00, // wMaxPacketSize (64)
    0x0A        // bInterval (10ms)
};

// ============================================================================
// USB String Descriptors
// ============================================================================

const uint8_t USB_STRING_DESCRIPTOR_MANUFACTURER[] PROGMEM = {
    0x1A,       // bLength
    0x03,       // bDescriptorType (String)
    'A', 0x00, 'r', 0x00, 'd', 0x00, 'u', 0x00, 'i', 0x00, 'n', 0x00, 'o', 0x00, ' ', 0x00, 'L', 0x00, 'L', 0x00, 'C', 0x00
};

const uint8_t USB_STRING_DESCRIPTOR_PRODUCT[] PROGMEM = {
    0x1C,       // bLength
    0x03,       // bDescriptorType (String)
    'L', 0x00, 'a', 0x00, 't', 0x00, 't', 0x00, 'e', 0x00, 'D', 0x00, 'e', 0x00, 'c', 0x00, 'k', 0x00, ' ', 0x00, 'U', 0x00, 'P', 0x00, 'S', 0x00
};

const uint8_t USB_STRING_DESCRIPTOR_SERIAL[] PROGMEM = {
    0x1A,       // bLength
    0x03,       // bDescriptorType (String)
    'L', 0x00, 'A', 0x00, 'T', 0x00, 'T', 0x00, 'E', 0x00, 'D', 0x00, 'E', 0x00, 'C', 0x00, 'K', 0x00, '0', 0x00, '0', 0x00, '1', 0x00
};

// ============================================================================
// USB Descriptor Functions
// ============================================================================

// Get USB device descriptor
const uint8_t* getUSBDeviceDescriptor();
uint8_t getUSBDeviceDescriptorLength();

// Get USB configuration descriptor
const uint8_t* getUSBConfigurationDescriptor();
uint8_t getUSBConfigurationDescriptorLength();

// Get USB string descriptors
const uint8_t* getUSBStringDescriptor(uint8_t index);
uint8_t getUSBStringDescriptorLength(uint8_t index);

// Initialize USB descriptors
void initializeUSBDescriptors();

#endif // USB_DESCRIPTOR_H
