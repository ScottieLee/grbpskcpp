#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

#include <string>
#include <algorithm>
#include <stdexcept>

std::string string_to_hex(const std::string& input);
std::string hex_to_string(const std::string& input);

void conv_uint_to_string(unsigned int integer, std::string& int_string);
void conv_string_to_uint(std::string& int_string, unsigned int &integer);


void packed_string_to_1_0_string(const std::string& input,std::string& output);
std::string string_1_0_to_packed_string(const std::string& input);
void string_1_0_to_packed_string(const std::string& input,std::string& output);

void conv_uchar_to_string(unsigned char byte, std::string& char_string);
std::string repeat( const std::string &word, int times );
#endif // STRING_FUNCTIONS_H
