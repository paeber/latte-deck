#!/bin/bash
# LatteDeck Battery Service - Ubuntu Installation Script
# =====================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SERVICE_NAME="latte-deck-battery"
SERVICE_USER="latte-deck"
SERVICE_GROUP="latte-deck"
INSTALL_DIR="/opt/latte-deck"
BINARY_NAME="latte-deck-battery-service"
SERIAL_PORT="/dev/ttyACM0"
BAUD_RATE="115200"

echo -e "${BLUE}LatteDeck Battery Service - Ubuntu Installation${NC}"
echo "=================================================="

# Check if running as root
if [[ $EUID -ne 0 ]]; then
   echo -e "${RED}This script must be run as root (use sudo)${NC}"
   exit 1
fi

# Check if Rust is installed
if ! command -v cargo &> /dev/null; then
    echo -e "${YELLOW}Rust is not installed. Installing Rust...${NC}"
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source ~/.cargo/env
    export PATH="$HOME/.cargo/bin:$PATH"
fi

# Install required system packages
echo -e "${BLUE}Installing required system packages...${NC}"
apt update
apt install -y \
    build-essential \
    pkg-config \
    libudev-dev \
    libdbus-1-dev \
    libsystemd-dev \
    udev \
    upower \
    systemd

# Create service user and group
echo -e "${BLUE}Creating service user and group...${NC}"
if ! id "$SERVICE_USER" &>/dev/null; then
    useradd --system --no-create-home --shell /bin/false "$SERVICE_USER"
    usermod -a -G dialout "$SERVICE_USER"
    echo -e "${GREEN}Created user: $SERVICE_USER${NC}"
else
    echo -e "${YELLOW}User $SERVICE_USER already exists${NC}"
fi

# Create installation directory
echo -e "${BLUE}Creating installation directory...${NC}"
mkdir -p "$INSTALL_DIR"
chown "$SERVICE_USER:$SERVICE_GROUP" "$INSTALL_DIR"

# Build the service
echo -e "${BLUE}Building LatteDeck Battery Service...${NC}"
cd battery_service_rust

# Set environment variables for cross-compilation if needed
export RUSTFLAGS="-C target-cpu=native"

# Build in release mode
cargo build --release

# Install the binary
echo -e "${BLUE}Installing service binary...${NC}"
cp "target/release/$BINARY_NAME" "$INSTALL_DIR/"
chown "$SERVICE_USER:$SERVICE_GROUP" "$INSTALL_DIR/$BINARY_NAME"
chmod +x "$INSTALL_DIR/$BINARY_NAME"

# Create systemd service file
echo -e "${BLUE}Creating systemd service...${NC}"
cat > "/etc/systemd/system/$SERVICE_NAME.service" << EOF
[Unit]
Description=LatteDeck Battery Service
Documentation=https://github.com/your-repo/latte-deck
After=multi-user.target
Wants=multi-user.target

[Service]
Type=simple
User=$SERVICE_USER
Group=$SERVICE_GROUP
ExecStart=$INSTALL_DIR/$BINARY_NAME $SERIAL_PORT $BAUD_RATE
ExecReload=/bin/kill -HUP \$MAINPID
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal
SyslogIdentifier=$SERVICE_NAME

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

# Create udev rules for serial port access
echo -e "${BLUE}Creating udev rules...${NC}"
cat > "/etc/udev/rules.d/99-latte-deck-serial.rules" << EOF
# LatteDeck Serial Port Rules
# This ensures the LatteDeck device gets proper permissions

# Match Arduino Leonardo devices
SUBSYSTEM=="tty", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="8036", GROUP="dialout", MODE="0664"

# Match LattePanda devices (if different VID/PID)
SUBSYSTEM=="tty", ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", GROUP="dialout", MODE="0664"
EOF

# Reload udev rules
udevadm control --reload-rules
udevadm trigger

# Enable and start the service
echo -e "${BLUE}Enabling and starting service...${NC}"
systemctl daemon-reload
systemctl enable "$SERVICE_NAME"
systemctl start "$SERVICE_NAME"

# Check service status
echo -e "${BLUE}Checking service status...${NC}"
sleep 2
if systemctl is-active --quiet "$SERVICE_NAME"; then
    echo -e "${GREEN}Service is running successfully!${NC}"
else
    echo -e "${RED}Service failed to start. Checking logs...${NC}"
    journalctl -u "$SERVICE_NAME" --no-pager -l
    exit 1
fi

# Display service information
echo -e "${GREEN}Installation completed successfully!${NC}"
echo ""
echo -e "${BLUE}Service Information:${NC}"
echo "  Service Name: $SERVICE_NAME"
echo "  Binary Path: $INSTALL_DIR/$BINARY_NAME"
echo "  Serial Port: $SERIAL_PORT"
echo "  Baud Rate: $BAUD_RATE"
echo "  User: $SERVICE_USER"
echo ""
echo -e "${BLUE}Useful Commands:${NC}"
echo "  Check status: sudo systemctl status $SERVICE_NAME"
echo "  View logs: sudo journalctl -u $SERVICE_NAME -f"
echo "  Restart: sudo systemctl restart $SERVICE_NAME"
echo "  Stop: sudo systemctl stop $SERVICE_NAME"
echo "  Start: sudo systemctl start $SERVICE_NAME"
echo ""
echo -e "${BLUE}Battery Device Location:${NC}"
echo "  /sys/class/power_supply/latte_deck_battery/"
echo ""
echo -e "${YELLOW}Note: Make sure your LatteDeck is connected to $SERIAL_PORT${NC}"
echo -e "${YELLOW}If using a different port, edit the service file and restart${NC}"
