#ifndef QA_SOFT_FRAME_CONTOR_H
#define QA_SOFT_FRAME_CONTOR_H
#include <gr_top_block.h>
#include "soft_frame_contor.h"
#include <gr_vector_source_b.h>
#include <gr_vector_source_f.h>
#include <gr_vector_sink_b.h>
#include <gr_vector_sink_f.h>
#include <gr_message.h>

#define INPUT_LENGTH 20

class qa_soft_frame_contor;
typedef boost::shared_ptr<qa_soft_frame_contor> qa_soft_frame_contor_sptr;// define a sptr type

qa_soft_frame_contor_sptr make_qa_soft_frame_contor_sptr(); // declare a function;

class qa_soft_frame_contor: public gr_top_block {

private:
    qa_soft_frame_contor();  // private default constructor
    friend qa_soft_frame_contor_sptr make_qa_soft_frame_contor_sptr(); // friend function is able to access private constructor.
    gr_vector_source_b_sptr          d_vector_source_hard_input;
    gr_vector_source_f_sptr          d_vector_source_soft_input;
    soft_frame_contor_sptr           d_soft_frame_contor;
    gr_msg_queue_sptr                d_msg_queue;
    short_codec_sptr                 d_short_decoder;

    static const unsigned char input_hard[INPUT_LENGTH];
    static const float         input_soft[INPUT_LENGTH];
public:
    gr_vector_source_b_sptr get_vector_source_hard_input() { return d_vector_source_hard_input; }
    gr_vector_source_f_sptr get_vector_source_soft_input() { return d_vector_source_soft_input; }
};

#endif

