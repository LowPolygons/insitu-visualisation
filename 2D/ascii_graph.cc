#include "ascii_graph.hh"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>

auto Grapher::print_graph(const std::vector<double> &data) -> void {
  auto lines = std::vector<char>(graph_size.first * graph_size.second);

  for (auto &p : lines)
    p = ' ';

  std::cout << "Size: " << graph_size.first << ", " << graph_size.second
            << std::endl;
  auto register_point = [&lines, this](std::size_t x, std::size_t y) {
    lines[(((graph_size.second - 1) - y) * graph_size.first) + x] = '#';
  };

  // calculate range of upper and lower
  auto upper = double{data[0]};
  auto lower = double{data[0]};

  std::cout << "Upper, Lower: " << upper << ", " << lower << std::endl;

  for (auto dat : data) {
    upper = upper > dat ? upper : dat;
    lower = lower < dat ? lower : dat;
  }

  std::cout << "Upper, Lower: " << upper << ", " << lower << std::endl;

  double x_scale = static_cast<double>(graph_size.first) * 0.9 / data.size();
  double y_scale =
      static_cast<double>(graph_size.second) * 0.9 / (upper - lower);

  std::cout << "X Scale, Y Scale: " << x_scale << ", " << y_scale << std::endl;

  // Now register the points
  auto index = 0;
  for (auto dat : data) {
    index++;
    std::cout << "Scaled [X, Y]: " << x_scale * index << ", "
              << (y_scale * (dat + lower)) << std::endl;
    std::cout << "Rounded [X, Y]: " << static_cast<std::size_t>(x_scale * index)
              << ", " << static_cast<std::size_t>(y_scale * (dat + lower))
              << std::endl;
    try {
      register_point(static_cast<std::size_t>(x_scale * index),
                     static_cast<std::size_t>(y_scale * (dat + lower)));
    } catch (std::exception e) {
      std::cout << "bleg" << std::endl;
    }
  }

  // Print
  // system("clear");
  std::cout << x_label << " vs " << y_label << std::endl;

  auto current_line = std::string{""};

  std::cout << lines.size() << std::endl;
  for (auto character : lines) {
    current_line = current_line + character;

    if (current_line.size() == graph_size.first) {
      std::cout << "|" << current_line << std::endl;
      current_line = "";
    }
  }

  current_line = "";
  for (int i = 0; i < graph_size.first; i++) {
    current_line = current_line + "_";
  }

  std::cout << "|" << current_line << std::endl;

  std::cout << "got here" << std::endl;
}
