# RTMC G-Codes
The RTMC Library is actively under development and is not yet ready for production use. These G-codes and their exact syntax are subject to change.



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
        <td>Rapid linear path</td>
        <td>Motion</td>
        <td>Supported</td>
    </tr>
    <tr>
        <td>G01</td>
        <td>Linear path</td>
        <td>Motion</td>
        <td>Supported</td>
    </tr>
    <tr>
        <td>G02</td>
        <td>Circular path (clockwise)</td>
        <td>Motion</td>
        <td>In Progress</td>
    </tr>
    <tr>
        <td>G03</td>
        <td>Circular path (counter-clockwise)</td>
        <td>Motion</td>
        <td>In Progress</td>
    </tr>
    <tr>
        <td>G17</td>
        <td>Select XY plane</td>
        <td>Plane</td>
        <td>Supported</td>
    </tr>
    <tr>
        <td>G18</td>
        <td>Select XZ plane</td>
        <td>Plane</td>
        <td>Untested<sup>1</sup></td>
    </tr>
    <tr>
        <td>G19</td>
        <td>Select YZ plane</td>
        <td>Plane</td>
        <td>Untested<sup>1</sup></td>
    </tr>
    <tr>
        <td colspan="4">
        <strong>Notes</strong><br/>
        1. G18 and G19 planes might be backwards (cause clockwise arcs to move counter-clockwise)
        </td>
    </tr>
</table>



## General Syntax
A single line (a.k.a., "block") of g-code is made up of one or more "words". A word is a key/value pair where the key is a single character and the value is a positive number. For example, the word "G17" has a key of "G" and a value of "17".

Keys can be in either lowercase or uppercase, and the value can optionally have a leading zero. "g0", "G0", and "G00" all mean the same thing.

In many cases, the values will have decimal points. "X10", and "X10.0000" are both valid words. Scientific notation is also allowed using the "e" character; for example, "X1000", "X1e3", and "X1.0E3" are all equivalent.

The values used must be kept under 19 characters, including the sign, decimal, "e" character, etc. If a value is given that is longer than 19 characters, an error will be thrown.



## Mathematic Background
### Polynomial Form
$$p_i\left(s\right) = A_is^3 + B_is^2 + C_is + D_i$$



### Trigonometric Form
$$p_i\left(s\right) = A_i\sin\left(B_i\left(s - C_i\right)\right) + D_i$$



## G00, G01 - Linear Paths
These commands move in a linear path to the given point. G00 commands move at the maximum possible feed rate. G01 commands move at the feed rate set by the F-word. If no feed rate is set before the G01 command, an error will be thrown.

The parsed path is returned in polynomial form with the following coefficients
$$A_i = B_i = 0$$
$$C_i = p_i\left(1\right) - p_i\left(0\right)$$
$$D_i = p_i\left(0\right)$$

where $p_i\left(0\right)$ and $p_i\left(1\right)$ are given for all $i$.



## G02, G03 - Circular Paths
These commands move in a circular path or arc. G02 commands move clockwise, and G03 commands move counter-clockwise. There are two forms of these commands: the IJK form, and the R form. If an R-word is used along side any of the I, J, or K words, an error will be thrown. 

Circular paths are traversed along a selected plane. Planes are selected through the G17, G18, and G19 commands.

### IJK Form
The IJK form uses the I, J, and K words to define a center point to rotate around. This is the preferred form.

While this form is preferred, it is overconstrained. This means the programmed end-point might not equal the end-point of the motion. In this case, the position error is reported, but no error is thrown.

The parsed path is returned in trigonometric form with the following coefficients (defined in terms of the the XY plane)

$$
A=\sqrt{\left(D_{x}-x_{0}\right)^{2}+\left(D_{y}-y_{0}\right)^{2}}
$$

$$
B =
\begin{cases}
    B_{base}-2\pi & \text{CW and }B_{sign}\ge0\ \\
    -B_{base} & \text{CW and }B_{sign}<0 \\
    B_{base} & \text{CCW and }B_{sign}>0\ \\
    2\pi-B_{base} & \text{CCW and }B_{sign}\le0 \\
\end{cases}
$$

$$
C_y =
\begin{cases}
    \frac{1}{B}\arccos\left(\frac{x_{0}-D_{x}}{A}\right) & D_y-y_0\ge0 \\
    -\frac{1}{B}\arccos\left(\frac{x_{0}-D_{x}}{A}\right) & D_y-y_0<0
\end{cases}
$$

$$
C_{x}=C_{y}-\frac{\pi}{2B}
$$

where

$$
B_{base}=\arccos\left(\frac{\left(D_{x}-x_{0}\right)\left(D_{x}-x_{1}\right)+\left(D_{y}-y_{0}\right)\left(D_{y}-y_{1}\right)}{A\sqrt{\left(D_{x}-x_{1}\right)^{2}+\left(D_{y}-y_{1}\right)^{2}}}\right)
$$

$$
B_{sign}=\left(D_{x}-x_{0}\right)\left(D_{y}-y_{1}\right)-\left(D_{y}-y_{0}\right)\left(D_{x}-x_{1}\right)
$$

### R Form
[TODO]

### Helical Paths
[TODO]



## G17, G18, G19 - Plane Selection
G17 selects the XY plane; G18 selects the XZ plane; G19 selects the YZ plane.