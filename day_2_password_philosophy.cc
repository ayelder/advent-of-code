#include <iostream>
#include <fstream>
#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "absl/strings/str_split.h"

class PasswordPolicy
{
public:
    virtual bool check_password(std::string_view pw) const = 0;
    virtual ~PasswordPolicy(){};
};

class PasswordChecker
{
public:
    explicit PasswordChecker(std::unique_ptr<PasswordPolicy> policy) : policy_(std::move(policy)){};
    bool check_password(std::string_view pw) const
    {
        return policy_->check_password(pw);
    }

private:
    std::unique_ptr<PasswordPolicy> policy_;
};

// OldPasswordPolicy encapsulates the corporate password policy which specifies
// the min and max amount of a given letter.
class OldPasswordPolicy : public PasswordPolicy
{
public:
    bool check_password(std::string_view pw) const override;
    static std::unique_ptr<OldPasswordPolicy> create_policy(std::string_view pw_policy_string);

private:
    OldPasswordPolicy(int min, int max, char letter) : min_(min), max_(max), letter_(letter){};
    int min_;
    int max_;
    char letter_;
};

std::unique_ptr<OldPasswordPolicy> OldPasswordPolicy::create_policy(std::string_view pw_policy_string)
{
    // PasswordPolicy is of the format "N-M l" where N is min, M is max and l is the letter. AFAICT they are all
    // single characters but this implentation allows for the digits to at least be > 1 digit

    // First, split the min-max from the letter
    std::vector<std::string_view> first_split = absl::StrSplit(pw_policy_string, " ");
    // Second half of the split, should only have one character which is the letter that must be found
    char letter = first_split[1][0];
    // Second, split the min-max to get them separately
    std::vector<std::string> second_split = absl::StrSplit(first_split[0], "-");
    return std::unique_ptr<OldPasswordPolicy>(new OldPasswordPolicy(
        /*min=*/std::stoi(second_split[0]),
        /*max=*/std::stoi(second_split[1]),
        letter));
}

bool OldPasswordPolicy::check_password(std::string_view pw) const
{
    // std::cout << pw << std::endl;
    int letter_count = 0;
    for (char letter : pw)
    {
        if (letter == letter_)
        {
            letter_count++;
        }
    }
    return letter_count >= min_ && letter_count <= max_;
}

void remove_trailing_whitespace(std::string_view &s)
{
    // Find the last character that is not whitespace (this should always be found unless it is only whitespace)
    std::size_t trailing_whitespace_pos = s.find_last_not_of(" ");
    // The last position in a string would be size() - 1, meaning if there is no whitespace the position would
    // be size() - 1. Therefore, we should always add 1 to the position to determine how many characters to actually
    // remove.
    s.remove_suffix(trailing_whitespace_pos == std::string::npos ? 0 : s.size() - (trailing_whitespace_pos + 1));
}

void remove_leading_whitespace(std::string_view &s)
{
    std::size_t leading_whitespace_pos = s.find_first_not_of(" ");
    s.remove_prefix(leading_whitespace_pos == std::string::npos ? 0 : leading_whitespace_pos);
}

template <typename PasswordPolicyFactoryFunction>
std::vector<std::tuple<PasswordChecker, std::string>> read_file(std::string_view filename, PasswordPolicyFactoryFunction create_policy_fn)
{
    std::vector<std::tuple<PasswordChecker, std::string>> entries;
    std::ifstream input_file_stream;
    std::cout << "Opening " << filename << std::endl;
    input_file_stream.open(std::string(filename), std::ios_base::in);
    std::string current_line;
    while (getline(input_file_stream, current_line))
    {
        std::vector<std::string_view> policy_and_pw = absl::StrSplit(current_line, ":");
        std::string_view pw = policy_and_pw[1];
        remove_leading_whitespace(pw);
        remove_trailing_whitespace(pw);
        PasswordChecker checker(std::unique_ptr<PasswordPolicy>(create_policy_fn(policy_and_pw[0]).release()));
        entries.push_back(std::make_tuple(std::move(checker), std::string(pw)));
    }
    std::cout << "Read " << entries.size() << " entries" << std::endl;
    return entries;
}

int count_valid_passwords(std::span<std::tuple<PasswordChecker, std::string>> pw_and_policies)
{
    int valid_passwords = 0;
    for (const auto &[pw_checker, pw] : pw_and_policies)
    {
        if (pw_checker.check_password(pw))
        {
            valid_passwords++;
        }
    }
    return valid_passwords;
}

// NewPasswordPolicy encapsulates the corporate password policy which specifies
// the first and second position where a given letter must be in ONLY one of those positions.
class NewPasswordPolicy : public PasswordPolicy
{
public:
    bool check_password(std::string_view pw) const override;
    static std::unique_ptr<NewPasswordPolicy> create_policy(std::string_view pw_policy_string);

private:
    NewPasswordPolicy(int first_pos, int second_pos, char letter) : first_pos_(first_pos), second_pos_(second_pos), letter_(letter){};
    int first_pos_;
    int second_pos_;
    char letter_;
};

std::unique_ptr<NewPasswordPolicy> NewPasswordPolicy::create_policy(std::string_view pw_policy_string)
{
    // PasswordPolicy is of the format "N-M l" where N is min, M is max and l is the letter. AFAICT they are all
    // single characters but this implentation allows for the digits to at least be > 1 digit

    // First, split the min-max from the letter
    std::vector<std::string_view> first_split = absl::StrSplit(pw_policy_string, " ");
    // Second half of the split, should only have one character which is the letter that must be found
    char letter = first_split[1][0];
    // Second, split the min-max to get them separately
    std::vector<std::string> second_split = absl::StrSplit(first_split[0], "-");
    return std::unique_ptr<NewPasswordPolicy>(new NewPasswordPolicy(
        /*first_pos=*/std::stoi(second_split[0]) - 1,
        /*second_pos=*/std::stoi(second_split[1]) - 1,
        letter));
}

bool NewPasswordPolicy::check_password(std::string_view pw) const
{
    // Use XOR so that it returns true IFF one of the positions have the specified letter
    return (first_pos_ < pw.size() && pw[first_pos_] == letter_) ^ (second_pos_ < pw.size() && pw[second_pos_] == letter_);
}

int main(int argc, char const *argv[])
{
    std::string filepath("/home/drew/workspace/advent-of-code/day_2_input.txt");
    { // PART ONE
        std::vector<std::tuple<PasswordChecker, std::string>> file_entries = read_file(filepath, &OldPasswordPolicy::create_policy);
        int valid_passwords = count_valid_passwords(file_entries);
        std::cout << "Valid passwords (old): " << valid_passwords << std::endl;
    }
    { // PART TWO
        std::vector<std::tuple<PasswordChecker, std::string>> file_entries = read_file(filepath, &NewPasswordPolicy::create_policy);
        int valid_passwords = count_valid_passwords(file_entries);
        std::cout << "Valid passwords (old): " << valid_passwords << std::endl;
    }
    return 0;
}
