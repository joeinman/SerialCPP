#include <SerialCPP/SerialCPP.h>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>

int main()
{
    // Create A SerialCPP Object Using The COM3 Port With A 115200 Baud Rate
    SerialCPP::SerialCPP serial("COM3", SerialCPP::BaudRate::BR_115200);

    // Open The Serial Port
    if (!serial.open())
    {
        std::cerr << "Failed To Open Serial Port." << std::endl;
        return 1;
    }

    // Create A Vector Of Bytes To Send
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};

    // Write The Bytes To The Serial Port
    serial.writeBytes(data);

    // Read 4 Bytes From The Serial Port
    std::vector<uint8_t> receivedData = serial.readBytes(4);

    // Print The Received Bytes
    std::cout << "Received: ";
    for (const auto &byte : receivedData)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // Close The Serial Port
    serial.close();
    return 0;
}
