
#ifndef QA_CORRELATE_SYNC_WORD_H
#define QA_CORRELATE_SYNC_WORD_H
#include <gr_top_block.h>
#include "correlate_sync_word.h"
#include <gr_vector_source_b.h>
#include <gr_vector_source_f.h>
#include <gr_vector_sink_b.h>
#include <gr_vector_sink_f.h>
#define INPUT_LENGTH 20

class qa_correlate_sync_word;
typedef boost::shared_ptr<qa_correlate_sync_word> qa_correlate_sync_word_sptr;// define a sptr type

qa_correlate_sync_word_sptr make_qa_correlate_sync_word_sptr(); // declare a function;

class qa_correlate_sync_word: public gr_top_block {

private:
    qa_correlate_sync_word();  // private default constructor
    friend qa_correlate_sync_word_sptr  make_qa_correlate_sync_word_sptr(); // friend function is able to access private constructor.
    gr_vector_source_b_sptr          d_vector_source_hard_input;
    gr_vector_source_f_sptr          d_vector_source_soft_input;
    gr_vector_sink_b_sptr            d_vector_sink_hard_and_flag;
    gr_vector_sink_f_sptr            d_vector_sink_soft_output;
    correlate_sync_word_sptr         d_correlate_sync_word;
    static const unsigned char       input_hard[INPUT_LENGTH];
    static const float               input_soft[INPUT_LENGTH];
    std::bitset<ACCESS_CODE_LEN> qa_access_code;
public:
    gr_vector_source_b_sptr get_vector_source_hard_input() { return d_vector_source_hard_input; }
    gr_vector_source_f_sptr get_vector_source_soft_input() { return d_vector_source_soft_input; }
    gr_vector_sink_b_sptr   get_vector_sink_hard_and_flag(){ return d_vector_sink_hard_and_flag; }
    gr_vector_sink_f_sptr   get_vector_sink_soft_output()  { return d_vector_sink_soft_output; }

};
#endif

