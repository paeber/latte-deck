#!/usr/bin/env python3
"""
LatteDeck Battery System Integration
===================================

Example script showing how to integrate LatteDeck battery monitoring
into system-level monitoring and notifications.

This script demonstrates:
- Reading battery data from LatteDeck
- System notifications (Linux/Windows)
- Logging battery status
- Low battery warnings
- Integration with system power management
"""

import serial
import json
import time
import logging
import platform
import subprocess
from datetime import datetime
from typing import Dict, Any, Optional

class BatterySystemIntegration:
    def __init__(self, port: str, baud_rate: int = 115200):
        self.port = port
        self.baud_rate = baud_rate
        self.serial_conn: Optional[serial.Serial] = None
        self.last_notification_time = 0
        self.notification_cooldown = 300  # 5 minutes between notifications
        
        # Setup logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('battery_monitor.log'),
                logging.StreamHandler()
            ]
        )
        self.logger = logging.getLogger(__name__)
    
    def connect(self) -> bool:
        """Connect to the serial port."""
        try:
            self.serial_conn = serial.Serial(
                port=self.port,
                baudrate=self.baud_rate,
                timeout=1.0
            )
            self.logger.info(f"Connected to LatteDeck at {self.port}")
            return True
        except serial.SerialException as e:
            self.logger.error(f"Failed to connect: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from serial port."""
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
            self.logger.info("Disconnected from LatteDeck")
    
    def read_battery_data(self) -> Optional[Dict[str, Any]]:
        """Read battery data from LatteDeck."""
        if not self.serial_conn or not self.serial_conn.is_open:
            return None
        
        try:
            line = self.serial_conn.readline().decode('utf-8').strip()
            if line.startswith("UPS_JSON:"):
                json_str = line[9:]
                data = json.loads(json_str)
                data['received_at'] = datetime.now().isoformat()
                return data
        except (json.JSONDecodeError, UnicodeDecodeError, serial.SerialException) as e:
            self.logger.debug(f"Error reading data: {e}")
        
        return None
    
    def send_system_notification(self, title: str, message: str, urgency: str = "normal"):
        """Send system notification (Linux/Windows)."""
        current_time = time.time()
        
        # Rate limiting
        if current_time - self.last_notification_time < self.notification_cooldown:
            return
        
        self.last_notification_time = current_time
        
        try:
            if platform.system() == "Linux":
                # Linux notification using notify-send
                subprocess.run([
                    "notify-send", 
                    f"--urgency={urgency}",
                    title, 
                    message
                ], check=True)
            elif platform.system() == "Windows":
                # Windows notification using PowerShell
                ps_script = f'''
                Add-Type -AssemblyName System.Windows.Forms
                $notification = New-Object System.Windows.Forms.NotifyIcon
                $notification.Icon = [System.Drawing.SystemIcons]::Information
                $notification.BalloonTipTitle = "{title}"
                $notification.BalloonTipText = "{message}"
                $notification.Visible = $true
                $notification.ShowBalloonTip(5000)
                '''
                subprocess.run(["powershell", "-Command", ps_script], check=True)
            
            self.logger.info(f"Notification sent: {title} - {message}")
            
        except subprocess.CalledProcessError as e:
            self.logger.error(f"Failed to send notification: {e}")
        except FileNotFoundError:
            self.logger.warning("Notification system not available")
    
    def check_battery_status(self, data: Dict[str, Any]):
        """Check battery status and send notifications if needed."""
        capacity = data.get('capacity_percent', 0)
        is_charging = data.get('is_charging', False)
        is_connected = data.get('is_connected', False)
        voltage = data.get('voltage_mV', 0)
        current = data.get('current_mA', 0)
        
        # Log battery status
        self.logger.info(
            f"Battery: {capacity}% | {voltage/1000:.2f}V | {current:+d}mA | "
            f"{'Charging' if is_charging else 'Discharging'} | "
            f"{'Connected' if is_connected else 'Disconnected'}"
        )
        
        # Low battery warnings
        if not is_charging and capacity <= 20:
            if capacity <= 10:
                self.send_system_notification(
                    "🔴 Critical Battery",
                    f"LatteDeck battery critically low: {capacity}%",
                    "critical"
                )
            elif capacity <= 15:
                self.send_system_notification(
                    "🟠 Low Battery",
                    f"LatteDeck battery low: {capacity}%",
                    "normal"
                )
        
        # Charging status notifications
        if is_charging and capacity >= 95:
            self.send_system_notification(
                "🔋 Battery Charged",
                f"LatteDeck battery fully charged: {capacity}%",
                "low"
            )
        
        # Connection issues
        if not is_connected:
            self.send_system_notification(
                "⚠️ UPS Disconnected",
                "LatteDeck UPS module disconnected",
                "normal"
            )
    
    def update_system_power_status(self, data: Dict[str, Any]):
        """Update system power status based on battery data."""
        capacity = data.get('capacity_percent', 0)
        is_charging = data.get('is_charging', False)
        
        # Create a simple status file for other scripts to read
        status_file = "latte_deck_battery_status.json"
        try:
            with open(status_file, 'w') as f:
                json.dump({
                    'capacity_percent': capacity,
                    'is_charging': is_charging,
                    'voltage_mV': data.get('voltage_mV', 0),
                    'current_mA': data.get('current_mA', 0),
                    'timestamp': data.get('received_at', ''),
                    'status': 'charging' if is_charging else 'discharging'
                }, f, indent=2)
        except Exception as e:
            self.logger.error(f"Failed to write status file: {e}")
    
    def monitor(self):
        """Main monitoring loop."""
        self.logger.info("Starting LatteDeck battery system integration")
        
        try:
            while True:
                data = self.read_battery_data()
                if data:
                    self.check_battery_status(data)
                    self.update_system_power_status(data)
                else:
                    time.sleep(1)
                    
        except KeyboardInterrupt:
            self.logger.info("Monitoring stopped by user")
        except Exception as e:
            self.logger.error(f"Error during monitoring: {e}")
        finally:
            self.disconnect()

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="LatteDeck Battery System Integration")
    parser.add_argument("port", help="Serial port (e.g., /dev/ttyACM0, COM3)")
    parser.add_argument("-b", "--baud", type=int, default=115200, help="Baud rate")
    
    args = parser.parse_args()
    
    integration = BatterySystemIntegration(args.port, args.baud)
    
    if integration.connect():
        integration.monitor()
    else:
        print("Failed to connect to LatteDeck")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
