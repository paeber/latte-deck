/*
 * Ubuntu-specific battery integration
 * ==================================
 * 
 * This module handles Ubuntu-specific battery device integration using:
 * - UPower D-Bus interface for system integration
 * - /sys/class/power_supply/ for battery device files
 * - systemd for service management
 */

use anyhow::{Context, Result};
use log::{debug, info, warn};
use std::path::Path;
use tokio::fs;
use zbus::{Connection, dbus_interface};

const BATTERY_DEVICE_PATH: &str = "/sys/class/power_supply/latte_deck_battery";
const UDEV_RULES_PATH: &str = "/etc/udev/rules.d/99-latte-deck-battery.rules";

pub struct UbuntuBatteryManager {
    connection: Option<Connection>,
    device_path: String,
}

impl UbuntuBatteryManager {
    pub fn new() -> Self {
        Self {
            connection: None,
            device_path: BATTERY_DEVICE_PATH.to_string(),
        }
    }

    pub async fn initialize(&mut self) -> Result<()> {
        info!("Initializing Ubuntu battery integration");
        
        // Create battery device directory
        self.create_battery_device().await?;
        
        // Setup UPower D-Bus connection
        self.setup_upower_connection().await?;
        
        // Create udev rules for device recognition
        self.create_udev_rules().await?;
        
        info!("Ubuntu battery integration initialized successfully");
        Ok(())
    }

    async fn create_battery_device(&self) -> Result<()> {
        debug!("Creating battery device at {}", self.device_path);
        
        // Create battery device directory
        fs::create_dir_all(&self.device_path).await
            .context("Failed to create battery device directory")?;

        // Create required device files
        let device_files = [
            ("type", "Battery"),
            ("device_name", "LatteDeck UPS"),
            ("manufacturer", "LatteDeck"),
            ("model_name", "UPS Battery"),
            ("technology", "Li-ion"),
            ("scope", "System"),
            ("serial_number", "LATTEDECK001"),
        ];

        for (filename, content) in &device_files {
            let file_path = format!("{}/{}", self.device_path, filename);
            fs::write(&file_path, *content).await
                .with_context(|| format!("Failed to create {}", filename))?;
        }

        // Set proper permissions
        self.set_device_permissions().await?;
        
        info!("Battery device created at {}", self.device_path);
        Ok(())
    }

    async fn set_device_permissions(&self) -> Result<()> {
        use nix::sys::stat;
        use nix::unistd;
        
        // Set directory permissions (755)
        let mode = stat::Mode::from_bits_truncate(0o755);
        nix::unistd::chmod(Path::new(&self.device_path), mode)
            .context("Failed to set directory permissions")?;

        // Set file permissions (644)
        let file_mode = stat::Mode::from_bits_truncate(0o644);
        let files = ["type", "device_name", "manufacturer", "model_name", "technology", "scope", "serial_number"];
        
        for file in &files {
            let file_path = format!("{}/{}", self.device_path, file);
            nix::unistd::chmod(Path::new(&file_path), file_mode)
                .with_context(|| format!("Failed to set permissions for {}", file))?;
        }

        Ok(())
    }

    async fn setup_upower_connection(&mut self) -> Result<()> {
        debug!("Setting up UPower D-Bus connection");
        
        // Connect to system D-Bus
        let connection = Connection::system()
            .await
            .context("Failed to connect to system D-Bus")?;
        
        self.connection = Some(connection);
        info!("UPower D-Bus connection established");
        Ok(())
    }

    async fn create_udev_rules(&self) -> Result<()> {
        debug!("Creating udev rules for device recognition");
        
        let udev_rule = format!(
            "# LatteDeck Battery Device Rules
# This file ensures the LatteDeck battery device is properly recognized
# by UPower and other system components

# Match LatteDeck battery device
SUBSYSTEM==\"power_supply\", ATTR{{device_name}}==\"LatteDeck UPS\", ENV{{POWER_SUPPLY_NAME}}=\"latte_deck_battery\"

# Set device properties for UPower
SUBSYSTEM==\"power_supply\", ATTR{{device_name}}==\"LatteDeck UPS\", ENV{{POWER_SUPPLY_TYPE}}=\"Battery\"
SUBSYSTEM==\"power_supply\", ATTR{{device_name}}==\"LatteDeck UPS\", ENV{{POWER_SUPPLY_SCOPE}}=\"System\"
SUBSYSTEM==\"power_supply\", ATTR{{device_name}}==\"LatteDeck UPS\", ENV{{POWER_SUPPLY_TECHNOLOGY}}=\"Li-ion\"
"
        );

        fs::write(UDEV_RULES_PATH, udev_rule).await
            .context("Failed to create udev rules")?;

        // Set proper permissions for udev rules
        use nix::sys::stat;
        use nix::unistd;
        let mode = stat::Mode::from_bits_truncate(0o644);
        nix::unistd::chmod(Path::new(UDEV_RULES_PATH), mode)
            .context("Failed to set udev rules permissions")?;

        info!("Udev rules created at {}", UDEV_RULES_PATH);
        Ok(())
    }

    pub async fn update_battery_status(&self, battery_data: &crate::BatteryData) -> Result<()> {
        debug!("Updating Ubuntu battery status");

        // Update capacity (0-100)
        fs::write(
            format!("{}/capacity", self.device_path),
            battery_data.capacity_percent.to_string(),
        ).await.context("Failed to update capacity")?;

        // Update voltage (in microvolts)
        let voltage_uv = (battery_data.voltage_mv as u32) * 1000;
        fs::write(
            format!("{}/voltage_now", self.device_path),
            voltage_uv.to_string(),
        ).await.context("Failed to update voltage")?;

        // Update current (in microamps)
        let current_ua = (battery_data.current_ma as i32) * 1000;
        fs::write(
            format!("{}/current_now", self.device_path),
            current_ua.to_string(),
        ).await.context("Failed to update current")?;

        // Update status
        let status = if battery_data.is_charging {
            "Charging"
        } else if battery_data.capacity_percent == 100 {
            "Full"
        } else {
            "Discharging"
        };
        fs::write(
            format!("{}/status", self.device_path),
            status,
        ).await.context("Failed to update status")?;

        // Update present (1 = present, 0 = not present)
        fs::write(
            format!("{}/present", self.device_path),
            if battery_data.is_connected { "1" } else { "0" },
        ).await.context("Failed to update present status")?;

        // Update health
        let health = if battery_data.consecutive_failures > 5 {
            "Unspecified"
        } else if battery_data.capacity_percent < 5 {
            "Dead"
        } else {
            "Good"
        };
        fs::write(
            format!("{}/health", self.device_path),
            health,
        ).await.context("Failed to update health")?;

        // Update temperature (in tenths of degree Celsius)
        let temp_tenths = (battery_data.temperature_celsius as i32) * 10;
        fs::write(
            format!("{}/temp", self.device_path),
            temp_tenths.to_string(),
        ).await.context("Failed to update temperature")?;

        // Update time to empty (in minutes)
        if !battery_data.is_charging && battery_data.current_ma < 0 {
            let current_ma = (-battery_data.current_ma) as u32;
            if current_ma > 0 {
                // Estimate time to empty based on current discharge rate
                let remaining_capacity_ma = (battery_data.capacity_percent as u32) * 4000 / 100; // Assuming 4000mAh total capacity
                let time_to_empty_min = (remaining_capacity_ma * 60) / current_ma;
                fs::write(
                    format!("{}/time_to_empty_now", self.device_path),
                    time_to_empty_min.to_string(),
                ).await.context("Failed to update time to empty")?;
            }
        } else {
            // Clear time to empty for charging or unknown current
            fs::write(
                format!("{}/time_to_empty_now", self.device_path),
                "0",
            ).await.context("Failed to clear time to empty")?;
        }

        // Update time to full (in minutes) - rough estimate
        if battery_data.is_charging && battery_data.current_ma > 0 {
            let remaining_capacity_ma = ((100 - battery_data.capacity_percent) as u32) * 4000 / 100;
            let time_to_full_min = (remaining_capacity_ma * 60) / (battery_data.current_ma as u32);
            fs::write(
                format!("{}/time_to_full_now", self.device_path),
                time_to_full_min.to_string(),
            ).await.context("Failed to update time to full")?;
        } else {
            // Clear time to full for discharging
            fs::write(
                format!("{}/time_to_full_now", self.device_path),
                "0",
            ).await.context("Failed to clear time to full")?;
        }

        // Update cycle count (placeholder - would need to track this)
        fs::write(
            format!("{}/cycle_count", self.device_path),
            "0",
        ).await.context("Failed to update cycle count")?;

        // Update energy full design (in microWh)
        let energy_full_design_uw = 4000 * 12000; // 4000mAh * 12V = 48Wh = 48,000,000μWh
        fs::write(
            format!("{}/energy_full_design", self.device_path),
            energy_full_design_uw.to_string(),
        ).await.context("Failed to update energy full design")?;

        // Update energy full (current full capacity in microWh)
        let energy_full_uw = (battery_data.capacity_percent as u32) * energy_full_design_uw / 100;
        fs::write(
            format!("{}/energy_full", self.device_path),
            energy_full_uw.to_string(),
        ).await.context("Failed to update energy full")?;

        // Update energy now (current energy in microWh)
        let energy_now_uw = (battery_data.capacity_percent as u32) * energy_full_design_uw / 100;
        fs::write(
            format!("{}/energy_now", self.device_path),
            energy_now_uw.to_string(),
        ).await.context("Failed to update energy now")?;

        debug!("Ubuntu battery status updated successfully");
        Ok(())
    }

    pub async fn cleanup(&self) -> Result<()> {
        info!("Cleaning up Ubuntu battery integration");
        
        // Remove udev rules
        if Path::new(UDEV_RULES_PATH).exists() {
            fs::remove_file(UDEV_RULES_PATH).await
                .context("Failed to remove udev rules")?;
            info!("Udev rules removed");
        }

        // Note: We don't remove the battery device directory as it might be in use
        // The system will handle cleanup when the service stops
        
        Ok(())
    }
}

// D-Bus interface for UPower integration (if needed)
#[dbus_interface(name = "org.freedesktop.UPower.Device")]
impl UbuntuBatteryManager {
    #[dbus_interface(property)]
    fn native_path(&self) -> String {
        self.device_path.clone()
    }

    #[dbus_interface(property)]
    fn vendor(&self) -> String {
        "LatteDeck".to_string()
    }

    #[dbus_interface(property)]
    fn model(&self) -> String {
        "UPS Battery".to_string()
    }

    #[dbus_interface(property)]
    fn serial(&self) -> String {
        "LATTEDECK001".to_string()
    }

    #[dbus_interface(property)]
    fn power_supply(&self) -> bool {
        true
    }

    #[dbus_interface(property)]
    fn has_history(&self) -> bool {
        false
    }

    #[dbus_interface(property)]
    fn has_statistics(&self) -> bool {
        false
    }

    #[dbus_interface(property)]
    fn online(&self) -> bool {
        true
    }

    #[dbus_interface(property)]
    fn energy(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn energy_empty(&self) -> f64 {
        0.0
    }

    #[dbus_interface(property)]
    fn energy_full(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn energy_full_design(&self) -> f64 {
        48.0 // 48Wh design capacity
    }

    #[dbus_interface(property)]
    fn energy_rate(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn voltage(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn time_to_empty(&self) -> i64 {
        0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn time_to_full(&self) -> i64 {
        0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn percentage(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn temperature(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn is_present(&self) -> bool {
        true // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn state(&self) -> u32 {
        0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn is_rechargeable(&self) -> bool {
        true
    }

    #[dbus_interface(property)]
    fn capacity(&self) -> f64 {
        0.0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn technology(&self) -> u32 {
        1 // Li-ion
    }

    #[dbus_interface(property)]
    fn warning_level(&self) -> u32 {
        0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn battery_level(&self) -> u32 {
        0 // Will be updated by the main service
    }

    #[dbus_interface(property)]
    fn icon_name(&self) -> String {
        "battery".to_string()
    }
}
