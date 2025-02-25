#include <windows.h>
#include <setupapi.h>
#include <devpkey.h>
#include <stdio.h>
#include <cfgmgr32.h>
#include <tlhelp32.h>

char *DISABLE_TARGET_CLASS[] = {"MEDIA"};

int get_pop_num(char *my_exe_name_ascii)
{
    wchar_t my_exe_name[MAX_PATH];
    mbstowcs(my_exe_name, my_exe_name_ascii, MAX_PATH);
    wprintf(L"Checking for clones of %s\n", my_exe_name);

    PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W), 0};
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!Process32FirstW(hSnapShot, &pe)) {
        printf("Failed to enumerate processes.\n");
        exit(1);
    }
    int clone_count = 0;
    do {
        if (wcsncmp(pe.szExeFile, my_exe_name, MAX_PATH) == 0) {
            clone_count++;
        }
    } while (Process32NextW(hSnapShot, &pe));
    CloseHandle(hSnapShot);
    return clone_count;
}

void chk_and_disable_dev(char *class_name, char *instance_id)
{
    for(int i=0; i < sizeof(DISABLE_TARGET_CLASS) / sizeof(void *); i++)
    {
        if(strcmp(class_name, DISABLE_TARGET_CLASS[i]) != 0)
        {
            continue;
        }
        printf("!!! Disabling device %s ... ", instance_id);
        DEVINST devInst = 0;
        if (CM_Locate_DevNode(&devInst, instance_id, CM_LOCATE_DEVNODE_NORMAL) == CR_SUCCESS) {
            if (CM_Disable_DevNode(devInst, CM_DISABLE_UI_NOT_OK | CM_DISABLE_BITS) == CR_SUCCESS) {
                printf("Device disabled\n");
            } else {
                printf("Failed to disable device, Error: %lu\n", GetLastError());
            }
        } else {
            printf("Failed to locate device node, Error: %lu\n", GetLastError());
        }
        return;
    }
}

void PowerOffDevices() {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        printf("Failed to get device information set\n");
        return;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    char deviceName[1024] = {0};
    char instanceID[1024] = {0};

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            printf("Device %d: %s, ", i, deviceName);
        } else {
            printf("Failed to get device description, ");
        }

        // get device class string
        char deviceClass[1024] = {0};
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_CLASS, NULL,
                                            (PBYTE)deviceClass, sizeof(deviceClass), NULL)) {
            printf("Class: %s, ", deviceClass);
        } else {
            printf("Failed to get device class, Error: %lu, ", GetLastError());
            deviceClass[0] = 0;
        }
        
        if (SetupDiGetDeviceInstanceId(deviceInfoSet, &deviceInfoData, instanceID, sizeof(instanceID), NULL)) {
            printf("Instance ID: %s\n", instanceID);
        } else {
            printf("Failed to get device instance ID, Error: %lu\n", GetLastError());
            instanceID[0] = 0;
        }

        // disable the target devices
        chk_and_disable_dev(deviceClass, instanceID);
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

void BakupData() {
    // assume we copy C:\Important -> Y:\BakupFolder
    //first, we need to check if Y: is mounted
    DWORD dwResult = GetFileAttributes("Y:");
    if (dwResult == INVALID_FILE_ATTRIBUTES) {
        printf("!!! Y: is not mounted\n");
        return;
    }
    // then we need to check if Y:\BakupFolder exists
    if (GetFileAttributes("Y:\\BakupFolder") == INVALID_FILE_ATTRIBUTES) {
        if (!CreateDirectory("Y:\\BakupFolder", NULL)) {
            printf("!!! Failed to create directory Y:\\BakupFolder, Error: %lu\n", GetLastError());
            return;
        }
    }
    // finally, we copy the data
    system("robocopy C:\\Important Y:\\BakupFolder /E /V /MT:8");
    printf("!!! Data backup completed\n");
}

int main(int argc, char *argv[]) {
    if(get_pop_num(strrchr(argv[0], '\\') + 1) > 1)
    {
        printf("!!! Another instance is running, exiting...\n");
        return 0;
    }
    HANDLE powerOffThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PowerOffDevices, NULL, 0, NULL);
    BakupData();
    // Wait for the thread to finish
    WaitForSingleObject(powerOffThread, INFINITE);
    return 0;
}