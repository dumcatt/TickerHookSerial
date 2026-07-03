# TickerHookSerial
Hook dll for capturing ticker text from IIDX. Sends data via serial.

This is a fork of [TickerHook](https://github.com/Radioo/TickerHook).

# Usage
Copy appropriate .dll file into your base directory, then load the dll using your tools, examples below:

inject.exe:  
`inject iidxhook1.dll TickerHook.dll bm2dx.exe --config iidxhook-09.conf %*`  
launcher.exe:  
`launcher -H 134217728 -B iidxhook9.dll -K TickerHook.dll bm2dx.dll --config iidxhook-28.conf %*`  
spice.exe/spice64.exe:  
`spice64 -k TickerHook.dll`  

After this, configure the COM port by creating a `tickerhook.conf` file. 

```
PORT=COM1
BAUD=115200
```
