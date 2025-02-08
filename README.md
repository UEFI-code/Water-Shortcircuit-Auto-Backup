# Auto Data Backup and Circuit Protection for Water Disasters

## Introduction

This project contains a sensor system and software components that can monitor hardware test points' potential or impedance, shut down unnecessary hardware components, and back up data to a remote server in case of water-caused short circuits or other disasters.

# Hardware Implementation

We provide an Arduino-based sensor system, using the serial port for feedback on potential status.

# Software Implementation

## Windows

We assume that the `C:\Important` folder contains your important data, and `Y:\` is your backup drive (you can map this drive using SMB, SSHFS, or any other method).

We assume you are using Windows 7 / Windows Server 2008 x64 or later. (If you are on a 32-bit system, please recompile the source code.)

Simply double-click `trigger_serial.exe` to try out this software, and run `install.bat` to install this software as a scheduled task.

## Linux

Under development.

## MacOS

Under development.

# Acknowledgement
This implementation is under patent CN2020107900105, and the inventor is Zhang Shizhuo.

If you are interested in implementing this system, please email `zhuoshizhang@hotmail.com` for queries.