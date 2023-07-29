#include "SerialCPP/SerialCPP.h"

namespace SerialCPP
{

    SerialCPP::SerialCPP(const std::string &port, BaudRate baud, size_t bufferSize)
        : portName(port), baudRate(static_cast<size_t>(baud)), bufferSize(bufferSize)
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
            return false;
        }

        DCB dcbSerialParams = {0};
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams))
        {
            return false;
        }
        dcbSerialParams.BaudRate = baudRate;
        if (!SetCommState(hSerial, &dcbSerialParams))
        {
            return false;
        }
#else
        fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY);
        if (fd < 0)
        {
            return false;
        }

        struct termios tty;
        if (tcgetattr(fd, &tty) != 0)
        {
            return false;
        }

        speed_t speed;
        switch (baudRate)
        {
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        default:
            return false; // Unsupported baud rate
        }

        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
            return false;
        }
#endif
        return true;
    }

    bool SerialCPP::close()
    {
#ifdef _WIN32
        if (hSerial != INVALID_HANDLE_VALUE)
        {
            if (!CloseHandle(hSerial))
            {
                return false;
            }
            hSerial = INVALID_HANDLE_VALUE;
        }
#else
        if (fd >= 0)
        {
            if (::close(fd) == -1)
            {
                return false;
            }
            fd = -1;
        }
#endif

        return true;
    }

    bool SerialCPP::write(const uint8_t *data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            uint8_t c = data[i];

#ifdef _WIN32
            DWORD bytesWritten;
            if (!WriteFile(hSerial, &c, 1, &bytesWritten, NULL))
            {
                return false;
            }
#else
            ssize_t result = ::write(fd, &c, 1);
            if (result == -1)
            {
                return false;
            }
#endif
        }

        return true;
    }

    bool SerialCPP::writeLine(const std::string &data)
    {
        // Convert string to a byte array
        std::vector<uint8_t> byteData(data.begin(), data.end());

        // Append newline character
        byteData.push_back('\n');

        // Write the byte array using the write method
        return write(byteData.data(), byteData.size());
    }

    bool SerialCPP::fillBuffer()
    {
        std::vector<uint8_t> tempBuffer(bufferSize);
        size_t bytesRead;
#ifdef _WIN32
        if (!ReadFile(hSerial, tempBuffer.data(), tempBuffer.size(), reinterpret_cast<DWORD *>(&bytesRead), NULL))
        {
            return false;
        }
#else
        bytesRead = ::read(fd, tempBuffer.data(), tempBuffer.size());
        if (bytesRead == -1)
        {
            return false;
        }
#endif
        for (size_t i = 0; i < bytesRead; ++i)
        {
            inputBuffer.push_back(tempBuffer[i]);
        }

        return true;
    }

    std::optional<uint8_t> SerialCPP::read()
    {
        if (inputBuffer.empty())
        {
            fillBuffer();
        }

        if (!inputBuffer.empty())
        {
            uint8_t c = inputBuffer.front();
            inputBuffer.pop_front();
            return c;
        }

        return std::nullopt;
    }

    std::string SerialCPP::readLine()
    {
        std::string line;
        std::optional<uint8_t> opt;
        while ((opt = read()) && opt.value() != '\n')
        {
            line += static_cast<char>(opt.value());
        }
        return line;
    }

    size_t SerialCPP::readBytes(uint8_t *buffer, size_t n)
    {
        size_t i;
        std::optional<uint8_t> opt;
        for (i = 0; i < n && (opt = read()); ++i)
        {
            buffer[i] = opt.value();
        }
        return i;
    }

    size_t SerialCPP::available()
    {
        fillBuffer();
        return inputBuffer.size();
    }

} // namespace SerialCPP