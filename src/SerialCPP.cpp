#include "SerialCPP/SerialCPP.h"
#include <iostream>
#include <vector>

SerialCPP::SerialCPP(const std::string &port, unsigned long baud) : portName(port), baudRate(baud)
{
#ifdef _WIN32
    hSerial = INVALID_HANDLE_VALUE;
#else
    fd = -1;
#endif
}

SerialCPP::~SerialCPP()
{
    close();
}

bool SerialCPP::open()
{
#ifdef _WIN32
    hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        // handle error
        return false;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        // handle error
        return false;
    }
    dcbSerialParams.BaudRate = baudRate;
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        // handle error
        return false;
    }
#else
    fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        // handle error
        return false;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        // handle error
        return false;
    }
    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        // handle error
        return false;
    }
#endif
    return true;
}

void SerialCPP::close()
{
#ifdef _WIN32
    if (hSerial != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
#else
    if (fd >= 0)
    {
        ::close(fd);
        fd = -1;
    }
#endif
}

void SerialCPP::write(const uint8_t *data, size_t size)
{
    // Add the data to the output buffer
    for (size_t i = 0; i < size; ++i)
    {
        outputBuffer.push_back(data[i]);
    }

    // Send all the data in the output buffer
    while (!outputBuffer.empty())
    {
        uint8_t c = outputBuffer.front();
        outputBuffer.pop_front();

#ifdef _WIN32
        DWORD bytesWritten;
        WriteFile(hSerial, &c, 1, &bytesWritten, NULL);
#else
        ::write(fd, &c, 1);
#endif
    }
}

void SerialCPP::writeLine(const std::string &data)
{
    // Convert string to a byte array
    std::vector<uint8_t> byteData(data.begin(), data.end());

    // Append newline character
    byteData.push_back('\n');

    // Write the byte array
    write(byteData.data(), byteData.size());
}

uint8_t SerialCPP::read()
{
    if (inputBuffer.empty())
        fillBuffer();

    uint8_t c = inputBuffer.front();
    inputBuffer.pop_front();
    return c;
}

void SerialCPP::fillBuffer()
{
    uint8_t tempBuffer[64];
    size_t bytesRead;
#ifdef _WIN32
    ReadFile(hSerial, tempBuffer, sizeof(tempBuffer), reinterpret_cast<DWORD *>(&bytesRead), NULL);
#else
    bytesRead = ::read(fd, tempBuffer, sizeof(tempBuffer));
#endif
    for (size_t i = 0; i < bytesRead; ++i)
    {
        inputBuffer.push_back(tempBuffer[i]);
    }
}

size_t SerialCPP::readBytes(uint8_t *buffer, size_t n)
{
    size_t i;
    for (i = 0; i < n && !this->inputBuffer.empty(); ++i)
    {
        buffer[i] = read();
    }
    return i;
}

std::string SerialCPP::readLine()
{
    std::string line;
    uint8_t c;
    while ((c = read()) != '\n')
    {
        line += static_cast<char>(c);
    }
    return line;
}

size_t SerialCPP::available()
{
    fillBuffer();
    return inputBuffer.size();
}