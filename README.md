# SerialCPP

![CMake](https://github.com/joeinman/SerialCPP/actions/workflows/cmake.yml/badge.svg)

SerialCPP is a cross-platform C++ library for serial communication. Based on [PySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) and the [Arduino Serial](https://www.arduino.cc/reference/en/#serial) library, it provides a simple and consistent API for reading from and writing to serial ports on both Windows and Unix-like systems (such as Linux or macOS).

## Features:

- Open & Close Serial Ports
- Read and Write Data to Serial Ports
- Read & Write a Line of Text to and From a Serial Port

## Basic Example:

```cpp
#include <SerialCPP/SerialCPP.h>
#include <iostream>

int main()
{
    // Create a SerialCPP object using the COM3 port with a 115200 baud rate
    SerialCPP serial("COM3", 115200);

    // Open the serial port
    if (!serial.open())
    {
        std::cerr << "Failed to open serial port.\n";
        return 1;
    }

    // Write a line of text to the serial port
    serial.writeLine("Hello World.");

    // Read a line of text from the serial port
    std::string line = serial.readLine();
    std::cout << "Received: " << line << '\n';

    // Close the serial port
    serial.close();
    return 0;
}
```
