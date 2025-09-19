#!/bin/bash
# LatteDeck Battery Service - Ubuntu Test Script
# =============================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

SERVICE_NAME="latte-deck-battery"
BATTERY_PATH="/sys/class/power_supply/latte_deck_battery"

echo -e "${BLUE}LatteDeck Battery Service - Ubuntu Test${NC}"
echo "=========================================="

# Check if running as root
if [[ $EUID -ne 0 ]]; then
   echo -e "${RED}This script must be run as root (use sudo)${NC}"
   exit 1
fi

# Test 1: Service Status
echo -e "${BLUE}Test 1: Service Status${NC}"
if systemctl is-active --quiet "$SERVICE_NAME"; then
    echo -e "${GREEN}✓ Service is running${NC}"
else
    echo -e "${RED}✗ Service is not running${NC}"
    echo "Service status:"
    systemctl status "$SERVICE_NAME" --no-pager -l
    exit 1
fi

# Test 2: Battery Device Directory
echo -e "${BLUE}Test 2: Battery Device Directory${NC}"
if [ -d "$BATTERY_PATH" ]; then
    echo -e "${GREEN}✓ Battery device directory exists${NC}"
    echo "Device files:"
    ls -la "$BATTERY_PATH"
else
    echo -e "${RED}✗ Battery device directory not found${NC}"
    echo "Expected: $BATTERY_PATH"
    exit 1
fi

# Test 3: Battery Device Files
echo -e "${BLUE}Test 3: Battery Device Files${NC}"
required_files=("capacity" "voltage_now" "current_now" "status" "present" "health")
for file in "${required_files[@]}"; do
    if [ -f "$BATTERY_PATH/$file" ]; then
        echo -e "${GREEN}✓ $file exists${NC}"
    else
        echo -e "${RED}✗ $file missing${NC}"
    fi
done

# Test 4: Battery Data Reading
echo -e "${BLUE}Test 4: Battery Data Reading${NC}"
if [ -f "$BATTERY_PATH/capacity" ]; then
    capacity=$(cat "$BATTERY_PATH/capacity")
    echo "Battery capacity: $capacity%"
    
    if [ "$capacity" -ge 0 ] && [ "$capacity" -le 100 ]; then
        echo -e "${GREEN}✓ Capacity value is valid${NC}"
    else
        echo -e "${YELLOW}⚠ Capacity value seems invalid: $capacity%${NC}"
    fi
else
    echo -e "${RED}✗ Cannot read capacity${NC}"
fi

# Test 5: Service Logs
echo -e "${BLUE}Test 5: Service Logs${NC}"
echo "Recent service logs:"
journalctl -u "$SERVICE_NAME" --since "5 minutes ago" --no-pager -l | tail -10

# Test 6: UPower Integration
echo -e "${BLUE}Test 6: UPower Integration${NC}"
if command -v upower &> /dev/null; then
    echo "UPower devices:"
    upower -e | grep -i latte || echo "No LatteDeck devices found in UPower"
else
    echo -e "${YELLOW}⚠ UPower not available${NC}"
fi

# Test 7: Serial Port Access
echo -e "${BLUE}Test 7: Serial Port Access${NC}"
serial_ports=("/dev/ttyACM0" "/dev/ttyUSB0" "/dev/ttyACM1")
found_port=""
for port in "${serial_ports[@]}"; do
    if [ -e "$port" ]; then
        echo -e "${GREEN}✓ Found serial port: $port${NC}"
        found_port="$port"
        break
    fi
done

if [ -z "$found_port" ]; then
    echo -e "${YELLOW}⚠ No common serial ports found${NC}"
    echo "Available serial devices:"
    ls -la /dev/tty* | grep -E "(ACM|USB)" || echo "No USB serial devices found"
fi

# Test 8: Service Resource Usage
echo -e "${BLUE}Test 8: Service Resource Usage${NC}"
echo "Service resource usage:"
systemctl show "$SERVICE_NAME" --property=MemoryCurrent,CPUUsageNSec --no-pager

# Test 9: Service Dependencies
echo -e "${BLUE}Test 9: Service Dependencies${NC}"
echo "Service dependencies:"
systemctl list-dependencies "$SERVICE_NAME" --no-pager

# Test 10: Battery Status Summary
echo -e "${BLUE}Test 10: Battery Status Summary${NC}"
if [ -d "$BATTERY_PATH" ]; then
    echo "Current battery status:"
    echo "  Capacity: $(cat "$BATTERY_PATH/capacity" 2>/dev/null || echo "N/A")%"
    echo "  Voltage: $(cat "$BATTERY_PATH/voltage_now" 2>/dev/null || echo "N/A") μV"
    echo "  Current: $(cat "$BATTERY_PATH/current_now" 2>/dev/null || echo "N/A") μA"
    echo "  Status: $(cat "$BATTERY_PATH/status" 2>/dev/null || echo "N/A")"
    echo "  Present: $(cat "$BATTERY_PATH/present" 2>/dev/null || echo "N/A")"
    echo "  Health: $(cat "$BATTERY_PATH/health" 2>/dev/null || echo "N/A")"
fi

echo ""
echo -e "${GREEN}Test completed!${NC}"
echo ""
echo -e "${BLUE}Useful Commands:${NC}"
echo "  View logs: sudo journalctl -u $SERVICE_NAME -f"
echo "  Restart: sudo systemctl restart $SERVICE_NAME"
echo "  Check status: sudo systemctl status $SERVICE_NAME"
echo "  Battery info: cat $BATTERY_PATH/capacity"
