#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <thread>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baud Rate
    SerialCPP serial("COM3", 115200);

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

        // If There Is Data Available To Read From The Serial Port
        if (serial.available() > 0)
        {
            // Read A Line Of Text From The Serial Port And Print It To The Console
            std::string line = serial.readLine();
            std::cout << "Received: " << line << '\n';
        }

        // Sleep For A Short Duration To Prevent High CPU Usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
