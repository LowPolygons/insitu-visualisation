#include "ascii_graph.hh"
#include "data_over_time.hh"
#include "ops_dat_to_primitives.hh"
#include <cassert>
#include <cstddef>
#include <iostream>
auto main() -> int {
  auto num_files = 1;
  auto file_lists = std::vector<std::string>{};

  for (int i = 0; i < num_files; i++)
    file_lists.emplace_back(
        std::string{"2d_test_data" + std::to_string(i) + ".dat"});

  if (file_lists.size() == 0)
    return -1;

  auto initial_data =
      ops_dat_to_prim::interpret_dat_to_vec_with_dims(file_lists[0]);

  if (!initial_data.has_value()) {
    std::cout << "Failed to interpret data" << std::endl;
    return 1;
  }

  auto domain_size = initial_data.value().first;

  auto target_point_pos = std::pair<std::size_t, std::size_t>{50, 75};
  assert(target_point_pos.first < domain_size[0]);
  assert(target_point_pos.second < domain_size[1]);

  auto data_container = DataOverTimeContainer<double>{};

  for (auto file_name : file_lists) {
    auto data = ops_dat_to_prim::interpret_dat_to_vec_with_dims(file_name);

    assert(data.has_value());
    assert(data.value().first[0] == domain_size[0]);
    assert(data.value().first[1] == domain_size[1]);

    data_container.push_back(
        data.value().second[(target_point_pos.second * domain_size[0]) +
                            target_point_pos.first]);
  }

  std::cout << "Values Over Time: " << std::endl;

  for (auto point : data_container.get_points()) {
    std::cout << "- " << point << std::endl;
  }

  auto graph = Grapher("Iteration", "Something", 500, 50);

  graph.print_graph(data_container.get_points());
}

// if (i == 50) {
//     // char* data;
//     // ops_dat_fetch_data(d_wprime, 0, data);
//
//     // double* double_data = reinterpret_cast<double*>(data);
//
//
//     // std::cout << data << std::endl;
//     // for (int iter = 0; iter < ny * nz; iter++) {
//     //     std::cout << double_data[iter];
//     //
//     //     if (iter % ny == 0)
//     //         std::cout << "\n";
//     // }
// }
