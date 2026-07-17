#ifndef ASCII_GRAPH_HH
#define ASCII_GRAPH_HH

#include <string>
#include <vector>
class Grapher {
public:
  Grapher(std::string x_l, std::string y_l, std::size_t x_s, std::size_t y_s)
      : x_label(x_l), y_label(y_l), graph_size(x_s, y_s) {}

  auto print_graph(const std::vector<double> &data) -> void;

private:
  std::string x_label;
  std::string y_label;

  std::pair<std::size_t, std::size_t> graph_size;
};

#endif
