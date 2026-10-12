#include <iostream>   // Console output: cout and cerr.
#include <fstream>    // File input and output streams.
#include <string>     // Strings, getline, find, substr, stoi, and stof.
#include <vector>     // A growing collection of Pixel objects.
#include "pixel.h"    // Defines the Pixel structure.

// Function declarations allow main to call functions defined below it.
void average_colors(std::vector<Pixel> &pixel_list);
void flip_vertically(std::vector<Pixel> &pixel_list);

int main(int argc, char *argv[])
{
    // argc counts the program name and arguments: expect one filename.
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " pixels.dat\n";
        return 1;
    }

    // argv[1] is the input filename supplied on the command line.
    std::string filename = argv[1];
    std::ifstream input; // Stream used to read the pixel data.
    input.open(filename, std::ios::in); // Open the file in reading mode.
    if (!input) // Stop if the input file could not be opened.
    {
        std::cerr << "Cannot open " << filename << '\n';
        return 1;
    }

    std::vector<Pixel> pixel_list; // Stores every pixel read from the file.
    std::string line; // Holds one complete input line.
    int line_number = 0; // Identifies the line if parsing fails.

    // getline reads one line at a time. Test the read itself so that
    // the last line is not processed twice, as can happen with !eof().
    while (std::getline(input, line))
    {
        ++line_number; // Count the line just read.
        std::string values[5]; // Fields in input order: x, y, r, g, b.
        // size_t is the unsigned integer type used for string/vector indices.
        std::size_t start = 0; // String index where the next field begins.

        // find returns the next comma's index. substr takes a start
        // index and a length, extracting the text before that comma.
        // Five fields have four separating commas.
        for (int i = 0; i < 4; ++i)
        {
            std::size_t comma = line.find(',', start);
            if (comma == std::string::npos) // npos means no comma was found.
            {
                std::cerr << "Missing comma on line " << line_number << '\n';
                return 1;
            }
            values[i] = line.substr(start, comma - start); // Exclude the comma.
            start = comma + 1; // Begin the next search after this comma.
        }
        // The blue value is the remaining text after the last comma.
        values[4] = line.substr(start);

        Pixel pixel; // Groups one pixel's position and color values.
        // stoi converts text to an integer; stof converts to float.
        // The supplied input file is expected to contain valid numbers.
        pixel.x = std::stoi(values[0]); // Horizontal coordinate.
        pixel.y = std::stoi(values[1]); // Vertical coordinate.
        pixel.r = std::stof(values[2]); // Red intensity.
        pixel.g = std::stof(values[3]); // Green intensity.
        pixel.b = std::stof(values[4]); // Blue intensity.
        // Check coordinates before using them as a vector index.
        if (pixel.x < 0 || pixel.x >= 512 || pixel.y < 0 || pixel.y >= 256)
        {
            std::cerr << "Coordinate out of range on line " << line_number << '\n';
            return 1;
        }
        pixel_list.push_back(pixel); // Append the completed pixel to the vector.
    }
    if (input.bad()) // Detect a serious read error rather than normal EOF.
    {
        std::cerr << "Error while reading input.\n";
        return 1;
    }
    input.close(); // Finished reading the input file.

    // The assignment requires the complete 512-by-256 image.
    if (pixel_list.size() != 512 * 256)
    {
        std::cerr << "Expected 131072 pixels; read " << pixel_list.size() << '\n';
        return 1;
    }

    std::cout << "Pixels read: " << pixel_list.size() << '\n';
    average_colors(pixel_list); // Print a separate average for each color.
    flip_vertically(pixel_list); // Flip positions and restore coordinate order.

    std::ofstream output; // Stream used to write the flipped pixels.
    // Create flipped.dat, replacing its contents if it already exists.
    output.open("flipped.dat", std::ios::out);
    if (!output)
    {
        std::cerr << "Cannot open flipped.dat for writing.\n";
        return 1;
    }

    // precision is a stream member function; it needs no extra header.
    // Nine significant digits preserve typical 32-bit float values.
    output.precision(9);
    // Use an index to visit each pixel in the vector.
    for (std::size_t i = 0; i < pixel_list.size(); ++i)
    {
        Pixel pixel = pixel_list[i]; // Read the pixel at index i.
        // Match the input format: x,y,r,g,b followed by a newline.
        output << pixel.x << ',' << pixel.y << ',' << pixel.r << ','
               << pixel.g << ',' << pixel.b << '\n';
    }
    output.close(); // Flush buffered data and close the file.
    if (!output)
    {
        std::cerr << "Error writing flipped.dat.\n";
        return 1;
    }
    std::cout << "Saved flipped.dat\n";
    return 0; // Indicate successful completion.
}

// Sum each color separately and divide by the number of pixels.
// Passing by reference avoids copying the entire vector.
void average_colors(std::vector<Pixel> &pixel_list)
{
    if (pixel_list.empty()) // Avoid computing averages with zero pixels.
        return;

    // double reduces rounding error while adding many float values.
    double total_r = 0.0, total_g = 0.0, total_b = 0.0;
    // Use an index to visit each pixel in the vector.
    for (std::size_t i = 0; i < pixel_list.size(); ++i)
    {
        Pixel pixel = pixel_list[i]; // Read the pixel at index i.
        total_r += pixel.r; // Add this pixel's red value to the red total.
        total_g += pixel.g; // Add its green value to the green total.
        total_b += pixel.b; // Add its blue value to the blue total.
    }
    // size() is the number of pixels actually read.
    std::cout << "Average red: " << total_r / pixel_list.size() << '\n';
    std::cout << "Average green: " << total_g / pixel_list.size() << '\n';
    std::cout << "Average blue: " << total_b / pixel_list.size() << '\n';
}

// The assignment file contains one pixel for each of the 512 * 256 positions.
void flip_vertically(std::vector<Pixel> &pixel_list)
{
    // Create space for the complete flipped image.
    std::vector<Pixel> flipped(pixel_list.size());

    for (std::size_t i = 0; i < pixel_list.size(); ++i)
    {
        Pixel pixel = pixel_list[i]; // Copy the position and all three colors.
        pixel.y = 255 - pixel.y; // Flip vertically: 0 <-> 255, 1 <-> 254.

        // x and RGB stay unchanged. Place the whole pixel in output order.
        // Indices 0..255 hold x=0, y=0..255; indices 256..511 hold x=1.
        int position = pixel.x * 256 + pixel.y;
        flipped[position] = pixel;
    }

    // Replace the original vector with the flipped, ordered pixels.
    pixel_list = flipped;
}
