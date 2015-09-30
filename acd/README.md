# Advanced Clock Divider

A clock divider for the ArdCore + Expander supporting:

* Simultaneous clock outputs at all the expander triggers at different clock divisions.
* Independent clock reset.
* Clock scaling.

## Usage

* CLK takes the master clock (trigger or short gate should do). The master clock is mirrored to D0.
* A0 provides the rotation R (from 0 to 64).
* A3 takes a reset trigger. Reset is mirrored on D1.
* A2 scales the master clock. Takes a 0-5V DC input or you can use the A2 attenuator.

### Scaling (A2)

The scaling is not uniform: The master clock is scaled by multiplying with:

* 0.25 (4x faster clock) when at 0V (attenuator all the way anti-clockwise)
* 1.0 at 2.5V (attenuator at noon)
* 2.0 (2x slower clock) when at 5V (attenuator turned all the way clockwise)
* on between values are scale the clock in 0.25 multiplication increments, so [0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0].

### Clock outputs

The clocks outputed at the expander gates are as follows:

* 1: ``(4 + R) x Clock``
* 2: ``(3 + R) x Clock``
* 3: ``(2 + R) x Clock``
* 4: ``(1 + R) x Clock``
* 5: ``1 / (1 + R) x Clock``
* 6: ``1 / (2 + R) x Clock``
* 7: ``1 / (3 + R) x Clock``
* 8: ``1 / (4 + R) x Clock``

where ``R`` is the rotation and ``Clock`` is the scaled clock

## License

Copyright (C) Yiorgis Gozadinos. Distributed under the MIT License.
