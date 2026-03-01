# Changelog

## 1.6.0

### New Features

- **Path Tracking Timeout**: Automatically resets the state machine when someone enters halfway and turns back, preventing stuck states. Configurable via `path_tracking_timeout` (default: 3s).
- **Adaptive Thresholds**: Continuously adjusts idle baseline to handle environmental drift (temperature changes, lighting). Uses Exponential Moving Average for smooth updates. Configurable via `adaptive_threshold` block.
- **Timeout Event**: The `entry_exit_event` text sensor now publishes "Timeout" when an incomplete crossing is detected.

### Improvements

- Moved path tracking state from static variables to class members for better encapsulation and testability
- Added sanity checks for adaptive threshold updates (rejects readings >20% different from current idle)
- Enhanced logging for debugging path tracking and threshold updates
- Added configuration dump for new features in `dump_config()`

### Configuration

New configuration options:

```yaml
roode:
  # Reset state machine if no activity within this period (prevents stuck states)
  path_tracking_timeout: 3s

  # Continuously adjust thresholds for environmental drift
  adaptive_threshold:
    enabled: true
    update_interval: 60s  # How long zones must be empty before updating
    alpha: 0.05           # EMA smoothing factor (0.01-0.5)
```

## 1.5.4

- Fix ESPHome 2026.2+ compatibility by migrating ESP8266 configurations to new platform format
- Update CI configurations and example files to use separate `esp8266:` block instead of deprecated `platform:` key
- Fix Wire library dependency for VL53L1X_ULD to resolve build errors on ESP32
- Fix release workflow to skip creating releases that already exist

## 1.5.0

- Manual ROI configuration fixed
- Sensor initialization fixed
- Fix setup priorities to ensure proper boot up
- Code formatting
- Cleanup

## 1.4.1

- Timing budget test by @Lyr3x in #60
- Restructure configuration by @Lyr3x in #61
- v1.4.0 by @Lyr3x in #55
- Improve roi calibration by @Lyr3x in #64
- Fix presence sensor and wdt crashes by @Lyr3x in #67
- Improve sensor creation, initialization and measurement reads by @Lyr3x in #68
- Use sampling always by @Lyr3x in #71
- Improve error log and fix manual mode by @Lyr3x in #73
- Configure IDE intellisense by @CarsonF in #74
- Fix error handling by @Lyr3x in #75
- Fix manual roi setting by @Lyr3x in #78
