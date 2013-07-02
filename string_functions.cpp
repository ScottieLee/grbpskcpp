#include "string_functions.h"
#include <string.h>
#include <iostream>
#include <cassert> //For assert

//convert string to hex string
std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}



std::string hex_to_string(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    if (len & 1) throw std::invalid_argument("odd length");

    std::string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2)
    {
        char a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        if (*p != a) throw std::invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        if (*q != b) throw std::invalid_argument("not a hex digit");

        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
}

void conv_uint_to_string(unsigned int integer, std::string& int_string)
{
    char p1[sizeof(unsigned int)];
    char* p2 = (char*) &integer;
    for(unsigned int v = 0; v < sizeof(unsigned int); v++){
        *(p1 + v) = *(p2 + sizeof(unsigned int) -1 -v);
    }
    int_string = std::string((char*)p1, sizeof(unsigned int));
}

void conv_string_to_uint(std::string& int_string,unsigned int &integer)
{
    char p1[sizeof(unsigned int)];
    char* p2 = (char*) int_string.data();
    for(int v = 0; v < sizeof(int); v++){
        *(p1 + v) = *(p2 + sizeof(unsigned int) -1 -v);
    }
    char* p3 = (char*) &integer;
    memcpy(p3, p1, sizeof(unsigned int));
}

void conv_uchar_to_string(unsigned char byte, std::string& char_string)
{
    std::string tmp(1, byte);
    char_string = tmp;
}

void packed_string_to_string_1_0(const std::string& input, std::string& output)
{
    unsigned char* input_array = (unsigned char*)input.data();
    unsigned char * array_1_0 = new unsigned char(8*input.length());
    for(int v = 0; v < input.length(); v++){
        *(array_1_0 + v*8 + 0) = (input_array[v]>>7) & 0x01;
        *(array_1_0 + v*8 + 1) = (input_array[v]>>6) & 0x01;
        *(array_1_0 + v*8 + 2) = (input_array[v]>>5) & 0x01;
        *(array_1_0 + v*8 + 3) = (input_array[v]>>4) & 0x01;
        *(array_1_0 + v*8 + 4) = (input_array[v]>>3) & 0x01;
        *(array_1_0 + v*8 + 5) = (input_array[v]>>2) & 0x01;
        *(array_1_0 + v*8 + 6) = (input_array[v]>>1) & 0x01;
        *(array_1_0 + v*8 + 7) = input_array[v] & 0x01;
    }
    for(int v = 0; v < 8*input.length(); v++)
    {
        output.push_back('0'+ *(array_1_0 + v));
    }
    delete[] array_1_0;

}


std::string string_1_0_to_packed_string(const std::string& input)
{
    char* input_array = (char*)input.data();
    assert(input.size()%8 == 0);
    char* output_char = new char[input.size()/8];
    for (int v = 0; v < input.size()/8; v++){
        output_char[v] = 0x00;
        output_char[v] |= (input_array[8*v] - '0') << 7;
        output_char[v] |= (input_array[8*v+1] - '0') << 6;
        output_char[v] |= (input_array[8*v+2] - '0') << 5;
        output_char[v] |= (input_array[8*v+3] - '0') << 4;
        output_char[v] |= (input_array[8*v+4] - '0') << 3;
        output_char[v] |= (input_array[8*v+5] - '0') << 2;
        output_char[v] |= (input_array[8*v+6] - '0') << 1;
        output_char[v] |= (input_array[8*v+7] - '0') << 0;
    }
    std::string output(output_char, sizeof(char)* input.size()/8);
    return output;
}


void string_1_0_to_packed_string(const std::string& input,std::string& output)
{
    char* input_array = (char*)input.data();
    assert(input.size()%8 == 0);
    char* output_char = new char[input.size()/8];
    for (int v = 0; v < input.size()/8; v++){
        output_char[v] = 0x00;
        output_char[v] |= (input_array[8*v] - '0') << 7;
        output_char[v] |= (input_array[8*v+1] - '0') << 6;
        output_char[v] |= (input_array[8*v+2] - '0') << 5;
        output_char[v] |= (input_array[8*v+3] - '0') << 4;
        output_char[v] |= (input_array[8*v+4] - '0') << 3;
        output_char[v] |= (input_array[8*v+5] - '0') << 2;
        output_char[v] |= (input_array[8*v+6] - '0') << 1;
        output_char[v] |= (input_array[8*v+7] - '0') << 0;
    }
    output = std::string(output_char, sizeof(char)* input.size()/8);
}





std::string repeat( const std::string &word, int times ) {
   std::string result ;
   result.reserve(times*word.length()); // avoid repeated reallocation
   for ( int a = 0 ; a < times ; a++ )
      result += word ;
   return result ;
}
