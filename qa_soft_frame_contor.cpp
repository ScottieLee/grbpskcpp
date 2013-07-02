#include "qa_soft_frame_contor.h"
#include <gr_io_signature.h>
#include <vector>
#include "short_codec.h"

#define VERBOSE

#ifdef VERBOSE
#include <iostream>
#include <stdio.h>
#endif

qa_soft_frame_contor_sptr make_qa_soft_frame_contor_sptr()
{
    return gnuradio::get_initial_sptr(new qa_soft_frame_contor());
}

const unsigned char qa_soft_frame_contor::input_hard[INPUT_LENGTH]= { 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1};
const float         qa_soft_frame_contor::input_soft[INPUT_LENGTH]= {-1.01, 1.21, -1, 1, -0.9, -0.8, -0.7, -1, -1.01, -0.9, -0.8, -1.2, -1.3, 0.5, -1, -1, -1, 1.5, 1.3, -0.9};

qa_soft_frame_contor::qa_soft_frame_contor():gr_top_block("qa_soft_frame_contor")
{
    short_codec_sptr short_encoder = boost::make_shared<repetition_encoder>(15);

    std::vector<bool> input;
    input.push_back(0);
    input.push_back(0);
    input.push_back(0);
    input.push_back(0);
    input.push_back(0);
    input.push_back(0);
    input.push_back(0);
    input.push_back(1);
    short_encoder -> load_input(input);
    short_encoder -> process();
    std::vector<bool> header = short_encoder -> result();
    std::cout << "Header Encoding Result is " << std::endl;
    for (int v = 0; v < header.size(); v++) {
        std::cout << header[v] << "\t";
    }
    std::cout << std::endl;


    d_short_decoder = boost::make_shared<repetition_decoder>(15); // 8bit len repeat 15 times

    std::vector<unsigned char> input_hard_vec (input_hard, input_hard + INPUT_LENGTH);
    std::vector<float> input_soft_vec (input_soft, input_soft + INPUT_LENGTH);

    input_hard_vec.insert(input_hard_vec.begin(), header.begin(), header.end());   // slow
    

    for (int v = 0; v < header.size(); v++) {
        input_soft_vec.insert(input_soft_vec.begin(), -25.6);
    }

    input_hard_vec.insert(input_hard_vec.begin(), input_hard_vec.begin(), input_hard_vec.end());
    input_soft_vec.insert(input_soft_vec.begin(), input_soft_vec.begin(), input_soft_vec.end());

    input_hard_vec.insert(input_hard_vec.begin(), 1);
    input_hard_vec.insert(input_hard_vec.begin(), 0);
    input_hard_vec[2] = 2;
    input_hard_vec.insert(input_hard_vec.begin()+142, 0);
    input_hard_vec[143] = 2;
    input_soft_vec.insert(input_soft_vec.begin(), 0.99);
    input_soft_vec.insert(input_soft_vec.begin(), 0.99);
    input_soft_vec.insert(input_soft_vec.begin()+142, 0.99);

    std::cout << "The input hard bits are :\n";
    for (int v = 0; v < input_hard_vec.size(); v++) {
        std::cout << (int)input_hard_vec[v] << "\t";
    }
    std::cout << std::endl;

    std::cout << "The input soft bits are :\n";
    for (int v = 0; v < input_soft_vec.size(); v++) {
        std::cout << input_soft_vec[v] << "\t";
    }
    std::cout << std::endl;

    d_vector_source_hard_input    = gr_make_vector_source_b(input_hard_vec, false, 1);
    d_vector_source_soft_input    = gr_make_vector_source_f(input_soft_vec, false, 1);
    d_msg_queue                   = gr_make_msg_queue();
    d_soft_frame_contor           = make_soft_frame_contor(d_msg_queue, d_short_decoder);
    this->connect (d_vector_source_hard_input, 0, d_soft_frame_contor, 0);
    this->connect (d_vector_source_soft_input, 0, d_soft_frame_contor, 1);
}

int main()
{

    qa_soft_frame_contor_sptr testcase1 = make_qa_soft_frame_contor_sptr();
    testcase1 -> run();

    return 0;
}
