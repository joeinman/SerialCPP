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
#ifdef _WIN32
    DWORD bytesWritten;
    WriteFile(hSerial, data, size, &bytesWritten, NULL);
#else
    ::write(fd, data, size);
#endif
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
    uint8_t c;
#ifdef _WIN32
    DWORD bytesRead;
    ReadFile(hSerial, &c, 1, &bytesRead, NULL);
#else
    ::read(fd, &c, 1);
#endif
    return c;
}

size_t SerialCPP::readBytes(uint8_t *buffer, size_t n)
{
#ifdef _WIN32
    DWORD bytesRead;
    ReadFile(hSerial, buffer, n, &bytesRead, NULL);
#else
    ssize_t bytesRead = ::read(fd, buffer, n);
#endif
    return bytesRead;
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