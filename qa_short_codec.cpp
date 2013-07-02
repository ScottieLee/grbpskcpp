#include "short_codec.h"
#include <vector>
#include <iostream>

int main(void)
{
    short_codec_sptr short_encoder = boost::make_shared<repetition_encoder>(5);
    std::vector<bool> input;
    input.push_back(1);
    input.push_back(0);
    input.push_back(1);
    input.push_back(1);
    short_encoder -> load_input(input);
    short_encoder -> process();
    std::vector<bool> output = short_encoder -> result();
    std::cout << "Encoding Result is " << std::endl;
    for (int v = 0; v < output.size(); v++) {
        std::cout << output[v] << "\t";
    }
    std::cout << std::endl;

    //Make some channel errors
    output[3] = !output[3];
    output[5] = !output[5];
    output[11] = !output[11];
    output[17] = !output[17];

    short_codec_sptr short_decoder = boost::make_shared<repetition_decoder>(5);
    short_decoder -> load_input(output);
    short_decoder -> process();
    output.clear();
    output = short_decoder -> result();
    std::cout << "Decoding Result is " << std::endl;
    for (int v = 0; v < output.size(); v++) {
        std::cout << output[v] << "\t";
    }
    return 0;
}
