
#include "ops_dat_to_primitives.hh"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

/*
 *
 * UTILS
 *
 */

auto Utils::split_line_on_delimiter(const std::string &line, char delimiter)
    -> std::vector<std::string> {
  auto pieces = std::vector<std::string>{};

  auto curr_piece = std::string{};

  for (char character : line) {
    if (character == delimiter) {
      if (curr_piece != "")
        pieces.push_back(curr_piece);

      curr_piece = "";

      continue;
    }
    curr_piece = curr_piece + character;
  }

  pieces.push_back(curr_piece);

  return pieces;
}

auto Utils::extract_numbers_from_block_dims(const std::string &piece,
                                            std::size_t num_dims)
    -> std::optional<std::vector<std::size_t>> {
  auto find_brackets = [](auto string) {
    auto indexes =
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>>{};
    auto index_counter = std::size_t{0};
    for (auto character : string) {
      if (character == '[')
        indexes.first.push_back(index_counter);

      if (character == ']')
        indexes.second.push_back(index_counter);

      index_counter++;
    }

    return indexes;
  };

  auto numbers = std::vector<std::size_t>{};

  auto brackets_indexes = find_brackets(piece);

  if (brackets_indexes.first.size() != brackets_indexes.second.size())
    return std::nullopt;

  for (std::size_t i = 0; i < brackets_indexes.first.size(); i++) {
    auto first = brackets_indexes.first[i] + 1;
    auto len = brackets_indexes.second[i] - first;

    try {
      numbers.push_back(std::stoi(piece.substr(first, len)));
    } catch (std::exception _e) {
      return std::nullopt;
    }
  }

  if (numbers.size() != num_dims)
    return std::nullopt;

  return numbers;
}

/*
 *
 * Gets the data as actual vectors of data
 *
 */
auto ops_dat_to_prim::interpret_dat_to_vec_with_dims(
    const std::string &file_name)
    -> std::optional<std::pair<std::vector<std::size_t>, std::vector<double>>> {
  auto file = std::ifstream(file_name);

  if (!file.is_open())
    return std::nullopt;

  auto curr_line = std::string{};
  auto lines = std::vector<std::string>{};

  auto index_of_block_dims = std::vector<std::size_t>{};
  auto indexes_of_elem_size = std::vector<std::size_t>{};

  auto index_counter = std::size_t{0};
  while (std::getline(file, curr_line)) {
    lines.push_back(curr_line);

    if (curr_line.find("block Dims") != std::string::npos)
      index_of_block_dims.push_back(index_counter);

    if (curr_line.find("elem size") != std::string::npos)
      indexes_of_elem_size.push_back(index_counter);

    index_counter++;
  }

  // ops_dat is seemingly jargon
  // ops_dat dim: may be as well
  // block Dims : N [num][num]..(N times)
  // elem size N: unsure

  auto data_points = std::vector<double>{};

  for (auto block_dim_index : index_of_block_dims) {
    auto pieces =
        Utils::split_line_on_delimiter(lines[block_dim_index], char{' '});

    // std::cout << pieces[0] << std::endl;

    assert(pieces[1] == "Dims");
    assert(pieces[2] == ":");

    auto block_dims = std::size_t{};
    try {
      block_dims = std::stoi(pieces[3]);
    } catch (std::exception e) {
      std::cerr << "could not cast pieces[3]" << std::endl;
      return std::nullopt;
    }

    // This is how many numbers will directly follow elem size
    auto maybe_block_dim_numbers =
        Utils::extract_numbers_from_block_dims(pieces[4], block_dims);

    if (!maybe_block_dim_numbers.has_value()) {
      std::cout << "Maybe block Dim numbers does not have a value" << std::endl;
      return std::nullopt;
    }

    auto block_dim_numbers =
        std::vector<std::size_t>{maybe_block_dim_numbers.value()};

    // The 2nd number is the number of lines
    for (auto nl = 0; nl < block_dim_numbers[1]; nl++) {
      auto line_index = nl + block_dim_index + 2;

      // std::cout << "line: " << lines[line_index] << std::endl;
      auto pieces =
          Utils::split_line_on_delimiter(lines[line_index], char{' '});

      // std::cout << "len : " << pieces.size() << std::endl;

      try {
        for (auto piece : pieces) {
          // std::cout << "piece: " << piece << std::endl;
          data_points.emplace_back(std::stod(piece));
        }
      } catch (std::exception e) {
        std::cerr << "could not cast data" << std::endl;
        return std::nullopt;
      }
    }

    // Until I know why it does 3 iterations for Poisson, only do one
    return std::make_pair(block_dim_numbers, data_points);
  }

  return std::nullopt;
}
