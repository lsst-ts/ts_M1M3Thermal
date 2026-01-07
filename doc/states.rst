M1M3TS CSC States
=================

M1M3TS CSC implemenets CSC-wide standby, disabled, enabled and fault states.
The engineering state can be entered and exited either in disabled, or enabled
modes.

Mixing valve control
--------------------

Mixing valve can only be opened in enabled state. Mixing valve actual position is either controlled by CSC, or manually.

When CSC control is enable, the mixing valve is opened and closed as needed in
a closed-loop. If:

  * :math:`gt_{in}` is the mirror cell glycol supply temperature
  * :math:`gt_{out}` is the glycol return temperature
  * :math:`gsp` is the GlycolSupplyPercentage configuration parameter
  * :math:`g_{target}` is the target temperature

then

.. math::

    \frac{gsp * gt_{in} + (100 - gsp) * g_{out}}{100} - g_{target}

is the input to PID configured in MixingValve/PID configuration. The PID output is then the new target position of the mixing valve.

When CSC is in engineering mode, only manual control is possible. The operator
than specifies target mixing valve opening ratio through EUI/setMixingValve SAL
command.
