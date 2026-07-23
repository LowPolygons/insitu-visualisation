# 3D Visualisation

This code will generate a 3D block of values as demonstrated in `Mesh Generation`

From here, it will create a slice in each axis, at the provided index.

These will then be visualised independently and written to their own respective [x/y/z]\_slice0.bmp files for viewing

Following this, a camera perspective is chosen on how to view these meshes as a 3D cuboid.

Surfaces are generated, a camera object is created, and the rasterisation is simulated

#### Note: The camera perspective will _NOT_ automatically pick the correct indexes for the slices such that the output is physically accurate, that is up to the user

## Mesh Generation

An `I x J x K` block has been generated like this:

```cpp
  auto rand_device = std::random_device{};
  auto rand_gen = std::mt19937{rand_device()};
  auto rand_distribution = std::uniform_int_distribution<std::size_t>(50, 1500);

  std::vector<double> data;
  std::array<std::size_t, 3> dims = {rand_distribution(rand_gen),
                                     rand_distribution(rand_gen),
                                     rand_distribution(rand_gen)};

  for (int z = dims[2]; z > 0; z--) {
    for (int y = 0; y < dims[1]; y++) {
      for (int x = 0; x < dims[0]; x++) {
        data.push_back(static_cast<double>(x * y * z));
      }
    }
  }
```

This means that, from the default camera perspective, the hot spot area should be the back, top, right corner

### Defaults

| X Perspective               | Y Perspective            | Z Perspective              |
| --------------------------- | ------------------------ | -------------------------- |
| `CameraPerspectiveX::RIGHT` | `CameraPerspectiveY::UP` | `CameraPerspectiveZ::BACK` |

| X Slice Index | Y Slice Index | Z Slice Index |
| ------------- | ------------- | ------------- |
| `dims[0] - 1` | `dims[1] - 1` | `0`           |

No Slice Image scaling

## Renders

### Slices

For clarification, an '[AXIS] Slice' means that for every point in the slice, the [AXIS] coefficient will be the same

| X Slice                  | Y Slice                  | Z Slice                  |
| ------------------------ | ------------------------ | ------------------------ |
| ![X_Slice](x_slice0.bmp) | ![Y_Slice](y_slice0.bmp) | ![Z_Slice](z_slice0.bmp) |

### 3D Cuboid Render

![Render](3d_render.bmp)
