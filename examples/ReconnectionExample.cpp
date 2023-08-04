#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <thread>
#include <string>

int main(int argc, char **argv)
{
    // Check If COM Port Was Provided
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <COM Port>\n";
        return 1;
    }

    // Create A SerialCPP Object Using The COM Port With A 115200 Baud Rate
    std::string comPort = argv[1];
    SerialCPP::SerialCPP serial(comPort, SerialCPP::BaudRate::BR_115200);

    while (1)
    {
        // If The Serial Port Is Not Open, Attempt To Reopen It
        if (!serial)
        {
            std::cout << "Attempting To Connect to " << comPort << "...\n";

            // Attempt To Reopen The Serial Port
            if (serial.open())
            {
                std::cout << "Connected Successfully to " << comPort << ".\n";
            }
            else
            {
                std::cout << "Failed To Connect to " << comPort << ". Retrying...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

        // Read A Line Of Text From The Serial Port & Print It To The Console
        std::string line = serial.readLine();
        if (!line.empty())
            std::cout << "Received: " << line << std::endl;

        // Sleep For A Short Duration To Prevent High CPU Usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    serial.close();
    return 0;
}
