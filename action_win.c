#include <windows.h>
#include <setupapi.h>
#include <devpkey.h>
#include <stdio.h>
#include <cfgmgr32.h>

char *DISABLE_TARGET_CLASS[] = {"MEDIA"};

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

int main() {
    PowerOffDevices();
    return 0;
}