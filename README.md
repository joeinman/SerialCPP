# SerialCPP

![CMake](https://github.com/joeinman/SerialCPP/actions/workflows/cmake.yml/badge.svg)

SerialCPP is a CrossPlatform C++ Library for Serial Communication. It provides a Simple and Consistent API for Reading from and Writing to Serial Ports on both Windows and Unix-like Systems (such as Linux or MacOS).

## Features:

- Open & Close Serial Ports
- Read and Write Data to Serial Ports
- Read & Write a Line of Text to and From a Serial Port

## Basic Example:

```cpp
#include "SerialCPP/SerialCPP.h"
#include <iostream>

int main()
{
    // Create A SerialCPP Object For The COM3 Port With A 115200 Baudrate
    SerialCPP serial("COM3", 115200);

    // Open The Serial Port
    if (!serial.open())
    {
        std::cerr << "Failed to open serial port\n";
        return 1;
    }

    // Write A Line Of Text To The Serial Port
    serial.writeLine("Hello, world!");

    // Read A Line Of Text From The Serial Port
    std::string line = serial.readLine();
    std::cout << "Received: " << line << '\n';

    // Close The Serial Port
    serial.close();
    return 0;
}
```
