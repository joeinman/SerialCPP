#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <deque>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

class SerialCPP
{
public:
    // Constructor: Initializes a new instance of the SerialCPP class.
    // @param port: The name of the serial port to connect to.
    // @param baudRate: The baud rate at which the communications device operates.
    // @param timeout: The timeout for read operations.
    // @param bufferSize: The size of the internal buffer.
    SerialCPP(const std::string &port, size_t baud = 115200, size_t timeout = 1000, size_t bufferSize = 64);

    // Destructor: Closes the serial port if it is open.
    ~SerialCPP();

    // Opens the serial port for communication.
    // @return: true if the port was opened successfully, false otherwise.
    bool open();

    // Closes the serial port.
    void close();

    // Writes data to the serial port.
    // @param data: The data to write to the port.
    // @param size: The number of bytes to write.
    void write(const uint8_t *data, size_t size);

    // Writes a string to the serial port, followed by a newline character.
    // @param data: The string to write to the port.
    void writeLine(const std::string &data);

    // Reads a byte from the serial port.
    // @return: The byte read.
    int read();

    // Reads data from the serial port.
    // @param buffer: The buffer to store the read data.
    // @param n: The maximum number of bytes to read.
    // @return: The actual number of bytes read.
    size_t readBytes(uint8_t *buffer, size_t n);

    // Reads a line of text from the serial port.
    // @return: The line of text read from the port.
    std::string readLine();

    // Returns the number of bytes available to read in the buffer.
    // @return: The number of bytes available to read.
    size_t available();

    // Sets the timeout for read operations.
    // @param timeout: The timeout in milliseconds.
    void setTimeout(size_t timeout);

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
    void fillBuffer();

    std::string portName;              // The name of the serial port.
    size_t baudRate;                   // The baud rate at which the communications device operates.
    std::deque<uint8_t> inputBuffer;   // The buffer for incoming data.
    size_t bufferSize;                 // The size of the buffer for read operations
    std::chrono::milliseconds timeout; // The timeout for read operations.

#ifdef _WIN32
    HANDLE hSerial; // The handle to the serial port (Windows only).
#else
    int fd; // The file descriptor for the serial port (non-Windows only).
#endif
};
