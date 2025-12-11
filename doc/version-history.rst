.. _Version_History:

===============
Version History
===============

v2.8.0
------

* Read flow meter identification.

v2.7.0
------

* OrificesDiameter in AirNozzle table.
* Uses EGW Pump's VFD DriveStatus2 event.
* Additional MixingValveSettings parameters.
* Fixed Enabled: false options for FlowMeter and GlycolPump.

v2.6.0
------

* Renamed ModbusBuffer::reset, change setFrequency to set_frequency.
* Properly read out flow meter.
* Fixed Enabled: false options for FlowMeter and GlycolPump

v2.5.1
------

* Bug fix - send loaded AirNozzles settings.

v2.5.0
------

* EGW pump auto recovery. Power down the pump in standby state.

v2.4.0
------

* Load and publish AirNozzles settings.
* Don't reset EGW's/Heaters PIDs on small setpoint change.

v2.3.0
------

* Finer mixing valve control.
* Allow heater control in non-engineering enabled mode.

v2.2.0
------

* FCU heaters controlled by PID
* EGW Pump autostart, error stat reporting
* Linear maping of mixing valve encoder voltage to %
* Removed OpenSplice reference

v2.1.0
------

* Panic function, safety checks for above mirror temperature
* Hot config reloading
* Mixing valve PID control
* FcuTarget event
* FCU heater control based on setpoint

v2.0.1
------

* non-Kafka (OSPL) option removed
* Kafka init script + support configuration

v2.0.0
------

* Compiles with Kafka SAL
* ApplySetpoint command, auto-regulate the mixing valve
* Auto-recover ILCs after bus failure
* Improved error response on pump readout

v1.0.0
------

* First official summit release
* Simplified temperature sensor communication
* Fixes threads, reaction to ILC errors

v0.4.0
------

* Simplify MPU (FlowMeter, Pump) communication

v0.3.1
======

* Fixes Docker build

v0.3.0
------

* Uses new cRIOcpp API

v0.2.0
------

* Report thermal status strings
* Reports meaning of ILC status bites
* CH3 control (ILC power on L5)
* Improved makefile

v0.1.0
------

* Basic functionality - FCU communication, FlowMeter and Glycol Pump telemetry & commands
