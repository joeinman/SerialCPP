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
