## TODO List
* Helical motion
* R-type arcs
* Make parser add to a path queue (to support canned cycles)

## Refactoring
* Get away from `typedef`s
* For the tests, use `EXPECT_DOUBLE_EQ` instead of RTMC method
* Rename `xxx_coords` to `xxx_pose`

## Errors that will crop up
* `start_coords` is not defined in `parser.c`
* !!!! `generate_path` thinks that the programmed `end_coords` are correct for
G02/G03 moves. It should be the `actual_end_coords`
* nothing is really setting `start_coords` or `end_coords`