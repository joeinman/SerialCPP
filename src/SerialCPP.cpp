#include "SerialCPP.h"
#include <iostream>

SerialPort::SerialPort(const std::string &port, unsigned long baud) : portName(port), baudRate(baud)
{
#ifdef _WIN32
    hSerial = INVALID_HANDLE_VALUE;
#else
    fd = -1;
#endif
}

SerialPort::~SerialPort()
{
    close();
}

bool SerialPort::open()
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

void SerialPort::close()
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

void SerialPort::write(const std::string &data)
{
#ifdef _WIN32
    DWORD bytesWritten;
    WriteFile(hSerial, data.c_str(), data.size(), &bytesWritten, NULL);
#else
    ::write(fd, data.c_str(), data.size());
#endif
}

void SerialPort::writeLine(const std::string &data)
{
    write(data + "\n");
}

std::string SerialPort::read(size_t n)
{
    char buffer[1024];
#ifdef _WIN32
    DWORD bytesRead;
    ReadFile(hSerial, buffer, n, &bytesRead, NULL);
#else
    ssize_t bytesRead = ::read(fd, buffer, n);
#endif
    return std::string(buffer, bytesRead);
}

std::string SerialPort::readLine()
{
    std::string line;
    char c;
    while ((c = read(1)[0]) != '\n')
    {
        line += c;
    }
    return line;
}