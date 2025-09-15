# HID Interface Testing Guide

This guide provides step-by-step instructions to verify that all HID interfaces (Battery, Mouse, and Keyboard) are working properly after the composite HID implementation update.

## Prerequisites

- LattePanda Delta 3 with the latte-deck firmware uploaded
- Windows 10/11 computer for testing
- USB cable to connect the LattePanda to the computer

## Initial Connection

1. Connect the LattePanda Delta 3 to your Windows computer using a USB cable
2. Wait for Windows to detect the device (you should hear the USB connection sound)
3. Open Device Manager (Win+X → Device Manager)

## Verifying Device Recognition

### Step 1: Check Device Manager

In Device Manager, you should see the following devices appear without any error icons:

1. Under "Human Interface Devices":
   - HID-compliant mouse
   - HID-compliant keyboard
   - HID-compliant battery device (or "Battery" under "Batteries" section)

If any of these show a yellow warning icon or "Unknown Device", proceed to the troubleshooting section.

### Step 2: Verify Battery Interface

1. Open Windows Settings
2. Go to System → Power & battery
3. You should see a battery icon and percentage for the connected LattePanda
4. Click on "Battery usage" to see more details
5. The battery should be listed with its charge level and estimated runtime

**Expected result:** Windows recognizes the device as a battery and displays its charge level.

### Step 3: Verify Mouse Interface

1. Open Notepad or any text editor
2. Move the right joystick on the LattePanda
3. The mouse cursor should move in the corresponding direction
4. Press the right joystick button
5. This should register as a mouse click

**Expected result:** The joystick movement translates to mouse cursor movement, and pressing the joystick button registers as a mouse click.

### Step 4: Verify Keyboard Interface

1. With Notepad still open
2. Move the left joystick up, down, left, and right
3. This should type 'w', 's', 'a', and 'd' respectively
4. Press the left joystick button
5. This should type a space character

**Expected result:** The joystick movement types the corresponding keys, and pressing the joystick button types a space.

## Troubleshooting

### If Devices Show Error Code 10

1. Right-click on the device with the error
2. Select "Update driver"
3. Choose "Browse my computer for drivers"
4. Select "Let me pick from a list of available drivers on my computer"
5. For mouse/keyboard: Choose "HID-compliant mouse" or "HID-compliant keyboard"
6. For battery: Choose "HID-compliant battery" or "Battery device"
7. Click "Next" and follow the prompts

### If Some Interfaces Work But Others Don't

This indicates that the composite HID implementation is partially working. Try these steps:

1. **Reset the USB device**:
   - Disconnect the USB cable
   - Wait 10 seconds
   - Reconnect the USB cable

2. **Check Serial Monitor**:
   - Open Arduino IDE
   - Connect to the LattePanda's serial port
   - Set baud rate to 115200
   - Check for any error messages during initialization

3. **Verify Report Sending**:
   - For non-working interfaces, check if reports are being sent
   - Battery reports should be sent every 30 seconds or when battery level changes
   - Mouse/keyboard reports are sent in response to joystick movement

## Advanced Testing

### Testing Battery Status Updates

1. Disconnect external power from the LattePanda
2. Observe that Windows shows the battery is discharging
3. Reconnect power
4. Observe that Windows shows the battery is charging

### Testing Mouse Sensitivity

1. Move the right joystick slightly
2. The cursor should move slowly
3. Move the joystick to its maximum position
4. The cursor should move faster

### Testing Keyboard Key Combinations

1. Press multiple buttons simultaneously
2. Verify that all keypresses are registered correctly

## Reporting Issues

If you encounter issues with the HID interfaces, collect the following information:

1. Which interfaces are working and which are not
2. Any error codes or messages in Device Manager
3. Serial monitor output during device initialization
4. Windows version and any recent updates

Report this information to the project maintainers for further assistance.
