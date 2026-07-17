#ifndef OPS_DAT_TO_PRIMITIVES_HH
#define OPS_DAT_TO_PRIMITIVES_HH
/*
 *
 * UTILS
 *
 */

#include <optional>
#include <string>
#include <vector>
namespace Utils {

auto split_line_on_delimiter(const std::string &line, char delimiter)
    -> std::vector<std::string>;

auto extract_numbers_from_block_dims(const std::string &piece,
                                     std::size_t num_dims)
    -> std::optional<std::vector<std::size_t>>;
} // namespace Utils

namespace ops_dat_to_prim {
/*
 *
 * Gets the data as actual vectors of data
 *
 */
auto interpret_dat_to_vec_with_dims(const std::string &file_name)
    -> std::optional<std::pair<std::vector<std::size_t>, std::vector<double>>>;

} // namespace ops_dat_to_prim
#endif
