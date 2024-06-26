## Building and Testing
* Building build files: `cmake -S . -B build`
* Building the library: `cmake --build build`
* Running tests: `ctest --test-dir build`

If that's too much typing, have a look at the `make.py` script!

Building creates `build/rtmc_lib.a`.

## Units
NOTE: this section only applies internally to the code. Users are free to use
whatever units G-code supports!

The code assumes the following units. If other units are used, they must be
converted during the parsing stage.
<table>
    <tr>
        <th>Quantity</th>
        <th>Unit</th>
    </tr>
    <tr>
        <td>Time</td>
        <td>s</td>
    </tr>
    <tr>
        <td>Distance</td>
        <td>m</td>
    </tr>
    <tr>
        <td>Angle</td>
        <td>rad</td>
    </tr>
</table>

These units are strict. For example, all input angles given in degrees or
revolutions must be converted to radians. This includes spindle speed, which
must be in terms of rad/s.

Storing values in consistent units keeps the program logic simple, since
values are only ever converted when stored or retrieved. The alternative would
be to update every value every time a unit changes. Sounds painful. 

## Limitations
* Doubles (represented as strings) must be kept ≤ 19 characters. If used
right, this allows for the maximum precision of the double (15 digits +
1 sign + 1 decimal + 1 E + 1 exponent sign). Large numbers should be expressed in scientific notation (e.g., "1.2E-7").
