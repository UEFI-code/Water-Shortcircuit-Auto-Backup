#include <windows.h>
#include <setupapi.h>
#include <devpkey.h>
#include <stdio.h>
#include <cfgmgr32.h>

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

        // disable the MEDIA class devices
        if (strcmp(deviceClass, "MEDIA") == 0) {
            printf("!!! Disabling device %s\n", instanceID);
            DEVINST devInst = 0;
            if (CM_Locate_DevNode(&devInst, instanceID, CM_LOCATE_DEVNODE_NORMAL) == CR_SUCCESS) {
                if (CM_Disable_DevNode(devInst, CM_DISABLE_UI_NOT_OK | CM_DISABLE_BITS) == CR_SUCCESS) {
                    printf("Device disabled\n");
                } else {
                    printf("Failed to disable device, Error: %lu\n", GetLastError());
                }
            } else {
                printf("Failed to locate device node, Error: %lu\n", GetLastError());
            }
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

int main() {
    PowerOffDevices();
    return 0;
}
