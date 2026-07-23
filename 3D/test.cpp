#include "3D_slice_tracker.hh"
#include "bmp.hh"
#include "first_cuboid_test.hh"
#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <random>
#include <vector>

int main() {
  auto data_alloc_start = std::chrono::steady_clock::now();

  auto rand_device = std::random_device{};
  auto rand_gen = std::mt19937{rand_device()};
  auto rand_distribution = std::uniform_int_distribution<std::size_t>(1, 2000);

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

  auto data_alloc_end = std::chrono::steady_clock::now();

  auto tracker_x =
      Insitu::SliceTracker3D<double, 0>(dims, dims[0] - 1, "x_slice", 1, 1);
  auto tracker_y =
      Insitu::SliceTracker3D<double, 1>(dims, dims[1] - 1, "y_slice", 1, 1);
  auto tracker_z = Insitu::SliceTracker3D<double, 2>(dims, 0, "z_slice", 1, 1);
  auto colour_range =
      Colours::ColourRange<double>(0.0, dims[0] * dims[1] * dims[2]);

  auto generate_start = std::chrono::steady_clock::now();

  auto x_image_and_x_dims =
      tracker_x.generate_graph(data.data(), colour_range).value();
  auto y_image_and_y_dims =
      tracker_y.generate_graph(data.data(), colour_range).value();
  auto z_image_and_z_dims =
      tracker_z.generate_graph(data.data(), colour_range).value();

  auto generate_end = std::chrono::steady_clock::now();

  auto rasteriser_start = std::chrono::steady_clock::now();

  auto faces = std::array<Cuboid::Face<double>, 3>{
      Cuboid::get_faces_of_cuboid<double>(dims)};

  auto camera = Cuboid::get_position_camera<double>(dims);

  constexpr auto CUBOID_RENDER_IMAGE_SIZE =
      std::pair<std::size_t, std::size_t>{1500, 1500};

  auto cuboid_pixel_buffer = Cuboid::get_pixel_buffer(
      {CUBOID_RENDER_IMAGE_SIZE.first, CUBOID_RENDER_IMAGE_SIZE.second}, camera,
      faces,
      {std::get<0>(z_image_and_z_dims), std::get<0>(x_image_and_x_dims),
       std::get<0>(y_image_and_y_dims)},
      {std::get<1>(z_image_and_z_dims), std::get<1>(x_image_and_x_dims),
       std::get<1>(y_image_and_y_dims)});

  Writer::write_bmp("3d_render.bmp", cuboid_pixel_buffer,
                    CUBOID_RENDER_IMAGE_SIZE.first,
                    CUBOID_RENDER_IMAGE_SIZE.second);

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
