mkdir C:\WaterDisasterBackupProgram
copy *.exe C:\WaterDisasterBackupProgram

schtasks /create /tn "WaterDisasterBackup" /tr "C:\WaterDisasterBackupProgram\trigger_serial.exe" /sc minute /mo 1 /f
schtasks /run /tn "WaterDisasterBackup"