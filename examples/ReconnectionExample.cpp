#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <thread>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baudrate
    SerialCPP serial("COM3", 115200);

    while (1)
    {
        // If The Serial Port Is Not Open
        if (!serial)
        {
            // Print A Message Indicating That The Serial Port Is Disconnected
            std::cerr << "Serial Port Disconnected, Attempting To Reconnect...\n";

            // Attempt To Reopen The Serial Port
            if (serial.open())
            {
                // If The Serial Port Is Successfully Opened, Print A Success Message
                std::cout << "Connected Successfully.\n";
            }
            else
            {
                // If The Serial Port Could Not Be Opened, Wait For 1 Second Before Retrying
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

        // If There Is Data Available To Read From The Serial Port
        if (serial.available() > 0)
        {
            // Read A Line Of Text From The Serial Port & Print It To The Console
            std::string line = serial.readLine();
            std::cout << "Received: " << line << '\n';
        }
    }

    // Close The Serial Port
    serial.close();
    return 0;
}
