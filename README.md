# M1M3 Thermal System

Realtime software which runs on NI cRIO regulating M1M3 temperature.
[FPGA](https://github.com/lsst-ts/ts_m1m3thermalFPGA) handling low level
communication is a separate project.

The application uses same infrastructure as the [M1M3 Support
System](https://github.com/lsst-ts/ts_m1m3support).

# Thermal system operation

Thermal system consists of 96 blowers, glycol heat exchange loop and glycol
pumps. System is designed to keep mirror temperature as close to ambient to
improve seeing. Seeing can be degraded by turbulent air created over hotter
than ambient ares.

Mirror temperature is sensed by thermocouples located below, in and above
mirror. Thermal system can regulate incoming glycol temperature mix, speed of
fans blowing air on the mirror and booster heaters located at blowers exit
nozzle.

# Make Targets

- all: build the application
- ts_M1M3Thermal: builds M1M3 TS CsC
- clean: remove build artefacts
- deploy: deploy CsC on cRIO
- tests: build tests
- run_tests: build and run tests
- junit: build and run tests, produce JUnit xml output
- doc: build Doxygen documentation
