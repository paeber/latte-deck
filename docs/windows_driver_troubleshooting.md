# Windows Driver Troubleshooting Guide

## Problem: "This device cannot start. (Code 10)" Error

If you see this error in Windows Device Manager for the "USB Input Device" under your LattePanda Leonardo, follow these steps:

### Compiler Warnings (Normal)
You may see these warnings during compilation - they are normal and expected:
- `#warning "using legacy HID core (non pluggable)"` - This is normal for ATmega32U4 boards
- `#pragma message "Using default ASCII layout for keyboard modules"` - This is expected behavior

These warnings do not affect functionality and can be safely ignored.

### Step 1: Update USB Descriptor Information
The code has been updated to use standard Arduino Leonardo USB identifiers for better Windows driver compatibility:
- **Vendor ID**: 0x2341 (Arduino LLC)
- **Product ID**: 0x8036 (Arduino Leonardo)
- **Manufacturer**: "Arduino LLC"
- **Product**: "Arduino Leonardo"

### Step 2: Manual Driver Installation
1. **Open Device Manager** (Win+X → Device Manager)
2. **Find the problematic device** under "Human Interface Devices" or "Universal Serial Bus devices"
3. **Right-click** on the device with the error
4. **Select "Update driver"**
5. **Choose "Browse my computer for drivers"**
6. **Select "Let me pick from a list of available drivers on my computer"**
7. **Choose "USB Input Device"** or **"HID-compliant device"**
8. **Click "Next"** and **"Close"**

### Step 3: Alternative Driver Selection
If the above doesn't work:
1. **Right-click** on the device
2. **Select "Properties"**
3. **Go to "Driver" tab**
4. **Click "Update Driver"**
5. **Choose "Browse my computer for drivers"**
6. **Select "Have Disk"**
7. **Browse to**: `C:\Windows\System32\DriverStore\FileRepository`
8. **Look for**: `hidusb.inf` or `input.inf`
9. **Select the appropriate driver**

### Step 4: Device Manager Reset
1. **Unplug** the LattePanda Leonardo
2. **Open Device Manager**
3. **Go to "View" → "Show hidden devices"**
4. **Find any grayed-out entries** for the device
5. **Right-click and "Uninstall device"**
6. **Check "Delete the driver software"** if available
7. **Restart** your computer
8. **Reconnect** the LattePanda Leonardo

### Step 5: Windows HID Driver Installation
1. **Download** the latest Arduino IDE
2. **Install** Arduino IDE (this installs the necessary USB drivers)
3. **Connect** your LattePanda Leonardo
4. **Let Windows** automatically install drivers

### Step 6: Manual INF File Creation (Advanced)
If all else fails, you can create a custom INF file:

```inf
[Version]
Signature="$Windows NT$"
Class=HIDClass
ClassGuid={745a17a0-74d3-11d0-b6fe-00a0c90f57da}
Provider=%ManufacturerName%
CatalogFile=LatteDeck.cat
DriverVer=01/01/2025,1.0.0.0

[Manufacturer]
%ManufacturerName%=Standard,NT$ARCH$

[Standard.NT$ARCH$]
%LatteDeck.DeviceDesc%=LatteDeck_Device, USB\VID_2341&PID_8036

[LatteDeck_Device.NT]
CopyFiles=LatteDeck_CopyFiles

[LatteDeck_Device.NT.HW]
AddReg=LatteDeck_AddReg

[LatteDeck_Device.NT.Services]
AddService=HidUsb,0x00000002,HidUsb_ServiceInstall

[HidUsb_ServiceInstall]
DisplayName=%HidUsb.SvcDesc%
ServiceType=1
StartType=3
ErrorControl=1
ServiceBinary=%12%\HidUsb.sys

[LatteDeck_CopyFiles]
HidUsb.sys

[LatteDeck_AddReg]
HKR,,"LowerFilters",0x00010008,"HidUsb"

[Strings]
ManufacturerName="Arduino LLC"
LatteDeck.DeviceDesc="LatteDeck Composite HID Device"
HidUsb.SvcDesc="USB Input Device"
```

### Step 7: Verify Device Recognition
After driver installation, check:
1. **Device Manager** should show the device without errors
2. **Windows should recognize**:
   - A mouse device (for joystick input)
   - A keyboard device (for joystick input)
   - A power device (for UPS battery status)
3. **Test functionality**:
   - Move the right joystick (should move mouse cursor)
   - Press the right joystick button (should click)
   - Move the left joystick (should send keyboard keys)

### Common Issues and Solutions

**Issue**: Device shows as "Unknown Device"
**Solution**: Use Step 2 to manually assign HID-compliant device driver

**Issue**: Device shows but mouse/keyboard don't work
**Solution**: Check that the composite HID descriptor is properly loaded

**Issue**: Device works but battery status not reported
**Solution**: Verify Power Device HID reports are being sent

**Issue**: Intermittent connection issues
**Solution**: Try a different USB cable or port

### Testing the Device
1. **Open a text editor** (Notepad)
2. **Move the left joystick** - should type 'w', 'a', 's', 'd'
3. **Press the left joystick button** - should type space
4. **Move the right joystick** - should move mouse cursor
5. **Press the right joystick button** - should click
6. **Check battery status** in Windows power settings

If all steps work, the device is properly configured and Windows drivers are correctly assigned.
