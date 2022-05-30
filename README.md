# Smallprox README

## General Information
- Author: Austin Norby
- Date 05/30/2022
- Description: Smallprox is a small, compile-time network proxy. This means it focuses on two main things while being a proxy: being small, and declaring everything at compile-time. The code is entirely contained in one file, main.c, and has no other features that what's necessary to proxy information from one host to another. Additionally, the configurations are made at compile-time and not at run-time. This keeps it easy to use once the binary has been made. Lastly, this code will compile for both Windows and Linux platforms.
- Tested on: Windows 11 and Ubuntu 20.04 (WSL)
  - Supports both Windows and Linux operating systems on 32-bit and 64-bit architectures.
  - Using gcc 8.1.0 for Linux builds
  - Using Visual Studio 2019 for Windows builds

## Video Link
- [Link]()

## Installation/Creation
- Requires a build environment
  - Visual Studio 2019
    - Import project
    - Build
  - GCC
    - `gcc main.c -o smallprox_release -Wno-incompatible-pointer-types`
    - `gcc main.c -o smallprox_debug -Wno-incompatible-pointer-types -D_DEBUG`
    - Ignores incompatible pointer types error when using `send()` and `recv()` because it wants const data

- Compile-time Options
  - These values may be supplied at compile time to configure smallprox
  - `-D_DEBUG`
    - Turn on debugging messages
  - `-DSERVER_IP=127.0.0.1`
    - Set address of smallprox to listen on (server)
  - `-DDEST_IP=127.0.0.1`
    - Set address for smallprox to send data to (dest)
  - `-DSERVER_PORT=1234`
    - Set port that smallprox listens on (server)
  - `-DDEST_PORT=8080`
    - Set port that smallproxy send data to (dest)

## Examples

## Additional Resources
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/#a-simple-stream-server)
- [MSDN](https://docs.microsoft.com/en-us/)
- [Linux Man Pages](https://linux.die.net/man/)
