# ld2420






# Bedroom Presence Detection using LD2420 Radar with ESPHome

This project configures an ESP32 board for presence detection using the LD2420 radar sensor. The configuration includes specific move and still threshold values for 16 gates based on customized input.

## Prerequisites

1. **ESPHome**: Ensure you have [ESPHome](https://esphome.io/) installed. You can install it using `pip`:
   ```bash
   pip install esphome
   ```

2. **ESP32 Board**: This project is set up for the `esp32dev` board.

3. **LD2420 Radar Sensor**: The radar sensor should be connected with the appropriate TX and RX pins.

## YAML Configuration

The YAML configuration defines the ESP32 board, the LD2420 radar sensor, and custom thresholds for presence detection. Here’s an overview of the settings:

- **UART**: Configured for TX (GPIO17) and RX (GPIO16) pins at a baud rate of 115200.
- **Gate-specific thresholds**: The `move_threshold` and `still_threshold` values for gates 0-15 have been customized based on user input.

### Configuration File

```yaml
esphome:
  name: bedroompresencedetection
  friendly_name: BedroomPresenceDetection
  platformio_options:
    upload_speed: 921600
    board_build.flash_mode: dio

esp32:
  board: esp32dev

logger:
  level: DEBUG
  baud_rate: 115200

wifi:
  ssid: "your ssid"
  password: "your password"
  ap:
    ssid: "Bedroompresencedetection"
    password: "axxxxxxxxx"

captive_portal:

uart:
  id: ld2420_radar
  tx_pin: GPIO17
  rx_pin: GPIO16
  baud_rate: 115200
  parity: NONE
  stop_bits: 1

ld2420:

text_sensor:
  - platform: ld2420
    fw_version:
      name: LD2420 Firmware

sensor:
  - platform: ld2420
    moving_distance:
      name: Moving Distance

binary_sensor:
  - platform: ld2420
    has_target:
      name: Presence

select:
  - platform: ld2420
    operating_mode:
      name: Operating Mode

number:
  - platform: ld2420
    presence_timeout:
      name: '120s'

    min_gate_distance:
      name: '1'

    max_gate_distance:
      name: '12'

    gate_select:
      name: '0'

    still_threshold:
      name: '40000'

    move_threshold:
      name: '60000'

    gate_move_sensitivity:
      name: '0.5'

    gate_still_sensitivity:
      name: '0.5'

    gate_0:
      move_threshold:
        name: '50.49'
      still_threshold:
        name: '48.95'

    gate_1:
      move_threshold:
        name: '49.51'
      still_threshold:
        name: '47.97'

    gate_2:
      move_threshold:
        name: '44.68'
      still_threshold:
        name: '43.13'

    gate_3:
      move_threshold:
        name: '29.22'
      still_threshold:
        name: '27.68'

    gate_4:
      move_threshold:
        name: '29.46'
      still_threshold:
        name: '27.92'

    gate_5:
      move_threshold:
        name: '28.10'
      still_threshold:
        name: '26.55'

    gate_6:
      move_threshold:
        name: '25.68'
      still_threshold:
        name: '24.13'

    gate_7:
      move_threshold:
        name: '22.97'
      still_threshold:
        name: '21.43'

    gate_8:
      move_threshold:
        name: '22.12'
      still_threshold:
        name: '20.57'

    gate_9:
      move_threshold:
        name: '21.14'
      still_threshold:
        name: '19.59'

    gate_10:
      move_threshold:
        name: '20.61'
      still_threshold:
        name: '19.08'

    gate_11:
      move_threshold:
        name: '20.00'
      still_threshold:
        name: '18.45'

    gate_12:
      move_threshold:
        name: '19.78'
      still_threshold:
        name: '18.20'

    gate_13:
      move_threshold:
        name: '19.34'
      still_threshold:
        name: '17.78'

    gate_14:
      move_threshold:
        name: '18.92'
      still_threshold:
        name: '17.40'

    gate_15:
      move_threshold:
        name: '19.03'
      still_threshold:
        name: '17.48'

button:
  - platform: ld2420
    apply_config:
      name: Apply Config
    factory_reset:
      name: Factory Reset
    restart_module:
      name: Restart Module
    revert_config:
      name: Undo Edits
```



#### For ESPHome Dashboard (UI-based):
To compile the YAML file and upload it to your ESP32 device, follow these steps:

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
