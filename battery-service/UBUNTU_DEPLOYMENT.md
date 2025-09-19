# LatteDeck Battery Service - Ubuntu Deployment Guide

This guide covers deploying the LatteDeck Battery Service on Ubuntu systems using Rust for optimal performance and system integration.

## Overview

The LatteDeck Battery Service is a high-performance background service that:
- Reads battery data from LatteDeck via serial communication
- Presents the battery as a native Ubuntu device
- Integrates with UPower for system tray battery display
- Uses minimal system resources (~2-8MB RAM)
- Provides reliable, crash-free operation

## Prerequisites

- Ubuntu 20.04 LTS or later
- LatteDeck hardware connected via USB
- Serial port access (typically `/dev/ttyACM0`)
- Root/sudo access for installation

## Quick Installation

### Automated Installation

```bash
# Clone the repository
git clone https://github.com/your-repo/latte-deck.git
cd latte-deck

# Run the installation script
sudo ./install_ubuntu.sh
```

The script will:
- Install Rust and required dependencies
- Build the service in release mode
- Create system user and permissions
- Install systemd service
- Configure udev rules
- Start the service automatically

### Manual Installation

#### 1. Install Dependencies

```bash
# Update package list
sudo apt update

# Install required packages
sudo apt install -y \
    build-essential \
    pkg-config \
    libudev-dev \
    libdbus-1-dev \
    libsystemd-dev \
    udev \
    upower \
    systemd

# Install Rust (if not already installed)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
source ~/.cargo/env
```

#### 2. Build the Service

```bash
cd battery_service_rust
cargo build --release
```

#### 3. Install the Service

```bash
# Create service user
sudo useradd --system --no-create-home --shell /bin/false latte-deck
sudo usermod -a -G dialout latte-deck

# Create installation directory
sudo mkdir -p /opt/latte-deck
sudo cp target/release/latte-deck-battery-service /opt/latte-deck/
sudo chown latte-deck:latte-deck /opt/latte-deck/latte-deck-battery-service
sudo chmod +x /opt/latte-deck/latte-deck-battery-service
```

#### 4. Configure Systemd Service

```bash
# Create service file
sudo tee /etc/systemd/system/latte-deck-battery.service > /dev/null << 'EOF'
[Unit]
Description=LatteDeck Battery Service
After=multi-user.target
Wants=multi-user.target

[Service]
Type=simple
User=latte-deck
Group=latte-deck
ExecStart=/opt/latte-deck/latte-deck-battery-service /dev/ttyACM0 115200
ExecReload=/bin/kill -HUP $MAINPID
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal
SyslogIdentifier=latte-deck-battery

# Security settings
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/sys/class/power_supply

# Resource limits
MemoryLimit=10M
CPUQuota=10%

# Environment
Environment=RUST_LOG=info

[Install]
WantedBy=multi-user.target
EOF

# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable latte-deck-battery
sudo systemctl start latte-deck-battery
```

#### 5. Configure Udev Rules

```bash
# Create udev rules for serial port access
sudo tee /etc/udev/rules.d/99-latte-deck-serial.rules > /dev/null << 'EOF'
# LatteDeck Serial Port Rules
SUBSYSTEM=="tty", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="8036", GROUP="dialout", MODE="0664"
SUBSYSTEM=="tty", ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", GROUP="dialout", MODE="0664"
EOF

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Service Management

### Basic Commands

```bash
# Check service status
sudo systemctl status latte-deck-battery

# View service logs
sudo journalctl -u latte-deck-battery -f

# Restart service
sudo systemctl restart latte-deck-battery

# Stop service
sudo systemctl stop latte-deck-battery

# Start service
sudo systemctl start latte-deck-battery

# Disable service (prevent auto-start)
sudo systemctl disable latte-deck-battery

# Enable service (allow auto-start)
sudo systemctl enable latte-deck-battery
```

### Log Analysis

```bash
# View recent logs
sudo journalctl -u latte-deck-battery --since "1 hour ago"

# View logs with timestamps
sudo journalctl -u latte-deck-battery -o short-iso

# Filter logs by priority
sudo journalctl -u latte-deck-battery -p err

# View logs from boot
sudo journalctl -u latte-deck-battery -b
```

## Battery Device Integration

### Device Location

The service creates a battery device at:
```
/sys/class/power_supply/latte_deck_battery/
```

### Device Files

| File | Description | Format |
|------|-------------|---------|
| `capacity` | Battery percentage (0-100) | Integer |
| `voltage_now` | Current voltage (microvolts) | Integer |
| `current_now` | Current current (microamps) | Integer |
| `status` | Charging status | String |
| `present` | Device present (1/0) | Integer |
| `health` | Battery health | String |
| `temp` | Temperature (tenths of °C) | Integer |
| `time_to_empty_now` | Time to empty (minutes) | Integer |
| `time_to_full_now` | Time to full (minutes) | Integer |
| `energy_full_design` | Design capacity (μWh) | Integer |
| `energy_full` | Current full capacity (μWh) | Integer |
| `energy_now` | Current energy (μWh) | Integer |

### System Integration

The battery device integrates with:

- **UPower**: System power management
- **GNOME Power Manager**: Desktop battery display
- **KDE Power Management**: KDE battery widget
- **System Tray**: Battery percentage and status
- **Power Settings**: Battery preferences and alerts

## Configuration

### Serial Port Configuration

To change the serial port or baud rate:

```bash
# Edit the service file
sudo systemctl edit latte-deck-battery

# Add override configuration
[Service]
ExecStart=
ExecStart=/opt/latte-deck/latte-deck-battery-service /dev/ttyUSB0 115200

# Reload and restart
sudo systemctl daemon-reload
sudo systemctl restart latte-deck-battery
```

### Log Level Configuration

```bash
# Edit the service file
sudo systemctl edit latte-deck-battery

# Add environment configuration
[Service]
Environment=RUST_LOG=debug

# Reload and restart
sudo systemctl daemon-reload
sudo systemctl restart latte-deck-battery
```

### Resource Limits

The service includes resource limits:
- **Memory Limit**: 10MB
- **CPU Quota**: 10%

To modify limits:

```bash
sudo systemctl edit latte-deck-battery

[Service]
MemoryLimit=20M
CPUQuota=20%
```

## Troubleshooting

### Common Issues

#### 1. Service Won't Start

```bash
# Check service status
sudo systemctl status latte-deck-battery

# View detailed logs
sudo journalctl -u latte-deck-battery --no-pager -l
```

**Common causes:**
- Serial port not found
- Permission denied
- Port already in use
- Missing dependencies

#### 2. Serial Port Issues

```bash
# List available serial ports
ls -la /dev/tty*

# Check port permissions
ls -la /dev/ttyACM0

# Test port access
sudo chmod 666 /dev/ttyACM0
```

#### 3. Battery Device Not Appearing

```bash
# Check if device directory exists
ls -la /sys/class/power_supply/latte_deck_battery/

# Check service logs for errors
sudo journalctl -u latte-deck-battery | grep -i error

# Verify udev rules
sudo udevadm test /sys/class/power_supply/latte_deck_battery
```

#### 4. UPower Integration Issues

```bash
# Check UPower status
systemctl status upower

# Restart UPower
sudo systemctl restart upower

# Check D-Bus connection
dbus-send --system --dest=org.freedesktop.UPower --type=method_call --print-reply /org/freedesktop/UPower org.freedesktop.UPower.EnumerateDevices
```

### Debug Mode

Enable debug logging:

```bash
# Edit service configuration
sudo systemctl edit latte-deck-battery

[Service]
Environment=RUST_LOG=debug

# Reload and restart
sudo systemctl daemon-reload
sudo systemctl restart latte-deck-battery

# Monitor debug output
sudo journalctl -u latte-deck-battery -f
```

### Manual Testing

Test the service manually:

```bash
# Run service in foreground
sudo -u latte-deck /opt/latte-deck/latte-deck-battery-service /dev/ttyACM0 115200

# Test serial communication
sudo minicom -D /dev/ttyACM0 -b 115200
```

## Performance

### Resource Usage

Typical resource usage:
- **Memory**: 2-8MB
- **CPU**: <1% (idle), 2-5% (active)
- **Disk I/O**: Minimal (only log writes)

### Optimization

For better performance:

```bash
# Set CPU governor to performance
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Increase serial buffer size
echo 4096 | sudo tee /sys/module/usbserial/parameters/buffer_size
```

## Security

### Service Security

The service runs with:
- **No new privileges**: Cannot escalate privileges
- **Private tmp**: Isolated temporary directory
- **Protected system**: Read-only system directories
- **Protected home**: No access to user home directories
- **Limited write access**: Only to power supply directory

### Network Security

The service:
- **No network access**: Completely offline
- **No external dependencies**: Self-contained
- **Local only**: Serial communication only

## Uninstallation

### Complete Removal

```bash
# Stop and disable service
sudo systemctl stop latte-deck-battery
sudo systemctl disable latte-deck-battery

# Remove service file
sudo rm /etc/systemd/system/latte-deck-battery.service

# Remove udev rules
sudo rm /etc/udev/rules.d/99-latte-deck-serial.rules

# Remove installation
sudo rm -rf /opt/latte-deck

# Remove service user
sudo userdel latte-deck

# Reload systemd
sudo systemctl daemon-reload

# Reload udev
sudo udevadm control --reload-rules
```

## Support

### Getting Help

1. **Check logs**: `sudo journalctl -u latte-deck-battery -f`
2. **Verify hardware**: Ensure LatteDeck is connected and working
3. **Test serial**: Use `minicom` or `screen` to test serial communication
4. **Check permissions**: Verify user has access to serial port

### Reporting Issues

When reporting issues, include:
- Ubuntu version: `lsb_release -a`
- Service logs: `sudo journalctl -u latte-deck-battery --no-pager`
- Hardware info: `lsusb` and `ls -la /dev/tty*`
- Service status: `sudo systemctl status latte-deck-battery`

## License

This service is licensed under the MIT License. See the LICENSE file for details.
