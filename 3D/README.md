# 3D Work

A crucial problem with the Cuboid rendering right now is the spacial changes between a block and the simulation

The space assumed by the 3D_slice_tracker is assumed such that by decreasing X, Y and Z,
this translates to moving left, down and backward in 3D space

BMP files are a little different - the image assumes a decreasing X, Y and Z translates to moving left, up and backward

## Current Situation

A 300x300x300 block has been generated like this:
```cpp
std::vector<double> data;

std::array<std::size_t, 3> dims = {300, 300, 300};

for (int z = dims[2]; z > 0; z--) {
  for (int y = dims[1]; y > 0; y--) {
    for (int x = dims[0]; x > 0; x--) {
      data.push_back(static_cast<double>(x * y * z));
    }
  }
}
```

This means in the block space, the bottom, back, left corner is 0 and the top, right, forward corner is 300 * 300 * 300

The following Slices are the 0th slice


### Slices

| X | Y | Z |
|----------|----------|----------|
| ![X_Slice](x.bmp0.bmp) | ![Y_Slice](y.bmp0.bmp) | ![Z_Slice](z.bmp0.bmp) |

### Current 3D Cuboid Render

![Render](EXAMPLE)



## Other Issues

You would want to view your block from different perspectives, and these would *have* to shift the different faces
