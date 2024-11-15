# Project Documentation: Gate Control System

## Basic Functionalities
- **Button Control**: Opens, closes, and stops the gate.
- **End-of-Stroke Sensors**: Detect when the gate is fully open or fully closed.
- **LED Indicators**: Simulate the gate's current activity (e.g., opening, closing, stopped).
- **Automatic Closing**: Automatically closes the gate after a predefined time.

## Communication with the Application

### ESP → App
- **Gate Opening/Closing Events**: Notify the app when the gate starts opening or closing.
- **Gate Status**: Send the current state (e.g., fully open, fully closed).
- **Interruptions**: Report any interruption during operation.

### App → ESP
- **Current Gate Status Request**: Query the gate's status (stopped/closing/opening).
- **Gate Control Commands**: Send commands to open or close the gate.

## Development Overview

### Drivers
**GPIO**: Provides basic input/output functionality.

**Functions**:
```c
// TODO: Refactor `GpioState_t` to other type
void set_cfg(gpio_config_t cfg, GpioState_t initial_value);
gpio_config_t get_cfg();
void write(bool state);
void toggle();
bool read();
```

**Assigned to**: @Marcos Gay (Due: 19/11)

### Libraries
**MQTT**: Handles communication between ESP and the application.

**Functions**:
```c
connect();
publish();
subscribe();
```

**Assigned to**: @Pedro L (Due: 11/19)

### Intermediates
- **LEDs**: Simulate gate activity.
  **Assigned to**: To be defined by 11/19

- **Endline Sensor**
  **Dependencies**:
  - Drivers: GPIO
  - Library: MQTT
  **Assigned to**: @blablabla (Due: 11/26)

- **Engine**
  **Dependencies**:
  - Drivers: GPIO
  - Library: MQTT
  **Assigned to**: @blablabla (Due: 11/26)

- **Control Gate (Button)**
  **Dependencies**:
  - Drivers: GPIO
  - Library: MQTT
  **Assigned to**: @blablabla (Due: 11/26)

## Topics

### Port Usage Research
Discover the best ports for interruptions and explain why certain ports shouldn't be used.
**Assigned to**: @Marcos Gay (Due: 19/11)

### Hardware Diagram
Create a hardware diagram after completing port research.
**Assigned to**: @Pedro L (Due: 11/26)

### MQTT Diagram
Design an MQTT communication diagram.
**Assigned to**: @Pedro L (Due: 11/19)

## Future Implementations
- **Add Motor Speed Control**: Gradually increase motor speed.
- **Open Gate Timeout**: Implement a timeout mechanism for gate opening.
- **Presence Sensor**: Introduce an anti-crush feature with a presence sensor.
