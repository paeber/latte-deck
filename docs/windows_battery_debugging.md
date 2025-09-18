# Windows Battery Debugging Guide

## Overview

This guide provides comprehensive instructions for debugging UPS battery reporting to Windows and verifying that the HID Power Device is working correctly.

## Debug Output Analysis

### Serial Monitor Debug Messages

When `DEBUG_PRINT_UPS` is enabled, you'll see detailed debug output in the Serial Monitor. Look for these key messages:

#### Initialization Phase
```
UPS HID: Creating UPSPowerDevice instance...
UPS HID: Calling power_device->begin()...
UPS HID: Setting up string features...
UPS HID: Setting up Power Device features...
UPS HID: Initializing status LED...
UPS HID: Initialization completed - ready for reports
```

#### Reporting Phase
```
UPS HID: Checking if report needed - Last report: 5000ms ago, Capacity changed: No
UPS HID: 5 second interval reached - sending
UPS HID: Starting HID report transmission...
UPS HID: Present status updated: 0x1A
UPS HID: Sending remaining capacity: 85%
UPS HID: Remaining capacity report result: 1
UPS HID: Sending present status: 0x1A
UPS HID: Present status report result: 1
UPS HID: HID report transmission completed
```

### Troubleshooting Debug Output

**If you see blocking behavior:**
1. Check if debug output stops at a specific message
2. Look for negative return values from `sendReport()` calls
3. Monitor for repeated "No report needed - skipping" messages

**Common issues:**
- **Stops at "Calling power_device->begin()"**: USB HID initialization problem
- **Negative report results**: USB communication failure
- **No debug output**: `DEBUG_PRINT_UPS` not enabled or Serial not connected

## Windows Battery Verification Methods

### Method 1: Device Manager

1. **Open Device Manager:**
   - Press `Win + X` and select "Device Manager"
   - Or press `Win + R`, type `devmgmt.msc`, press Enter

2. **Check Battery Section:**
   - Expand "Batteries" section
   - Look for "HID UPS Battery" or similar entry
   - Right-click and select "Properties"

3. **Verify Device Status:**
   - Should show "This device is working properly"
   - Check "Device status" in the General tab
   - Look for any error codes or warnings

### Method 2: PowerShell Battery Query

1. **Open PowerShell as Administrator:**
   - Press `Win + X` and select "Windows PowerShell (Admin)"

2. **Run Battery Query:**
   ```powershell
   Get-CimInstance -ClassName Win32_Battery | Select-Object Name, BatteryStatus, EstimatedChargeRemaining, EstimatedRunTime, PowerManagementCapabilities
   ```

3. **Expected Output:**
   ```
   Name                : HID UPS Battery
   BatteryStatus       : 2 (Discharging) or 1 (Charging)
   EstimatedChargeRemaining : 85
   EstimatedRunTime    : 120
   PowerManagementCapabilities : {1, 2, 3, 4, 5, 6}
   ```

4. **Alternative Query (More Detailed):**
   ```powershell
   Get-CimInstance -ClassName Win32_Battery | Format-List *
   ```

### Method 3: Windows Battery Settings

1. **Open Settings:**
   - Press `Win + I`
   - Go to "System" → "Power & battery"

2. **Check Battery Status:**
   - Should show UPS battery in the list
   - Verify charge percentage matches your UPS
   - Check if charging/discharging status is correct

### Method 4: System Tray Battery Icon

1. **Check System Tray:**
   - Look for battery icon in bottom-right corner
   - Hover over icon to see percentage and status
   - Click to see detailed battery information

2. **Expected Behavior:**
   - Shows UPS battery percentage
   - Indicates charging/discharging status
   - Updates in real-time (every 5 seconds)

### Method 5: Event Viewer Logs

1. **Open Event Viewer:**
   - Press `Win + R`, type `eventvwr.msc`, press Enter

2. **Check System Logs:**
   - Navigate to "Windows Logs" → "System"
   - Look for entries related to "Battery" or "Power"
   - Check for any error messages

3. **Check Application Logs:**
   - Navigate to "Windows Logs" → "Application"
   - Look for HID-related entries

### Method 6: USB Device Tree

1. **Open Device Manager:**
   - Press `Win + X` and select "Device Manager"

2. **Check USB Devices:**
   - Expand "Universal Serial Bus controllers"
   - Look for your Arduino Leonardo device
   - Verify it shows as "Composite USB Device"

3. **Check Human Interface Devices:**
   - Expand "Human Interface Devices"
   - Look for "USB Input Device" entries
   - Should show multiple entries for Mouse, Keyboard, and Power Device

## Advanced Debugging

### PowerShell Real-time Monitoring

Create a PowerShell script to monitor battery status in real-time:

```powershell
# Save as monitor_battery.ps1
while ($true) {
    Clear-Host
    Write-Host "UPS Battery Status - $(Get-Date)" -ForegroundColor Green
    Write-Host "================================" -ForegroundColor Green
    
    $battery = Get-CimInstance -ClassName Win32_Battery
    if ($battery) {
        Write-Host "Name: $($battery.Name)"
        Write-Host "Status: $($battery.BatteryStatus)"
        Write-Host "Charge: $($battery.EstimatedChargeRemaining)%"
        Write-Host "Runtime: $($battery.EstimatedRunTime) minutes"
        Write-Host "Voltage: $($battery.DesignVoltage) mV"
    } else {
        Write-Host "No battery detected!" -ForegroundColor Red
    }
    
    Start-Sleep -Seconds 2
}
```

Run with: `.\monitor_battery.ps1`

### Registry Monitoring

1. **Open Registry Editor:**
   - Press `Win + R`, type `regedit`, press Enter

2. **Navigate to Battery Info:**
   ```
   HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Power\PowerSettings\238c9fa8-0aad-41ed-83f4-97be242c8f20\29f6c1db-86da-48c5-9fdb-f2b67b1f44da
   ```

3. **Check for UPS-related entries**

## Troubleshooting Common Issues

### Issue: Battery Not Detected

**Symptoms:**
- No battery icon in system tray
- PowerShell query returns no results
- Device Manager shows no battery devices

**Solutions:**
1. Check if `DEBUG_PRINT_UPS` is enabled
2. Verify USB connection is stable
3. Check if HID Power Device is properly initialized
4. Restart Windows Power service: `net stop power && net start power`

### Issue: Incorrect Battery Percentage

**Symptoms:**
- Battery percentage doesn't match actual UPS charge
- Percentage doesn't update

**Solutions:**
1. Check debug output for correct voltage readings
2. Verify SoC calculation in `ups_common.cpp`
3. Check if battery calibration is needed

### Issue: Charging Status Incorrect

**Symptoms:**
- Shows charging when discharging or vice versa
- Status doesn't change when plugging/unplugging

**Solutions:**
1. Check debug output for current readings
2. Verify charging detection logic in `ups_core.cpp`
3. Check present status bit flags

### Issue: HID Reports Failing

**Symptoms:**
- Negative return values in debug output
- "No report needed - skipping" messages only

**Solutions:**
1. Check USB connection stability
2. Verify HID descriptor is correct
3. Check for USB bandwidth issues
4. Try different USB port

## Performance Monitoring

### Serial Monitor Timing

Monitor the timing of debug messages to identify blocking:

```
UPS HID: Starting HID report transmission...
[If this message appears but next doesn't, blocking occurs here]
UPS HID: Present status updated: 0x1A
```

### USB Bandwidth Check

1. **Open Device Manager**
2. **Check USB Root Hub Properties**
3. **Monitor bandwidth usage**
4. **Ensure sufficient bandwidth for HID reports**

## Expected Behavior

### Normal Operation
- Debug messages every 5 seconds
- Battery percentage updates in Windows
- System tray icon shows correct status
- PowerShell queries return valid data

### Error Conditions
- Debug messages stop appearing
- Negative return values from HID functions
- Windows shows "Unknown battery" or no battery
- Device Manager shows error codes

## Contact Information

If issues persist after following this guide:
1. Check the main project documentation
2. Review the troubleshooting guide
3. Check GitHub issues for similar problems
4. Provide debug output and Windows version when reporting issues
