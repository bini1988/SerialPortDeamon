## SerialPortDeamon ##

**comsrv** is a simple linux deamon for access to serial port from the network written in C. 

## About project ##

The project has 5 modules:
* **cfg:** Configuration module, allows read configuration file and parse execution parameters.
* **com:** COM module, allows open COM file by give name, read and write data by using opened COM port.
* **dm:** Deamon module, allows stransform current linux process into linux deamon.
* **net:** Network module, allows create simple listening network socket.
* **prot:** Protocol module, describe the network and com packets representation.

All these modules are used in main file **comsrv**

## Configuration and autorun ##

The example of **comsrv** configuration file is exist in directory `etc/comsrv.conf`. The file `etc/init.d/rcomsrv`is example of autorun script which is teste on OpenWRT linux OS.

## Compilation ##

File **Makefile.inc** contains compilations options. For compilation of project in project directory type: `make`.


