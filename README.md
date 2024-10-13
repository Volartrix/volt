# Volt
An operating system designed for the x86 architecture

![GitHub last commit](https://img.shields.io/github/last-commit/volartrix/volt?style=for-the-badge&logo=git)
![GitHub Issues ](https://img.shields.io/github/issues/volartrix/volt?style=for-the-badge&logo=github)
![GitHub License](https://img.shields.io/github/license/volartrix/volt?style=for-the-badge&logo=apache)
![GitHub Repo stars](https://img.shields.io/github/stars/volartrix/volt?style=for-the-badge&logo=github)
![Discord](https://img.shields.io/discord/1177845304373039234?style=for-the-badge&logo=discord&label=Discord)

## What is Volt?
Volt is an x86_64 based monolithic Operating System Kernel Based on the limine bootloader. It uses no external libraries and most of the code is written in C or x86 Assembly with NASM / Intel syntax. The kernel is designed to be fairly modern and lightweight (I find it ironic since its a monolithic kernel). Any dependencies will be listed under the `Dependencies` section of the Readme.

## What are Volt's features?
The kernel has a few important features that it needs to use to manage memory for example. Below is a list of features the kernel uses.

* **PMM**: A bitmap physical memory manager is used handle physical memory allocation
* **VMM**: A virtual memory manager is used to manage but not allocate virtual memory with paging. It is a bit broken right now

## How to use Volt
### Dependencies
1. `clang` - The C compiler because I don't like GCC
2. `nasm` - The assembler because I really don't like AT&T syntax
3. `mtools` and `xorriso` - The tools used to create the ISO and HDD images each
4. `make` - The build tool for the project
5. `git` - The VCS and used to get some dependencies 
6. `wget` - Also used to get dependencies
7. `qemu` - The emulator for the kernel

### Build and Run the Kernel
There are different option of running / emulating the kernel. Below is a list of them with all the commands
1. ISO image, UEFI BIOS, native Linux - `make run`
2. HDD image, UEFI BIOS, native Linux - `make run-hdd`
3. ISO image, normal BIOS, native Linux - `make run-bios`
4. HDD image, normal BIOS, native Linux - `make run-hdd-bios`
5. ISO image, UEFI BIOS, WSL Linux - `make run-wsl`
6. HDD image, UEFI BIOS, WSL Linux - `make run-hdd-wsl`
7. ISO image, normal BIOS, WSL Linux - `make run-wsl-bios`
8. HDD image, normal BIOS, WSL Linux - `make run-hdd-bios-wsl`

## Credits
Here are some deserved Credits:
* **Unmapped Stack**: @.jakedoesstuff on Discord - Helping me with paging and other things
* **Kevin Alavik**: @kevinalavik on Discord - Helping me with the PMM and other parts
* **OSDev Wiki**: https://wiki.osdev.org - One of the most important parts of OSDev

## Other Info
### Inspiration
The original inspiration came from Nanobyte OS and jdh's video of making a Tetris OS

## License
This Project is licensed under the Apache License Version 2.0