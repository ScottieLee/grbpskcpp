
#include "qa_correlate_sync_word.h"
#include <gr_io_signature.h>
#include <vector>

#define VERBOSE

#ifdef VERBOSE
#include <iostream>
#include <stdio.h>
#endif

qa_correlate_sync_word_sptr make_qa_correlate_sync_word_sptr()
{
    return gnuradio::get_initial_sptr(new qa_correlate_sync_word());
}

const unsigned char qa_correlate_sync_word::input_hard[INPUT_LENGTH]= { 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1};
const float         qa_correlate_sync_word::input_soft[INPUT_LENGTH]= {-1.01, 1.21, -1, 1, -0.9, -0.8, -0.7, -1, -1.01, -0.9, -0.8, -1.2, -1.3, 0.5, -1, -1, -1, 1.5, 1.3, -0.9};

static std::string bit_string = "0100000000";
qa_correlate_sync_word::qa_correlate_sync_word():gr_top_block("qa_correlate_sync_word"),qa_access_code(bit_string)
{

    std::vector<unsigned char> input_hard_vec (input_hard, input_hard + INPUT_LENGTH);
    std::vector<float> input_soft_vec (input_soft, input_soft + INPUT_LENGTH);
    d_vector_source_hard_input    = gr_make_vector_source_b(input_hard_vec, false, 1);
    d_vector_source_soft_input    = gr_make_vector_source_f(input_soft_vec, false, 1);
    d_correlate_sync_word         = make_correlate_sync_word(qa_access_code, 10, 0); // access code and threshold
    d_vector_sink_hard_and_flag   = gr_make_vector_sink_b();
    d_vector_sink_soft_output     = gr_make_vector_sink_f();
    this->connect (d_vector_source_hard_input, 0, d_correlate_sync_word, 0);
    this->connect (d_vector_source_soft_input, 0, d_correlate_sync_word, 1);
    this->connect (d_correlate_sync_word, 0, d_vector_sink_hard_and_flag, 0);
    this->connect (d_correlate_sync_word, 1, d_vector_sink_soft_output, 0);
}

int main()
{
    qa_correlate_sync_word_sptr testcase1 = make_qa_correlate_sync_word_sptr();
    testcase1 -> start();
    testcase1 -> wait();
    std::vector<unsigned char> correlate_hard_and_flag;
    std::vector<float> correlate_soft_output;
    correlate_hard_and_flag = testcase1-> get_vector_sink_hard_and_flag() -> data();
    correlate_soft_output   = testcase1-> get_vector_sink_soft_output() -> data();
    printf("\n\nHard Output is :\n");
    for (int v = 0; v < correlate_hard_and_flag.size(); v++){
        std::cout << static_cast<unsigned int>(correlate_hard_and_flag[v]) << "\t" << std::flush;
    }
    std::cout << std::endl;
    printf("\n\nSoft Output is :\n");
    for (int v = 0; v < correlate_soft_output.size(); v++){
        std::cout << (correlate_soft_output[v]) << "\t" << std::flush;
    }
    std::cout << std::endl;

    return 0;
}
