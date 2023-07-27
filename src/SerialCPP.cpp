#include "SerialCPP/SerialCPP.h"

SerialCPP::SerialCPP(const std::string &port, size_t baud, size_t timeouT, size_t bufferSize)
    : portName(port), baudRate(baud), timeout(std::chrono::milliseconds(timeout)), bufferSize(bufferSize)
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
    for (size_t i = 0; i < size; ++i)
    {
        uint8_t c = data[i];

#ifdef _WIN32
        DWORD bytesWritten;
        WriteFile(hSerial, &c, 1, &bytesWritten, NULL);
#else
        ssize_t result = ::write(fd, &c, 1);
        if (result == -1)
        {
            // TODO: Handle Error
        }
#endif
    }
}

void SerialCPP::writeLine(const std::string &data)
{
    // Convert string to a byte array
    std::vector<uint8_t> byteData(data.begin(), data.end());

    // Append newline character
    byteData.push_back('\n');

    // Write the byte array using the write method
    write(byteData.data(), byteData.size());
}

int SerialCPP::read()
{
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < timeout)
    {
        if (!inputBuffer.empty())
        {
            uint8_t c = inputBuffer.front();
            inputBuffer.pop_front();
            return c;
        }
        else
        {
            fillBuffer();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    return -1; // Return -1 on timeout
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

void SerialCPP::setTimeout(size_t timeout)
{
    this->timeout = std::chrono::milliseconds(timeout);
}