#include "SerialCPP/SerialCPP.h"
#include <iostream>
#include <thread>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baudrate
    SerialCPP serial("COM3", 115200);

    while (1)
    {
        // If The Serial Port Is Not Open, Attempt To Reopen It
        if (!serial)
        {
            // Attempt To Reopen The Serial Port
            serial.open();
            std::cout << "Connected Successfully.\n";
        }

        // If There Is Data Available To Read From The Serial Port
        if (serial.available() > 0)
        {
            // Read A Line Of Text From The Serial Port & Print It To The Console
            std::string line = serial.readLine();
            std::cout << "Received: " << line << '\n';
        }
    }

    // Note: The code will never reach this point, so the "serial.close()" is not necessary.
    return 0;
}
