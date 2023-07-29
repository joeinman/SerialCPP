# SerialCPP

![CMake](https://github.com/joeinman/SerialCPP/actions/workflows/cmake.yml/badge.svg)

SerialCPP is a cross-platform C++ library for serial communication. Based on [PySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) and [Arduino Serial](https://www.arduino.cc/reference/en/#serial), it provides a simple and consistent API for use on both Windows and Unix-like systems (such as Linux or macOS).

## Features:

- Open & Close Serial Ports
- Read and Write Data to Serial Ports
- Read & Write a Line of Text to and From a Serial Port

## Basic Example:

```cpp
#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <string>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baud Rate
    SerialCPP::SerialCPP serial("COM3", SerialCPP::BaudRate::BR_115200);

    // Open The Serial Port
    if (!serial.open())
    {
        std::cerr << "Failed To Open Serial Port.\n";
        return 1;
    }

    // Write A Line Of Text To The Serial Port
    serial.writeLine("Hello World.");

    // Read A Line Of Text From The Serial Port
    std::string line = serial.readLine();
    std::cout << "Received: " << line << '\n';

    // Close The Serial Port
    serial.close();
    return 0;
}
```
