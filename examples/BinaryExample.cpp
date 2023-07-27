#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <vector>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baudrate
    SerialCPP serial("COM3", 115200);

    // Attempt To Open The Serial Port
    if (!serial.open())
    {
        // If The Serial Port Could Not Be Opened, Print An Error Message And Exit
        std::cerr << "Failed To Open Serial Port\n";
        return 1;
    }

    try
    {
        // Create A Vector Of Bytes To Send
        std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};

        // Write The Bytes To The Serial Port
        serial.write(data.data(), data.size());

        // Create A Buffer To Store The Received Bytes
        uint8_t buffer[4];

        // Attempt To Read 4 Bytes From The Serial Port
        if (serial.readBytes(buffer, 4) == 4)
        {
            // If 4 Bytes Were Successfully Read, Print Them
            std::cout << "Received: ";
            for (int i = 0; i < 4; ++i)
            {
                std::cout << static_cast<int>(buffer[i]) << ' ';
            }
            std::cout << '\n';
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "An error occurred: " << e.what() << '\n';
    }

    // Close The Serial Port
    serial.close();
    return 0;
}
