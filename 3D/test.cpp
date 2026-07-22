#include "3D_slice_tracker.hh"
#include "bmp.hh"
#include "first_cuboid_test.hh"
#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <vector>

int main() {
  std::vector<double> data;

  auto data_alloc_start = std::chrono::steady_clock::now();
  std::array<std::size_t, 3> dims = {300, 300, 300};

  for (int z = dims[2]; z > 0; z--) {
    for (int y = dims[1]; y > 0; y--) {
      for (int x = dims[0]; x > 0; x--) {
        data.push_back(static_cast<double>(x * y * z));
      }
    }
  }
  // for (int i = 0; i < dims[0] * dims[1] * dims[2]; i++)
  //   data.push_back(i);

  auto data_alloc_end = std::chrono::steady_clock::now();

  auto tracker_x = Insitu::SliceTracker3D<double, 0>(dims, 0, "x_slice", 1, 1);
  auto tracker_y = Insitu::SliceTracker3D<double, 1>(dims, 0, "y_slice", 1, 1);
  auto tracker_z = Insitu::SliceTracker3D<double, 2>(dims, 0, "z_slice", 1, 1);
  auto colour_range =
      Colours::ColourRange<double>(data[data.size() - 1], data[0]);

  auto generate_start = std::chrono::steady_clock::now();

  auto x_image = tracker_x.generate_graph(data.data(), colour_range).value();
  auto y_image = tracker_y.generate_graph(data.data(), colour_range).value();
  auto z_image = tracker_z.generate_graph(data.data(), colour_range).value();

  auto generate_end = std::chrono::steady_clock::now();

  auto rasteriser_start = std::chrono::steady_clock::now();

  auto faces =
      std::array<Cuboid::Face<double>, 3>{Cuboid::get_faces_of_cuboid<double>(
          {{{dims[0], dims[1]}, {dims[2], dims[1]}, {dims[2], dims[0]}}})};

  auto camera = Cuboid::get_position_camera(faces);
  auto test_buffer = Cuboid::get_pixel_buffer(
      {1000, 1000}, camera, faces, {z_image, x_image, y_image},
      {{{300, 300}, {300, 300}, {300, 300}}});

  Writer::write_bmp("3d_render.bmp", test_buffer, 1000, 1000);

  auto rasteriser_end = std::chrono::steady_clock::now();

  std::cout << "Array Population (ms): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   data_alloc_end - data_alloc_start)
                   .count()
            << std::endl;

  std::cout << "Graph Generation (ms): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   generate_end - generate_start)
                   .count()
            << std::endl;
  std::cout << "Rasteriser Image Generation (ms): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   rasteriser_end - rasteriser_start)
                   .count()
            << std::endl;
}
