# Firmware

Firmware for Iida devkit.

## How to build

### Manually

1. Configure the build project with `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi.cmake`.
2. Build the project with `cmake --build build`

### Visual Studio Code

1. Press `CTRL+SHIFT+B`

## How to debug

### Manually

1. Connect to the devkit with _JLinkGDBServer_.
2. Start _arm-none-eabi-gdb_ and connect to the JLink GDB server.

### QEMU

1. Start QEMU with `qemu-system-arm -M netduinoplus2 -kernel firmware/build/bootloader/bootloader.elf -nographic -serial null -serial stdio -monitor none -s -S`-
2. Start _arm-none-eabi-gdb_ and connect to the QEMU GDB server (`localhost:1234`).

### Visual Studio Code

1. Press _F5_ to start the debugging task `Debug: JLink`.
