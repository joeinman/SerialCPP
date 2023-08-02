#include "SerialCPP/SerialCPP.h"

namespace SerialCPP
{

    SerialCPP::SerialCPP(const std::string &port, BaudRate baud)
        : portName(port), baudRate(static_cast<size_t>(baud))
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
        // Lock the mutex to prevent multiple threads from accessing the serial port at the same time.
        std::lock_guard<std::mutex> lock(mutex);

#ifdef _WIN32
        hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hSerial == INVALID_HANDLE_VALUE)
        {
            return false;
        }
        else
        {
            DCB dcbSerialParameters = {0};

            if (!GetCommState(hSerial, &dcbSerialParameters))
            {
                return false;
            }
            else
            {
                dcbSerialParameters.BaudRate = baudRate;
                dcbSerialParameters.ByteSize = 8;
                dcbSerialParameters.StopBits = ONESTOPBIT;
                dcbSerialParameters.Parity = NOPARITY;
                dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

                if (!SetCommState(hSerial, &dcbSerialParameters))
                {
                    return false;
                }
                else
                {
                    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

                    // Set The Read Timeouts To Make ReadFile Non-Blocking
                    COMMTIMEOUTS timeouts = {0};
                    timeouts.ReadIntervalTimeout = MAXDWORD;
                    timeouts.ReadTotalTimeoutConstant = 0;
                    timeouts.ReadTotalTimeoutMultiplier = 0;
                    if (!SetCommTimeouts(hSerial, &timeouts))
                    {
                        return false;
                    }
                }
            }
        }
#else
        fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY /* | O_NONBLOCK*/);
        if (fd < 0)
        {
            return false;
        }

        struct termios tty;
        memset(&tty, 0, sizeof(tty));

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

        tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
        tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
        tty.c_cflag |= CS8;            // 8 bits per byte (most common)
        tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

        tty.c_lflag &= ~ICANON;
        tty.c_lflag &= ~ECHO;   // Disable echo
        tty.c_lflag &= ~ECHOE;  // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP

        tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // Turn off s/w flow ctrl
        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

        // Set in/out baud rate to be speed
        cfsetispeed(&tty, speed);
        cfsetospeed(&tty, speed);

        // Save tty settings, also checking for error
        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
            return false;
        }
#endif
        return true;
    }

    bool SerialCPP::close()
    {
        // Lock the mutex to prevent multiple threads from accessing the serial port at the same time.
        std::lock_guard<std::mutex> lock(mutex);

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

    bool SerialCPP::write(uint8_t byte)
    {
#ifdef _WIN32
        DWORD bytesWritten;
        if (!WriteFile(hSerial, &byte, 1, &bytesWritten, NULL))
        {
            return false;
        }
#else
        ssize_t result = ::write(fd, &byte, 1);
        if (result == -1)
        {
            return false;
        }
#endif
        return true;
    }

    bool SerialCPP::writeBytes(const std::vector<uint8_t> &data)
    {
        // Lock the mutex to prevent multiple threads from accessing the serial port at the same time.
        std::lock_guard<std::mutex> lock(mutex);

        for (const uint8_t &byte : data)
        {
            if (!write(byte))
            {
                return false;
            }
        }
        return true;
    }

    bool SerialCPP::writeLine(const std::string &data)
    {
        // Lock the mutex to prevent multiple threads from accessing the serial port at the same time.
        std::lock_guard<std::mutex> lock(mutex);

        // Convert string to a byte array
        std::vector<uint8_t> byteData(data.begin(), data.end());

        // Append newline character
        byteData.push_back('\n');

        // Write the byte array using the write method
        for (size_t i = 0; i < byteData.size(); ++i)
        {
            if (!write(byteData.data()[i]))
            {
                return false;
            }
        }
        return true;
    }

    std::optional<uint8_t> SerialCPP::read()
    {
        uint8_t byte;
#ifdef _WIN32
        DWORD bytesRead;
        if (!ReadFile(hSerial, &byte, 1, &bytesRead, NULL) || bytesRead == 0)
        {
            return std::nullopt;
        }
#else
        ssize_t result = ::read(fd, &byte, 1);
        if (result <= 0)
        {
            return std::nullopt;
        }
#endif
        return byte;
    }

    std::vector<uint8_t> SerialCPP::readBytes(size_t n)
    {
        // Lock the mutex to prevent multiple threads from accessing the serial port at the same time.
        std::lock_guard<std::mutex> lock(mutex);

        std::vector<uint8_t> buffer;
        std::optional<uint8_t> opt;
        for (size_t i = 0; i < n && (opt = read()); ++i)
        {
            buffer.push_back(opt.value());
        }
        return buffer;
    }

    std::string SerialCPP::readLine()
    {
        std::lock_guard<std::mutex> lock(mutex);

        std::string line;
        std::optional<uint8_t> opt;
        while ((opt = read()) && opt.value() != '\n')
        {
            line += static_cast<char>(opt.value());
        }

        // Remove Potential Trailing Newline & Carriage Return Characters
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        return line;
    }

    bool SerialCPP::isDeviceConnected()
    {
#ifdef _WIN32
        if (hSerial == INVALID_HANDLE_VALUE)
        {
            close();
            return false;
        }
        else
        {
            DWORD errors;
            COMSTAT status;
            ClearCommError(hSerial, &errors, &status);
            return errors == 0;
        }
#else
        if (fd < 0)
        {
            close();
            return false;
        }
        else
        {
            int mcs = 0;
            if (ioctl(fd, TIOCMGET, &mcs) < 0)
            {
                close();
                return false;
            }
            return true;
        }
#endif
    }

} // namespace SerialCPP