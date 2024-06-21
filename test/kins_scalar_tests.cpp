#include <gtest/gtest.h>
#include "rtmc_kins_scalar.h"
#include "rtmc_math.h"
#include "rtmc_parser.h"

// TODO: name this test
TEST(KinsScalarTests, TestName) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_path_t path;

    // create a path
    rtmc_parse(&queue, "G00 X100");
    path = rtmc_path_dequeue(&queue);

    // set scale factors
    double scale_factors[RTMC_NUM_AXES];
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        scale_factors[i] = -2;
    }

    // set up the kinematics
    rtmc_kins_scalar_setup(scale_factors);
    rtmc_kins_scalar_load(path);

    // validate the pose vectors
    double pose[RTMC_NUM_AXES];
    rtmc_kins_scalar_pose(pose, 0);
    EXPECT_TRUE(rtmc_is_equal(pose[RTMC_X_AXIS], 0));
    rtmc_kins_scalar_pose(pose, 0.5);
    EXPECT_TRUE(rtmc_is_equal(pose[RTMC_X_AXIS], -100));
    rtmc_kins_scalar_pose(pose, 1);
    EXPECT_TRUE(rtmc_is_equal(pose[RTMC_X_AXIS], -200));
}
