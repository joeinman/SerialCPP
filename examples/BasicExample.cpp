#include <SerialCPP/SerialCPP.h>
#include <iostream>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baudrate
    SerialCPP serial("COM3", 115200);

    // Open The Serial Port
    if (!serial.open())
    {
        std::cerr << "Failed To Open Serial Port.\n";
        return 1;
    }

    // Write A Line Of Text To The Serial Port
    serial.writeLine("Hello World.");

    // Read A Line Of Text From The Serial Port
    try
    {
        std::string line = serial.readLine();
        std::cout << "Received: " << line << '\n';
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "An error occurred: " << e.what() << '\n';
    }

    // Close The Serial Port
    serial.close();
    return 0;
}
