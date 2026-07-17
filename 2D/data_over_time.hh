#ifndef OPS_INSITU_DATA_OVER_TIME_CONTAINER
#define OPS_INSITU_DATA_OVER_TIME_CONTAINER

#include <vector>
template <typename T> class DataOverTimeContainer {
public:
  DataOverTimeContainer() : points(std::vector<T>{}) {}

  auto get_points() const -> const std::vector<T> &;
  auto push_back(T new_val) -> void;
  auto emplace_back(T &&new_val) -> void;

private:
  std::vector<T> points;
};

template <typename T>
auto DataOverTimeContainer<T>::get_points() const -> const std::vector<T> & {
  return points;
}

template <typename T>
auto DataOverTimeContainer<T>::push_back(T new_val) -> void {
  points.push_back(new_val);
}

template <typename T>
auto DataOverTimeContainer<T>::emplace_back(T &&new_val) -> void {
  points.emplace_back(new_val);
}

#endif
