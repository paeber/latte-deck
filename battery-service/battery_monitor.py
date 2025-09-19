#!/usr/bin/env python3
"""
LatteDeck Battery Monitor
========================

A Python script to monitor battery status from LatteDeck via serial communication.
Parses JSON data from the Arduino and provides battery monitoring functionality.

Usage:
    python3 battery_monitor.py [serial_port] [baud_rate]

Examples:
    python3 battery_monitor.py /dev/ttyACM0 115200
    python3 battery_monitor.py COM3 115200
"""

import serial
import json
import sys
import time
import argparse
from datetime import datetime
from typing import Dict, Any, Optional

class BatteryMonitor:
    def __init__(self, port: str, baud_rate: int = 115200):
        """Initialize the battery monitor with serial connection."""
        self.port = port
        self.baud_rate = baud_rate
        self.serial_conn: Optional[serial.Serial] = None
        self.last_data: Optional[Dict[str, Any]] = None
        
    def connect(self) -> bool:
        """Connect to the serial port."""
        try:
            self.serial_conn = serial.Serial(
                port=self.port,
                baudrate=self.baud_rate,
                timeout=1.0,
                write_timeout=1.0
            )
            print(f"Connected to {self.port} at {self.baud_rate} baud")
            return True
        except serial.SerialException as e:
            print(f"Failed to connect to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from the serial port."""
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
            print("Disconnected from serial port")
    
    def read_battery_data(self) -> Optional[Dict[str, Any]]:
        """Read and parse battery data from serial."""
        if not self.serial_conn or not self.serial_conn.is_open:
            return None
        
        try:
            # Read line from serial
            line = self.serial_conn.readline().decode('utf-8').strip()
            
            # Look for JSON data
            if line.startswith("UPS_JSON:"):
                json_str = line[9:]  # Remove "UPS_JSON:" prefix
                data = json.loads(json_str)
                
                # Add timestamp for display
                data['received_at'] = datetime.now().isoformat()
                self.last_data = data
                return data
                
        except (json.JSONDecodeError, UnicodeDecodeError) as e:
            print(f"Error parsing data: {e}")
        except serial.SerialException as e:
            print(f"Serial error: {e}")
        
        return None
    
    def format_battery_status(self, data: Dict[str, Any]) -> str:
        """Format battery data for display."""
        if not data:
            return "No data available"
        
        # Calculate time since last update
        last_update = data.get('last_update_ms', 0)
        current_time = time.time() * 1000
        age_ms = int(current_time - last_update) if last_update > 0 else 0
        
        # Format charging status
        charging_status = "🔋 Charging" if data.get('is_charging', False) else "🔌 Discharging"
        connected_status = "✅ Connected" if data.get('is_connected', False) else "❌ Disconnected"
        
        # Format voltage
        voltage = data.get('voltage_mV', 0)
        voltage_str = f"{voltage/1000:.2f}V" if voltage > 0 else "N/A"
        
        # Format current
        current = data.get('current_mA', 0)
        current_str = f"{current:+d}mA" if current != 0 else "0mA"
        
        # Format capacity with color coding
        capacity = data.get('capacity_percent', 0)
        if capacity >= 80:
            capacity_color = "🟢"
        elif capacity >= 50:
            capacity_color = "🟡"
        elif capacity >= 20:
            capacity_color = "🟠"
        else:
            capacity_color = "🔴"
        
        # Format temperature
        temp = data.get('temperature_celsius', 0)
        temp_str = f"{temp}°C" if temp > 0 else "N/A"
        
        # Format failures
        failures = data.get('consecutive_failures', 0)
        failure_str = f" ({failures} failures)" if failures > 0 else ""
        
        return f"""
{capacity_color} Battery: {capacity}% | {voltage_str} | {current_str} | {temp_str}
{charging_status} | {connected_status}{failure_str}
Last update: {age_ms}ms ago | Received: {data.get('received_at', 'N/A')}
"""
    
    def monitor_continuous(self, update_interval: float = 1.0):
        """Continuously monitor battery status."""
        print("Starting continuous battery monitoring...")
        print("Press Ctrl+C to stop")
        print("-" * 60)
        
        try:
            while True:
                data = self.read_battery_data()
                if data:
                    # Clear screen and print status
                    print("\033[2J\033[H", end="")  # Clear screen
                    print(f"LatteDeck Battery Monitor - {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
                    print("-" * 60)
                    print(self.format_battery_status(data))
                    print("-" * 60)
                    print("Press Ctrl+C to stop")
                else:
                    time.sleep(update_interval)
                    
        except KeyboardInterrupt:
            print("\nMonitoring stopped by user")
        except Exception as e:
            print(f"Error during monitoring: {e}")
    
    def get_single_reading(self) -> Optional[Dict[str, Any]]:
        """Get a single battery reading."""
        print("Waiting for battery data...")
        timeout = 10  # 10 second timeout
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            data = self.read_battery_data()
            if data:
                return data
            time.sleep(0.1)
        
        print("Timeout waiting for data")
        return None

def main():
    parser = argparse.ArgumentParser(description="LatteDeck Battery Monitor")
    parser.add_argument("port", help="Serial port (e.g., /dev/ttyACM0, COM3)")
    parser.add_argument("-b", "--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("-s", "--single", action="store_true", help="Get single reading and exit")
    parser.add_argument("-i", "--interval", type=float, default=1.0, help="Update interval in seconds (default: 1.0)")
    
    args = parser.parse_args()
    
    # Create monitor instance
    monitor = BatteryMonitor(args.port, args.baud)
    
    # Connect to serial port
    if not monitor.connect():
        sys.exit(1)
    
    try:
        if args.single:
            # Single reading mode
            data = monitor.get_single_reading()
            if data:
                print(monitor.format_battery_status(data))
            else:
                print("No data received")
        else:
            # Continuous monitoring mode
            monitor.monitor_continuous(args.interval)
    finally:
        monitor.disconnect()

if __name__ == "__main__":
    main()
