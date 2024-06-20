parser $\rightarrow$ kins $\rightarrow$ interpolator

* The parser needs to output the path in task space with respect to the s parameter, $p_T(s)$.

* The kinematic solver ("kins") needs to output the path in joint space with respect to the s parameter, $p_J(s)$.

* The interpolator needs to output the path in joint space with respect to time, $p_J(s(t))$.

```c
// rtmc_parser.h
rtmc_path_t rtmc_parse(
    const char* block, const double* start_coords
);
```

```c
// rtmc_kins.h
// Note: there will be many "kins" implementations available
void rtmc_setup_kins(rtmc_path_t path);
double* rtmc_kins_get_pose(double s);
```

```c
// rtmc_interpolator.h

// returns durations of the motion (t value when s=1)
double rtmc_setup_interpolate(double* (*pose_s)(double));

double* rtmc_get_pose(double s);
```

Some pseudocode using this library:
```c
// main.c

#include "rtmc_parser.h"
#include "rtmc_kins.h"
#include "rtmc_interpolator.h"

int main() {
    // set up the g-code block
    char block[] = "G01 X100 Y250 F100";
    double start_coords[RTMC_NUM_AXES] = {0};

    // parse the block
    rtmc_path_t task_path = rtmc_parse(block, start_coords);

    // set up kins
    rtmc_setup_kins(task_path);

    // interpolate
    double duration = rtmc_interpolate(rtmc_kins_get_pose);

    // instruct motion to interpolated pose
    double step = 0.01; // unit: seconds
    for(double time = 0.0; time <= duration; time += step) {
        double pose = rtmc_get_pose(time);
        rtmc_send_instruction_to_fpga(pose);
    }

    return 0;
}
```
