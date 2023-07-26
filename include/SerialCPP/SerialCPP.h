#pragma once

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

class SerialPort
{
public:
    SerialPort(const std::string &port);
    ~SerialPort();

    bool open();
    void close();

    void write(const std::string &data);
    void writeLine(const std::string &data);
    std::string read(size_t n = 1);
    std::string readLine();

private:
    std::string portName;

#ifdef _WIN32
    HANDLE hSerial;
#else
    int fd;
#endif
};
