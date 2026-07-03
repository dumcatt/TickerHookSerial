#include "pch.h"
#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

class serial_server {
public:
    serial_server() : hComm(INVALID_HANDLE_VALUE), port("COM1"), baudrate(115200) {
        LoadConfig();
    }

    ~serial_server() {
        if (hComm != INVALID_HANDLE_VALUE) {
            CloseHandle(hComm);
        }
    }

    void LoadConfig() {
        std::ifstream configFile("tickerhook.conf");
        if (configFile.is_open()) {
            std::string line;
            while (std::getline(configFile, line)) {
                // simple trim
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);

                std::istringstream is_line(line);
                std::string key;
                if (std::getline(is_line, key, '=')) {
                    std::string value;
                    if (std::getline(is_line, value)) {
                        // trim key and value
                        key.erase(0, key.find_first_not_of(" \t\r\n"));
                        key.erase(key.find_last_not_of(" \t\r\n") + 1);
                        value.erase(0, value.find_first_not_of(" \t\r\n"));
                        value.erase(value.find_last_not_of(" \t\r\n") + 1);

                        if (key == "PORT") {
                            port = value;
                        } else if (key == "BAUD") {
                            try {
                                baudrate = std::stoi(value);
                            } catch (...) {
                                std::cout << "Invalid BAUD in config" << std::endl;
                            }
                        }
                    }
                }
            }
        } else {
            std::cout << "Could not open tickerhook.conf, using defaults PORT=" << port << " BAUD=" << baudrate << std::endl;
        }
    }

    void run() {
        std::string portName = "\\\\.\\" + port; // Support COM10 and above
        hComm = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hComm == INVALID_HANDLE_VALUE) {
            std::cout << "Error opening serial port " << port << std::endl;
            return;
        }

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hComm, &dcbSerialParams)) {
            std::cout << "Error getting state" << std::endl;
            CloseHandle(hComm);
            hComm = INVALID_HANDLE_VALUE;
            return;
        }

        dcbSerialParams.BaudRate = baudrate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hComm, &dcbSerialParams)) {
            std::cout << "Error setting state" << std::endl;
            CloseHandle(hComm);
            hComm = INVALID_HANDLE_VALUE;
            return;
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        if (!SetCommTimeouts(hComm, &timeouts)) {
            std::cout << "Error setting timeouts" << std::endl;
            CloseHandle(hComm);
            hComm = INVALID_HANDLE_VALUE;
            return;
        }

        std::cout << "Serial port " << port << " opened successfully at " << baudrate << " baud." << std::endl;
    }

    void send(const char* text) {
        if (hComm == INVALID_HANDLE_VALUE) return;

        DWORD bytesWritten;
        WriteFile(hComm, "\n", 1, &bytesWritten, NULL);
    }

private:
    HANDLE hComm;
    std::string port;
    int baudrate;
};
