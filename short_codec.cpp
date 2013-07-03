#include "short_codec.h"
#include <iostream>

short_codec::short_codec():d_input_buf(),d_output_buf()
{
}

short_encoder::short_encoder():short_codec()
{
}

short_decoder::short_decoder():short_codec()
{
}

repetition_encoder::repetition_encoder(int repetition_factor): short_encoder(), d_repetition_factor(repetition_factor)
{
    if (!(d_repetition_factor % 2)) {
        std::cerr << "Repetiton_encoder:: Repetition factor should be odd." << std::endl;
        exit(1);
    }
}

bool repetition_encoder::process()
{
    std::vector<bool>  input_buf  = get_encode_buf();
    std::vector<bool>& output_buf = get_outbuf();
    output_buf.clear();
    for (int v = 0; v < d_repetition_factor; v++) {
        for ( int w = 0; w < input_buf.size(); w++) {
            output_buf.push_back(input_buf[w]);
        }
    }
    return true;
}
repetition_decoder::repetition_decoder(int repetition_factor): short_decoder(), d_repetition_factor(repetition_factor)
{
    if (!(d_repetition_factor % 2)) {
        std::cerr << "Repetiton_encoder:: Repetition factor should be odd." << std::endl;
        exit(1);
    }
}

bool repetition_decoder::process()
{
    std::vector<bool>  input_buf  = get_decode_buf();
    std::vector<bool>& output_buf = get_outbuf();
    output_buf.clear();
    size_t out_size = input_buf.size()/d_repetition_factor;
    std::vector<float> tmp(out_size, 0);
    for (int v = 0; v < d_repetition_factor; v++) {
        for (int w = 0; w < out_size; w++)
        {
            
            tmp[w] += input_buf[w + out_size*v];
        }
    }
    for (int v = 0; v < out_size; v++){
        tmp[v] = tmp[v] / d_repetition_factor;
        output_buf.push_back(tmp[v] < 0.5? 0 : 1);
    }
    return true;
}
