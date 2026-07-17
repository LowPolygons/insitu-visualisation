#ifndef OPS_INSITU_VISUALISATION_BLOCK_TRACKER_HH
#define OPS_INSITU_VISUALISATION_BLOCK_TRACKER_HH

#include "bmp.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

namespace Graphing {

const static auto DEFAULT_COLOUR_RANGE = std::vector<std::string>{
    "\x1B[100m", "\x1B[101m", "\x1B[103m", "\x1B[102m",
    "\x1B[104m", "\x1B[105m", "\x1B[106m", "\x1B[107m",
};

template <typename T> class InsituBlockTracker2D {
  static_assert(std::is_arithmetic<T>::value,
                "Template argument T to InsitueBlockTracker2D must pass "
                "std::is_arithmetic_v<T>");

public:
  InsituBlockTracker2D(std::pair<std::size_t, std::size_t> block_size,
                       std::string output_files_header,
                       std::pair<std::size_t, std::size_t> output_size,
                       std::size_t graph_every_n = 1)
      : block_size(block_size), file_header(output_files_header),
        output_size(output_size), graph_every(graph_every_n),
        colour_range(DEFAULT_COLOUR_RANGE), num_calls(0), max(255) {};

  auto set_colour_range(std::vector<std::string> new_colour_range) -> void;

  auto generate_graph(T *const &flattened_block) -> bool;

private:
  std::pair<std::size_t, std::size_t> block_size;
  std::string file_header;
  std::pair<std::size_t, std::size_t> output_size;
  std::size_t graph_every;
  std::vector<std::string> colour_range;
  std::size_t num_calls;
  std::uint8_t max;
};

template <typename T>
auto InsituBlockTracker2D<T>::set_colour_range(
    std::vector<std::string> new_colour_range) -> void {
  colour_range = new_colour_range;
}

template <typename T>
auto InsituBlockTracker2D<T>::generate_graph(T *const &flattened_block)
    -> bool {

  if (colour_range.size() == 0) {
    std::cout << "Colour Range must contain at least one value" << std::endl;
    return false;
  }

  // Calculate the minimum and maximum values to map the colour range to the
  // particle range
  auto min_and_max = std::pair<T, T>{T{0}, T{0}};

  for (std::size_t i = 0; i < block_size.first * block_size.second; i++) {
    min_and_max.first = flattened_block[i] < min_and_max.first
                            ? flattened_block[i]
                            : min_and_max.first;
    min_and_max.second = flattened_block[i] > min_and_max.second
                             ? flattened_block[i]
                             : min_and_max.second;
  }

  // TODO: Safely implement it so the highest 5% of values are the highest
  // colour
  auto point_range = (min_and_max.second - min_and_max.first);

  auto get_colour_from_value = [point_range, this,
                                min_and_max](T value) -> std::string {
    if (std::abs(point_range) < 1e-06)
      return colour_range[0];

    auto chosen_colour_index = static_cast<int>(
        (std::max(T{0}, (value - min_and_max.first)) / point_range) *
        static_cast<int>(colour_range.size()));

    return colour_range[std::min(int{colour_range.size() - 1},
                                 std::max(0, chosen_colour_index - 1))];
  };

  auto get_colour_num_from_value = [point_range, this,
                                    min_and_max](T value) -> std::uint8_t {
    if (std::abs(point_range) < 1e-06)
      return 0;

    auto chosen_colour = static_cast<std::uint8_t>(
        (std::max(T{0}, (value - min_and_max.first)) / point_range) *
        static_cast<int>(max));

    return chosen_colour;
  };

  auto graph = std::vector<std::string>{};
  auto graph_nums = std::vector<std::uint8_t>{};

  for (std::size_t y = 0; y < block_size.second; y++) {
    for (std::size_t x = 0; x < block_size.first; x++) {
      graph.push_back(
          get_colour_from_value(flattened_block[(y * block_size.first) + x]));
      graph_nums.push_back(get_colour_num_from_value(
          flattened_block[(y * block_size.first) + x]));
    }
  }

  Writer::write_bmp("bmp_output" + std::to_string(num_calls) + ".bmp",
                    graph_nums, block_size.first, block_size.second);

  num_calls++;

  return true;
}

} // namespace Graphing

#endif

// auto output_file = std::ofstream(file_header + std::to_string(num_calls));
//
// if (!output_file.is_open())
//   return false;
//
// auto points_counter = std::size_t{0};
// for (auto point : graph) {
//   if (points_counter % block_size.first == 0) {
//     output_file << "\n";
//   }
//   output_file << point;
//
//   points_counter++;
// }
//
// output_file.close();
