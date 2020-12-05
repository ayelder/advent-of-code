// https://adventofcode.com/2020/day/1
#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <fstream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"

// Find two entries that sum to 2020 then multiply them together

std::vector<int> read_file(std::string_view filename) {
    std::vector<int> file_entries;
    std::ifstream input_file_stream;
    std::cout << "Opening " << filename << std::endl;
    input_file_stream.open(std::string(filename), std::ios_base::in);
    std::string current_line;
    while (input_file_stream >> current_line) {
        file_entries.push_back(std::stoi(current_line));
    }
    std::cout << "Read " << file_entries.size() << " entries" << std::endl;
    return file_entries;
}

absl::StatusOr<std::pair<int, int>> find_sum_indices_pair(std::span<int> sorted_entries, int sum_value) {
    int min_index = 0;
    int max_index = sorted_entries.size() - 1;

    while (min_index != max_index) {
        auto sum = sorted_entries[min_index] + sorted_entries[max_index];
        if (sum == sum_value) {
            return std::make_pair(min_index, max_index);
        } else if (sum > sum_value) {
            max_index--;
        } else { // if (sum < sum_value)
            min_index++;
        }
    }
    // TODO: Use absl::StrCat to create a string that actually includes the value
    return absl::NotFoundError(absl::StrFormat("no pair found that sums to provided value (%d)", sum_value));
}

absl::StatusOr<std::tuple<int, int, int>> find_sum_indices_triplet(std::span<int> sorted_entries, int sum_value) {
    for (size_t i = 0; i < sorted_entries.size(); i++) {
        auto current_value = sorted_entries[i];
        // Find a pair that is equal to the requested |sum_value| - our |current_value|, if found that would mean
        // that pair sum + |current_value| = |sum_value| and therefore we have found our match.
        auto potential_match = find_sum_indices_pair(sorted_entries.subspan(i), sum_value - current_value);
        if (potential_match.ok()) {
            // NOTE: need to add |i| to the indices in the pair since they are relative to the subspan starting at |i|.
            return std::make_tuple(i, (*potential_match).first + i, (*potential_match).second + i);
        }
    }
    return absl::NotFoundError(absl::StrFormat("no triplet found that sums to provided value (%d)", sum_value));
}

int main(int argc, char const *argv[])
{
    // TODO: Figure out how to make this programattic instead of hardcoding
    // auto input_filepath = std::filesystem::current_path() / "day_1_input.txt";
    // std::cout << input_filepath.string() << std::flush;
    auto file_entries = read_file("/home/drew/workspace/advent-of-code/day_1_input.txt");
    // Brute force method would be O(n^2) where we compare every single entry with every other entry.
    // More elegant solution would be sort the entries then keep two indices that move inwards until
    // we find the sum we want.
    std::sort(file_entries.begin(), file_entries.end());
    { // FIRST PART
        auto index_pair = find_sum_indices_pair({&file_entries[0], file_entries.size()}, 2020);
        if (!index_pair.ok()) {
            return -1;
        }
        const auto& [first_index, second_index] = *index_pair;
        auto first_value = file_entries[first_index];
        auto second_value = file_entries[second_index];
        std::cout << first_value << " + " << second_value << " = 2020" << std::endl;
        std::cout << first_value << " * " << second_value << " = " << first_value * second_value << std::endl;
    }

    { // SECOND PART
        auto index_triplet = find_sum_indices_triplet({&file_entries[0], file_entries.size()}, 2020);
        if (!index_triplet.ok()) {
            return -1;
        }
        const auto& [first_index, second_index, third_index] = *index_triplet;
        auto first_value = file_entries[first_index];
        auto second_value = file_entries[second_index];
        auto third_value = file_entries[third_index];
        std::cout << first_value << " + " << second_value << " + " << third_value << " = 2020" << std::endl;
        std::cout << first_value << " * " << second_value << " * " << third_value << " = " << first_value * second_value * third_value << std::endl;
    }

    return 0;
}




