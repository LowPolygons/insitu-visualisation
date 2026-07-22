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

  for (int i = 0; i < dims[0] * dims[1] * dims[2]; i++)
    data.push_back(i);
  auto data_alloc_end = std::chrono::steady_clock::now();

  auto tracker_x = Insitu::SliceTracker3D<double, 0>(dims, 0, "x.bmp", 3, 1);
  auto tracker_y = Insitu::SliceTracker3D<double, 1>(dims, 0, "y.bmp", 3, 1);
  auto tracker_z = Insitu::SliceTracker3D<double, 2>(dims, 150, "z.bmp", 3, 1);
  auto colour_range = Colours::ColourRange<double>(0.0, 300 * 300 * 300);

  auto generate_start = std::chrono::steady_clock::now();
  tracker_x.generate_graph(data.data(), colour_range);
  tracker_y.generate_graph(data.data(), colour_range);
  tracker_z.generate_graph(data.data(), colour_range);
  auto generate_end = std::chrono::steady_clock::now();

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

  constexpr auto theoretical_cube_x = 750;
  constexpr auto theoretical_cube_y = 250;
  constexpr auto theoretical_cube_z = 250;

  auto faces =
      std::array<Cuboid::Face<double>, 3>{Cuboid::get_faces_of_cuboid<double>(
          {{{theoretical_cube_x, theoretical_cube_y},
            {theoretical_cube_z, theoretical_cube_y},
            {theoretical_cube_z, theoretical_cube_x}}})};

  std::cout << "All Faces: " << std::endl;
  for (auto face : faces) {
    Cuboid::printFace(face);
  }

  auto camera = Cuboid::get_position_camera(faces);

  std::cout << Cuboid::stringVec(camera.origin) << std::endl;
  std::cout << Cuboid::stringVec(camera.direction) << std::endl;

  auto test_buffer = Cuboid::get_pixel_buffer({1000, 1000}, camera, faces);

  Writer::write_bmp("EXAMPLE", test_buffer, 1000, 1000);
}
