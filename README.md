## Building and Testing
* Build build files: `cmake -S . -B build`
* Build library: `cmake --build build`
* Run tests: `ctest --test-dir build`

If that's too much typing, use the `make.py` script!

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
        <td>mm</td>
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

## Current G-Code Coverage
<table>
    <tr>
        <th>Code</th>
        <th>Function</th>
        <th>Modal Group</th>
        <th>Status</th>
    </tr>
    <tr>
        <td>G00</td>
        <td>Rapid motion</td>
        <td>Motion</td>
        <td>Supported</td>
    </tr>
    <tr>
        <td>G01</td>
        <td>Linear path</td>
        <td>Motion</td>
        <td>Supported</td>
    </tr>
</table>

## Limitations
* Doubles (represented as strings) must be kept ≤ 19 characters. If used
right, this allows for the maximum precision of the double (15 digits +
1 sign + 1 decimal + 1 E + 1 exponent sign). Large numbers should be expressed in scientific notation (e.g., "1.2E-7").
