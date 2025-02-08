#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

#define SERIAL_PORT "COM3"

int get_pop_num(char *the_exe_name_ascii)
{
    wchar_t the_exe_name[MAX_PATH];
    mbstowcs(the_exe_name, the_exe_name_ascii, MAX_PATH);
    wprintf(L"Checking for clones of %s\n", the_exe_name);

    PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W), 0};
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!Process32FirstW(hSnapShot, &pe)) {
        printf("Failed to enumerate processes.\n");
        exit(1);
    }
    int clone_count = 0;
    do {
        if (wcsncmp(pe.szExeFile, the_exe_name, MAX_PATH) == 0) {
            clone_count++;
        }
    } while (Process32NextW(hSnapShot, &pe));
    CloseHandle(hSnapShot);
    return clone_count;
}

#define SMART_LAUNCH_ACTION \
    if(get_pop_num("action_win.exe") == 0) \
        ShellExecute(NULL, "open", ActorEXE_PATH, NULL, NULL, SW_SHOWNORMAL); \
    else \
        printf("The Action is already going on!\n");

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

int main(int argc, char *argv[])
{
    if(get_pop_num(strrchr(argv[0], '\\') + 1) > 1) {
        printf("Another Instance of Mine is Already Running, Exiting...\n");
        return 0;
    }

    char ActorEXE_PATH[256];
    strcpy(ActorEXE_PATH, argv[0]);
    strcpy(strrchr(ActorEXE_PATH, '\\') + 1, "action_win.exe");
    FILE *fp = fopen(ActorEXE_PATH, "rb");
    if (fp == NULL) {
        printf("Please make sure the action_win.exe is in the same directory as the trigger_serial.exe\n");
        return 1;
    }
    fclose(fp);
    fp = NULL;

    start:
    while (1) {
        hSerial = InitSerial(SERIAL_PORT);
        if(hSerial) {
            break;
        }
        printf("Waiting for serial port to be available, Pls Make Sure the %s is available\n", SERIAL_PORT);
        Sleep(1000);
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
                    SMART_LAUNCH_ACTION
                }
            }
        } else {
            UINT16 errorCode = GetLastError();
            switch(errorCode) {
                case ERROR_NOT_READY: // error 21
                case ERROR_BAD_COMMAND: // error 22
                case ERROR_OPERATION_ABORTED: // error 995
                    printf("Serial port is closed, Will try to reopen\n");
                    printf("Consider of Sensor Device Died, will TRIGGER the action\n");
                    SMART_LAUNCH_ACTION
                    CloseHandle(hSerial);
                    goto start;
                default:
                    printf("Read failed (Error Code: %d)\n", errorCode);
                    break;
            }
        }
    }
}