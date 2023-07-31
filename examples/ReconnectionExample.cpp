#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <thread>
#include <string>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baud Rate
    SerialCPP::SerialCPP serial("COM3", SerialCPP::BaudRate::BR_115200);

    while (1)
    {
        // If The Serial Port Is Not Open, Attempt To Reopen It
        if (!serial)
        {
            std::cout << "Attempting To Connect...\n";

            // Attempt To Reopen The Serial Port
            if (serial.open())
            {
                std::cout << "Connected Successfully.\n";
            }
            else
            {
                std::cout << "Failed To Connect. Retrying...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

        // Read A Line Of Text From The Serial Port
        std::string line = serial.readLine();

        // Close The Serial Port If The Line Is Empty
        if (line.empty())
        {
            serial.close();
            continue;
        }

        // Print The Line To The Console
        std::cout << "Received: " << line << std::endl;

        // Sleep For A Short Duration To Prevent High CPU Usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    serial.close();
    return 0;
}
