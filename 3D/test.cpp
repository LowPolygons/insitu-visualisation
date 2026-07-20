#include "3D_slice_tracker.hh"
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
}
