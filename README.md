# Automatic Gateway

## Architecture
```mermaid
graph TD
    %% Root Nodes
    Drivers[Drivers]
    Library[Library]

    %% Drivers Details
    Drivers --> GPIO[GPIO]
    GPIO --> SetCfg["set_cfg(cfg, initial_value)"]
    GPIO --> GetCfg["get_cfg()"]
    GPIO --> Write["write(state)"]
    GPIO --> Toggle["toggle()"]
    GPIO --> Read["read()"]

    %% Library Details
    Library --> MQTT[MQTT]
    MQTT --> Connect["connect()"]
    MQTT --> Publish["publish()"]
    MQTT --> Subscribe["subscribe()"]

    %% Intermediates and Their Dependencies
    EndlineSensor --> Drivers
    EndlineSensor --> Library

    Motor --> Drivers
    Motor --> Library

    ControlGate --> Drivers
    ControlGate --> Library

```

## External Interruptions
The GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, and GPIO11 can not recommended to use as interrupts because they are used by the SPI Flash communication.

## Code Quality
The project uses `clang-format` to enforce code style, to format the code run:
```bash
clang-format -i *.cpp *.h
```

### Download Clang-Format
- Linux:
    ```bash
    sudo apt-get install clang-format
    ```
- Windows:
  - Go to [LLVM GitHub](https://github.com/llvm/llvm-project/releases) releases page.
  - Download the `LLVM-19.1.0-win64.exe` file.
  - Run the installer and make sure to add LLVM to the system PATH.
**Note**: The `19.1.3` not works in my machine.

## Workflow
Own workflow is in `docs/development/workflow.md`.

## References
- [Clang-Format Download](https://medium.com/@swargarajd/installing-clang-formatter-in-visual-studio-code-for-windows-9a4fc882785c)