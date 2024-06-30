# Firmware

Firmware for Iida devkit.

## Requirements

| Tool         | Version |
|--------------|---------|
| CMake        | >= 3.23 |
| Ninja        | >= 1.10 |
| Python       | >= 3.8  |
| clang-format | >= 19   |

## How to build

### Manually

1. Configure the build project with `cmake -G Ninja -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug`.
2. Build the project with `cmake --build build`

### Visual Studio Code

1. Press `CTRL+SHIFT+B`

## How to debug

### Manually

1. Connect to the devkit with _JLinkGDBServer_.
2. Start _arm-none-eabi-gdb_ and connect to the JLink GDB server.

### Visual Studio Code

1. Press _F5_ to start the debugging task `Debug: JLink`.
