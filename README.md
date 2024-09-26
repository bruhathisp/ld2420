# ld2420






# Bedroom Presence Detection using LD2420 Radar with ESPHome

This project configures an ESP32 board for presence detection using the LD2420 radar sensor. The configuration includes specific move and still threshold values for 16 gates based on customized input.

## Prerequisites

1. **ESPHome**: Ensure you have [ESPHome](https://esphome.io/) installed. You can install it using `pip`:
   ```bash
   pip install esphome
   ```

2. **ESP32c3 Board**: This project is set up for the board.

3. **LD2420 Radar Sensor**: The radar sensor should be connected with the appropriate TX and RX pins.

## YAML Configuration

The YAML configuration defines the ESP32 board, the LD2420 radar sensor, and custom thresholds for presence detection. Here’s an overview of the settings:

- **UART**:
- tx_pin: GPIO6  # Update with your Tx pin (GPIO6 recommended for Xaio ESP32-C3)
- rx_pin: GPIO7  # Update with your Rx pin (GPIO7 recommended for Xaio ESP32-C3)
- **Gate-specific thresholds**: The `move_threshold` and `still_threshold` values for gates 0-15 have been customized based on user input.

### Configuration File


#### For ESPHome Dashboard (UI-based):
To compile the YAML file and upload it to your ESP32c3 device, follow these steps:

#### a) Create your ESPHome configuration

In a empty folder create the file  ld2420_project.yaml and paste the given code

Run this command to start the ESPHome dashboard:




```bash
esphome compile ld2420_project.yaml
esphome upload ld2420_project.yaml
esphome dashboard .
```

This will open the ESPHome dashboard in your browser at `http://localhost:6052`.

```

This `README.md` file will guide you through the installation, configuration, compilation, and usage of your ESP32-based radar sensor project.
