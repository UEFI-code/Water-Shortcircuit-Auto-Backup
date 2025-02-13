# Auto Data Backup and Circuit Protection for Water Disasters

## Introduction

This project contains a sensor system and software components that can monitor hardware test points' potential or impedance, shut down unnecessary hardware components, and back up data to a remote server in case of water-caused short circuits or other disasters.

Our solution is very useful for data centers, and much lower cost than traditional Fully_Geographic-Redundant-Solution (F_GRS) systems.

# Hardware Implementation

We provide an Arduino-based sensor system, using the serial port for feedback on potential status. You may replace the Arduino with any other microcontroller and using any other communication method, as long as the system can provide the potential status of the test points.

If you choose our implementation, you will need the following steps:

1. Attach a potential board to your server's metal case, and connect the board to the Arduino's `A0` pin.
2. Attach a resistor between the Arduino's `3.3v` pin and the `A0` pin.
3. Connect the Arduino to your server's USB port.
4. Check the GND connection between the Arduino and the server.
5. Follow the software implementation steps.

# Software Implementation

## Windows

We assume that the `C:\Important` folder contains your important data, and `Y:\` is your backup drive (you can map this drive using SMB, SSHFS, or any other method).

We assume you are using Windows 7 / Windows Server 2008 x64 or later. (If you are on a 32-bit system, please recompile the source code.)

To try this system, build and flash `Arduino_Sample/Arduino_Sample.ino` to your Arduino board, and make sure the board is assigned to COM port `COM3`. Then double-click `trigger_serial.exe` to test, and run `install.bat` to install this software as a scheduled task.

Pls note, the drive `Y:\` should be available at earlier time before the disaster happens. If you are using SMB, you can use `net use Y: \\server\share` to map the drive, and `Y:\` will be become available after you log in. You may enable auto-login by setting the following registry keys:

- "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\AutoAdminLogon" to 1
- "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\DefaultUserName" to your username
- "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\DefaultPassword" to your password

## Linux

Under development.

## MacOS

Under development.

# License
This implementation is under patent CN-B-111901961(CN2020107900105), and the inventor & copyright holder is `ZHANG SHIZHUO`.

If you are interested in implementing this system, please email `zhuoshizhang@hotmail.com` for queries.