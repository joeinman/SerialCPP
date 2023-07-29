#pragma once

#include <optional>
#include <cstdint>
#include <vector>
#include <string>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace SerialCPP
{

    enum class BaudRate
    {
        BR_4800 = 4800,
        BR_9600 = 9600,
        BR_19200 = 19200,
        BR_38400 = 38400,
        BR_57600 = 57600,
        BR_115200 = 115200
    };

    class SerialCPP
    {
    public:
        // Constructor: Initializes a new instance of the SerialCPP class.
        // @param port: The name of the serial port to connect to.
        // @param baudRate: The baud rate at which the communications device operates.
        SerialCPP(const std::string &port, BaudRate baud = BaudRate::BR_115200);

        // Destructor: Closes the serial port if it is open.
        ~SerialCPP();

        // Opens the serial port for communication.
        // @return: true if the port was opened successfully, false otherwise.
        bool open();

        // Closes the serial port.
        // @return: true if the port was closed successfully, false otherwise.
        bool close();

        // Writes data to the serial port.
        // @param data: The data to write to the port.
        // @return: true if the data was written successfully, false otherwise.
        bool writeBytes(const std::vector<uint8_t> &data);

        // Writes a string to the serial port, followed by a newline character.
        // @param data: The string to write to the port.
        // @return: true if the string was written successfully, false otherwise.
        bool writeLine(const std::string &data);

        // Reads data from the serial port.
        // @param n: The maximum number of bytes to try and read.
        // @return: A vector containing the actual bytes read.
        std::vector<uint8_t> readBytes(size_t n);

        // Reads a line of text from the serial port.
        // @return: The line of text read from the port, or an empty string if no data is available.
        std::string readLine();

        // Returns True if the serial port is open, False otherwise.
        // @return: True if the serial port is open, False otherwise.
        operator bool() const
        {
#ifdef _WIN32
            return hSerial != INVALID_HANDLE_VALUE;
#else
            return fd >= 0;
#endif
        }

    private:
        // Reads a byte from the serial port.
        // @return: The byte read, or std::nullopt if no data is available.
        std::optional<uint8_t> read();

        // Writes a byte to the serial port.
        // @param data: The byte to write to the port.
        // @return: true if the data was written successfully, false otherwise.
        bool write(const uint8_t byte);

        std::string portName; // The name of the serial port.
        size_t baudRate;      // The baud rate at which the communications device operates.
        std::mutex mutex;     // The mutex used to synchronize access to the serial port.

#ifdef _WIN32
        HANDLE hSerial; // The handle to the serial port (Windows only).
#else
        int fd; // The file descriptor for the serial port (non-Windows only).
#endif
    };

} // namespace SerialCPP