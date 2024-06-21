## TODO List
* Helical motion
* R-type arcs
* Make parser add to a path queue (to support canned cycles)

## Refactoring
* Rename `xxx_coords` to `xxx_pose`

## Errors that will crop up
* Currently, G02/G03 instructions don't set the 4th coefficient to constant
for non-moving axes.
