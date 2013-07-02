#ifndef TOP_BLOCK_H
#define TOP_BLOCK_H

#include <gr_top_block.h>
#include <gr_vector_source_b.h>
#include <gr_vector_sink_b.h>
#include <usrp_source_c.h>
#include <usrp_sink_c.h>

#include <gr_throttle.h>

#include <boost/thread.hpp>
#include <vector>
#include <string>

void callback(bool ok, std::string mpdu, timeval lt_timeval);

class top_block;
typedef boost::shared_ptr<top_block> top_block_sptr;// define a sptr type

top_block_sptr make_top_block_sptr(); // declare a function;

//-------------------------------------------------------------------------------------------
static usrp_subdev_spec                  static_spec(0,0);  //FIXME change sideA(0,0) and sideB(1,0)
//-------------------------------------------------------------------------------------------

class top_block: public gr_top_block {

private:
    top_block();  // private default constructor
    friend top_block_sptr make_top_block_sptr();

    // constparameters
    static const unsigned int                SAMPLES_PER_SYMBOL;
    static const unsigned int                DAC_RATE;
    static const unsigned int                BITRATE;
    static const double                      TX_FREQ;
    static const float                       TX_AMPL;
    static const unsigned int                BITS_PER_SYMBOL;

    static const unsigned int                ADC_RATE;
    static const double                      RX_FREQ;
    static const float                       RX_GAIN;
    static const float                       MAX_AMPLITUDE;

    // blocks for transmit path
    usrp_sink_c_sptr                         d_usrp_sink_sptr;
    db_base_sptr                             d_sub_device_tx;
    usrp_subdev_spec                         d_spec_tx;
    gr_message_source_sptr                   d_message_source;
    gr_packed_to_unpacked_bb_sptr            d_packed_to_unpacked;
    gr_chunks_to_symbols_bc_sptr             d_chunks_to_symbols;
    std::vector<float>                       d_rrc_tx_taps;
    gr_interp_fir_filter_ccf_sptr            d_rrc_tx;
    gr_multiply_const_cc_sptr                d_power_control;
    // blocks for receive path
    usrp_source_c_sptr                       d_usrp_source_sptr;
    db_base_sptr                             d_sub_device_rx;
    usrp_subdev_spec                         d_spec_rx;
    std::vector<float>                       d_channel_filter_taps;
    gr_fir_filter_ccf_sptr                   d_channel_filter;
    gr_agc2_cc_sptr                          d_agc;
    std::vector<float>                       d_rrc_rx_taps;
    gr_interp_fir_filter_ccf_sptr            d_rrc_rx;
    gr_mpsk_receiver_cc_sptr                 d_mpsk_receiver;
    soft_slicer_sptr                         d_soft_slicer;
    gr_msg_queue_sptr                        d_rcvd_pktq;
    correlate_sync_word_sptr                 d_correlator;
    soft_frame_contor_sptr                   d_framer;
    queue_watcher_thread*                    d_queue_watcher;

    gr_throttle_sptr                         d_throttle; // For simulation
public:
    bool send_pkt(std::string mpdu, bool eof);
};


#endif // TOP_BLOCK_H
