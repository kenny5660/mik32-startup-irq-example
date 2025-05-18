# MIK32 Startup and IRQ example
* This repo contain alternative simple linker, startup and irq (in develop) example code for mcu [MIK32](https://mik32.ru/)
* The code does not use vendor specific HAL and libs.
* The startup file is simplified as much as possible and written in C
* Linker file is also simplified as much as possible and takes into account the [RISC-V Linker Relaxation](https://www.sifive.com/blog/all-aboard-part-3-linker-relaxation-in-riscv-toolchain)
* The project is built using simple `GNU make` file

## Tool requirements

* [riscv-none-elf-gcc-14.2.0-3](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/tag/v14.2.0-3)
* [openocd-12](https://github.com/xpack-dev-tools/openocd-xpack/releases/tag/v0.12.0-6)
* [mik32-uploader-0.3.3](https://github.com/MikronMIK32/mik32-uploader/releases/tag/v0.3.3)

## Build

```
make RISCV_TOOLCHAIN_ROOT=<path-to-toolchain>/bin/
```

## Flash

For flashing mcu, you need to install  and 
```
make flash MIK32_UPLOADER_DIR=<path-to-mik32-uploader> OPENOCD_PATH=<path-to-opencd>/bin/openocd
```