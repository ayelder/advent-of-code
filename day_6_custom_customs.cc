#include <iostream>
#include <functional>
#include <fstream>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

// The form asks a series of 26 yes-or-no questions marked a through z.
// All you need to do is identify the questions for which anyone in your group answers "yes".

// Each group's answers are separated by a blank line, and within each group,
// each person's answers are on a single line.

// Part 1
// For each group, count the number of questions to which anyone answered "yes".
// What is the sum of those counts?

// Part 2
// For each group, count the number of questions to which everyone answered "yes".
// What is the sum of those counts?

template <typename Entry>
std::vector<Entry> read_file(std::string_view filename, std::function<Entry(std::string_view)> create_entry_fn)
{
    std::vector<Entry> entries;
    std::ifstream input_file_stream;
    std::cout << "Opening " << filename << std::endl;
    input_file_stream.open(std::string(filename), std::ios_base::in);
    std::string current_line;
    while (getline(input_file_stream, current_line))
    {
        entries.push_back(create_entry_fn(current_line));
    }
    std::cout << "Read " << entries.size() << " entries" << std::endl;
    return entries;
}

// We want to have a hash set for each group which allows us to easily get the count after considering
// the members for each group.
// Since groups span multiple lines, the simplest way is just to read in as simple vector of strings
// then iterate over them again to aggregate into groups.
std::vector<std::unordered_set<char>> aggregate_into_groups_anyone(std::span<std::string> answer_lines)
{
    // Seed vector with a starting set to make the loop easier
    std::vector<std::unordered_set<char>> group_sets = {std::unordered_set<char>()};
    for (std::string line : answer_lines)
    {
        if (line.size() == 0)
        {
            // Empty line means end of the group, create a new empty group set to initialize
            // the processing of the next group.
            group_sets.push_back(std::unordered_set<char>());
        }
        else
        {
            // Update the current group's set with this passenger's answers
            auto &cur_set = group_sets.back();
            for (char answer : line)
            {
                cur_set.insert(answer);
            }
        }
    }
    return group_sets;
}

std::vector<std::unordered_set<char>> aggregate_into_groups_everyone(std::span<std::string> answer_lines)
{
    std::vector<std::unordered_set<char>> group_sets = {};
    bool new_group = true;
    for (std::string line : answer_lines)
    {
        if (line.size() == 0)
        {
            new_group = true;
            continue;
        }

        // Seed the new group with the first answer, then successively remove
        // any answers that are not seen by any following lines for that group
        if (new_group)
        {
            std::unordered_set<char> new_group_set;
            for (char answer : line)
            {
                new_group_set.insert(answer);
            }
            group_sets.push_back(new_group_set);
            new_group = false;
        }
        else
        {
            // Update the current group's set with this passenger's answers
            std::unordered_set<char> cur_passenger_set;
            for (char answer : line)
            {
                cur_passenger_set.insert(answer);
            }
            std::unordered_set<char> updated_group_set = {};
            std::unordered_set<char> &cur_group_set = group_sets.back();
            for (char answer_so_far : cur_group_set)
            {
                // Only set the updated group with anwers that everyone has
                // answered yes to so far and this passenger answered yes to.
                if (cur_passenger_set.contains(answer_so_far))
                {
                    updated_group_set.insert(answer_so_far);
                }
            }
            // Swap the updated group with the current group
            cur_group_set.swap(updated_group_set);
        }
    }
    return group_sets;
}

int main(int argc, char const *argv[])
{
    // I needed to explicitly convert in order to pass it to read_file (as opposed to inlining the lambda)
    // in order to make the compiler happy
    std::function<std::string(std::string_view)> create_fn = [](std::string_view view) { return std::string(view); };
    std::vector<std::string> file_input = read_file("/home/drew/workspace/advent-of-code/day_6_input.txt",
                                                    create_fn);
    { // PART ONE: Anyone answered yes in a group
        std::vector<std::unordered_set<char>> group_sets =
            aggregate_into_groups_anyone({&file_input[0], file_input.size()});
        int answer_count = 0;
        for (auto &set : group_sets)
        {
            answer_count += set.size();
        }
        std::cout << "Num groups: " << group_sets.size() << std::endl;
        std::cout << "Total answer count(anyone): " << answer_count << std::endl;
    }
    { // PART TWO: Everyone answered yes in a group
        std::vector<std::unordered_set<char>> group_sets =
            aggregate_into_groups_everyone({&file_input[0], file_input.size()});
        int answer_count = 0;
        for (auto &set : group_sets)
        {
            answer_count += set.size();
        }
        std::cout << "Num groups: " << group_sets.size() << std::endl;
        std::cout << "Total answer count (everyone): " << answer_count << std::endl;
    }

    return 0;
}