# LatteDeck Battery Service Makefile
# ==================================

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET = battery_service
SOURCE = battery_service.cpp

# Platform-specific libraries
ifeq ($(OS),Windows_NT)
    LIBS = -lsetupapi
    TARGET_EXT = .exe
else
    LIBS = -lpthread
    TARGET_EXT = 
endif

TARGET_FULL = $(TARGET)$(TARGET_EXT)

# Default target
all: $(TARGET_FULL)

# Build the service
$(TARGET_FULL): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET_FULL) $(SOURCE) $(LIBS)

# Install systemd service (Linux only)
install: $(TARGET_FULL)
	@echo "Installing LatteDeck Battery Service..."
	@sudo cp $(TARGET_FULL) /usr/local/bin/
	@sudo cp latte-deck-battery.service /etc/systemd/system/
	@sudo systemctl daemon-reload
	@sudo systemctl enable latte-deck-battery.service
	@echo "Service installed. Start with: sudo systemctl start latte-deck-battery.service"

# Uninstall systemd service
uninstall:
	@echo "Uninstalling LatteDeck Battery Service..."
	@sudo systemctl stop latte-deck-battery.service
	@sudo systemctl disable latte-deck-battery.service
	@sudo rm -f /etc/systemd/system/latte-deck-battery.service
	@sudo rm -f /usr/local/bin/$(TARGET_FULL)
	@sudo systemctl daemon-reload
	@echo "Service uninstalled"

# Clean build artifacts
clean:
	rm -f $(TARGET_FULL)

# Run the service (for testing)
run: $(TARGET_FULL)
	./$(TARGET_FULL) /dev/ttyACM0 115200

# Show service status
status:
	@systemctl status latte-deck-battery.service

# Show service logs
logs:
	@journalctl -u latte-deck-battery.service -f

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the battery service"
	@echo "  install  - Install as systemd service"
	@echo "  uninstall- Remove systemd service"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Run service for testing"
	@echo "  status   - Show service status"
	@echo "  logs     - Show service logs"
	@echo "  help     - Show this help"

.PHONY: all install uninstall clean run status logs help
