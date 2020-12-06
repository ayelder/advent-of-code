// binary space partitioning to seat people.
//  A seat might be specified like FBFBBFFRLR, where F means "front", B means "back",
//  L means "left", and R means "right".

// The first 7 characters will either be F or B; these specify exactly one of the 128
// rows on the plane (numbered 0 through 127). Each letter tells you which half of a
// region the given seat is in. Start with the whole list of rows; the first letter
// indicates whether the seat is in the front (0 through 63) or the back (64 through 127).
// The next letter indicates which half of that region the seat is in, and so on until
//  you're left with exactly one row.

#include <fstream>
#include <iostream>
#include <math.h>
#include <string_view>
#include <vector>

const int kNumRows = 128;
const int kRowPosChars = (int) log2(kNumRows);
const int kNumCols = 8;
const int kColPosChars = (int) log2(kNumCols);

class SeatPosition
{

public:
    SeatPosition(int row, int col) : row_(row), col_(col), seat_id_((row * 8) + col) {}
    int seat_id() const { return seat_id_; }

private:
    const int row_;
    const int col_;
    const int seat_id_;
};

int calc_pos(std::string_view pos_string, int num_pos)
{
    std::cout << pos_string << " ^ " << num_pos << std::endl;
    int min_pos = 0;
    int max_pos = max_pos - 1;
    int partition_size = num_pos;

    for (char pos_char : pos_string)
    {
        partition_size /= 2;
        if (pos_char == 'F' || pos_char == 'L')
        {
            // Lower half
            max_pos -= partition_size;
        }
        else
        { // pos_char == 'B' || pos_char == 'R'
            // Upper half
            min_pos += partition_size;
        }
    }
    // min_row and max_row should now be equal, can return either
    return min_pos;
}

std::vector<SeatPosition> read_file(std::string_view filename)
{
    std::vector<SeatPosition> file_entries;
    std::ifstream input_file_stream;
    std::cout << "Opening " << filename << std::endl;
    input_file_stream.open(std::string(filename), std::ios_base::in);
    std::string current_line;
    while (getline(input_file_stream, current_line))
    {
        std::string_view current_line_view = std::string_view(current_line);
        int row_pos = calc_pos(current_line_view.substr(0, kRowPosChars), kNumRows);
        int col_pos = calc_pos(current_line_view.substr(kRowPosChars, kColPosChars), kNumCols);
        std::cout << current_line_view << " => (" << row_pos << ", " << col_pos << ")" << std::endl;
        file_entries.emplace_back(row_pos, col_pos);
    }
    std::cout << "Read " << file_entries.size() << " entries" << std::endl;
    return file_entries;
}

int main(int argc, char const *argv[])
{
    std::vector<SeatPosition> pos_vec = read_file("/home/drew/workspace/advent-of-code/day_5_input.txt");
    int max_seat_id = 0;
    for (const SeatPosition& seat_pos : pos_vec ) {
        max_seat_id = std::max(max_seat_id, seat_pos.seat_id());
    }
    std::cout << "Max seat ID: " << max_seat_id;
    return 0;
}
