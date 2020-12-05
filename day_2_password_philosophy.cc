#include <iostream>
#include <fstream>
#include <span>
#include <string_view>
#include <vector>

#include "absl/strings/str_split.h"

// PasswordPolicy encapsulates the corporate password policy which specifies
// the min and max amount of a given letter.
struct PasswordPolicy
{
    int min;
    int max;
    char letter;
};

PasswordPolicy create_password_policy(std::string_view pw_policy_string)
{
    // PasswordPolicy is of the format "N-M l" where N is min, M is max and l is the letter. AFAICT they are all
    // single characters but this implentation allows for the digits to at least be > 1 digit

    // First, split the min-max from the letter
    std::vector<std::string_view> first_split = absl::StrSplit(pw_policy_string, " ");
    // Second half of the split, should only have one character which is the letter that must be found
    char letter = first_split[1][0];
    // Second, split the min-max to get them separately
    std::vector<std::string> second_split = absl::StrSplit(first_split[0], "-");
    return PasswordPolicy{
        .min = std::stoi(second_split[0]),
        .max = std::stoi(second_split[1]),
        .letter = letter};
}

bool check_password(const PasswordPolicy &pw_policy, std::string_view pw)
{
    // std::cout << pw << std::endl;
    int letter_count = 0;
    for (char letter : pw)
    {
        if (letter == pw_policy.letter)
        {
            letter_count++;
        }
    }
    // std::cout << letter_count << std::endl;
    return letter_count >= pw_policy.min && letter_count <= pw_policy.max;
}

void remove_trailing_whitespace(std::string_view& s) {
    // Find the last character that is not whitespace (this should always be found unless it is only whitespace)
    std::size_t trailing_whitespace_pos = s.find_last_not_of(" ");
    // The last position in a string would be size() - 1, meaning if there is no whitespace the position would
    // be size() - 1. Therefore, we should always add 1 to the position to determine how many characters to actually
    // remove.
    s.remove_suffix(trailing_whitespace_pos == std::string::npos ? 0 : s.size() - (trailing_whitespace_pos + 1));
}

void remove_leading_whitespace(std::string_view& s) {
    std::size_t leading_whitespace_pos = s.find_first_not_of(" ");
    s.remove_prefix(leading_whitespace_pos == std::string::npos ? 0 : leading_whitespace_pos);
}

std::vector<std::tuple<PasswordPolicy, std::string>> read_file(std::string_view filename)
{
    std::vector<std::tuple<PasswordPolicy, std::string>> entries;
    std::ifstream input_file_stream;
    std::cout << "Opening " << filename << std::endl;
    input_file_stream.open(std::string(filename), std::ios_base::in);
    std::string current_line;
    while (getline(input_file_stream, current_line))
    {
        std::cout << current_line << std::endl << std::flush;
        std::vector<std::string_view> policy_and_pw = absl::StrSplit(current_line, ":");
        std::string_view pw = policy_and_pw[1];
        remove_leading_whitespace(pw);
        remove_trailing_whitespace(pw);
        std::cout << pw << std::endl << std::flush;
        entries.emplace_back(create_password_policy(policy_and_pw[0]), pw);

    }
    std::cout << "Read " << entries.size() << " entries" << std::endl;
    return entries;
}

int count_valid_passwords(std::span<std::tuple<PasswordPolicy, std::string>> pw_and_policies) {
    int valid_passwords = 0;
    for (const auto& [pw_policy, pw] : pw_and_policies) {
        if (check_password(pw_policy, pw)) {
            valid_passwords++;
        }
    }
    return valid_passwords;
}

int main(int argc, char const *argv[])
{
    std::vector<std::tuple<PasswordPolicy, std::string>> file_entries = read_file("/home/drew/workspace/advent-of-code/day_2_input.txt");
    int valid_passwords = count_valid_passwords(file_entries);
    std::cout << "Valid passwords: " << valid_passwords;
    return 0;
}
