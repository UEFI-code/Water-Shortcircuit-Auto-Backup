#include <windows.h>
#include <stdio.h>

#define SERIAL_PORT "COM3"

HANDLE InitSerial(char *serialPort)
{
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    hSerial = CreateFile(serialPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Failed to open serial port (Error Code: %d)\n", GetLastError());
        return NULL;
    }
    printf("Serial port %s opened successfully\n", serialPort);

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Failed to get serial port state\n");
        CloseHandle(hSerial);
        return NULL;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Failed to set serial port parameters\n");
        CloseHandle(hSerial);
        return NULL;
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Failed to set timeouts\n");
        CloseHandle(hSerial);
        return NULL;
    }

    return hSerial;
}

HANDLE hSerial = NULL;
int main() {
    start:
    hSerial = NULL;
    while(hSerial == NULL) {
        hSerial = InitSerial(SERIAL_PORT);
        if(hSerial == NULL) {
            printf("Waiting for serial port to be available, Pls Make Sure the %s is available\n", SERIAL_PORT);
            Sleep(1000);
        }
    }

    char buffer[256];
    DWORD bytesRead;
    while (1) {
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                printf("Received: %s\n", buffer);
                if(strcmp(buffer, "ALERT") == 0) {
                    printf("Triggering action\n");
                    system("action_win.exe");
                }
            }
        } else {
            UINT16 errorCode = GetLastError();
            switch(errorCode) {
                case ERROR_INVALID_HANDLE:
                    printf("Serial port is closed, Reopening\n");
                    CloseHandle(hSerial);
                    goto start;
                default:
                    printf("Read failed (Error Code: %d)\n", errorCode);
                    break;
            }
        }
    }

    CloseHandle(hSerial);
    printf("Serial port closed\n");

    return 0;
}
