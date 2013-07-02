#include "rx_path_hierblock.h"
#include <gr_io_signature.h>

rx_path_hierblock_sptr make_rx_path_hierblock_sptr(demod_params*     demod_param,
                                                   demod_t          demode_type,
                                                   callbacktype1   rx_callback,
                                                   unsigned int    samples_per_symbol,
                                                   unsigned int    correlate_threshold,
                                                   const std::string& access_code,
                                                   short_codec_sptr phy_codec
                                                   )
{
    return gnuradio::get_initial_sptr(new rx_path_hierblock(demod_param,
                                                             demode_type,
                                                             rx_callback,
                                                             samples_per_symbol,
                                                             correlate_threshold,
                                                             access_code,
                                                             phy_codec
                                                             ));
}

rx_path_hierblock::rx_path_hierblock(      demod_params*    demod_param,
                                           demod_t         demod_type,
                                           callbacktype1   rx_callback,
                                           unsigned int    samples_per_symbol,
                                           unsigned int    correlate_threshold,
                                           const std::string& access_code,
                                           short_codec_sptr    phy_codec):
             gr_hier_block2("rx_path_hierblock",
                            gr_make_io_signature(1, 1, sizeof(gr_complex)),
                            gr_make_io_signature(0, 0, 0)
                           ),
             d_rx_callback(rx_callback),
             d_samples_per_symbol(samples_per_symbol),
             d_correlate_threshold(correlate_threshold),
             d_access_code(access_code),
             d_phy_codec_sptr(phy_codec)

{

    // Setup for channel filter
    d_channel_filter_taps = gr_firdes::low_pass(
                                              1.0, //gain
                                              d_samples_per_symbol, // sampling rate
                                              1.0, // midpoint of trans. band
                                              0.5, // width of trans. band
                                              gr_firdes::WIN_BLACKMAN // window
                                              );
    d_channel_filter = gr_make_fir_filter_ccf(1, // no decimation                //FIXME!!!
                                              d_channel_filter_taps
                                             );

    // Setup for demod block
    d_demod_block = create_demod_block(demod_type,demod_param);

    //Setup for correlator and frame sink
    d_rcvd_pktq = gr_make_msg_queue();
    std::bitset<ACCESS_CODE_LEN> access_code_bitset(d_access_code);
    d_correlator = make_correlate_sync_word(access_code_bitset, ACCESS_CODE_LEN, d_correlate_threshold); //threashold
    d_framer = make_soft_frame_contor(d_rcvd_pktq, d_phy_codec_sptr);

    // Connect'em up
    this->connect (this->basic_block(), 0, d_channel_filter, 0);
    this->connect (d_channel_filter, 0, d_demod_block, 0);
    this->connect (d_demod_block, 0, d_correlator, 0);
    this->connect (d_demod_block, 1, d_correlator, 1);
    this->connect (d_correlator, 0, d_framer, 0);
    this->connect (d_correlator, 1, d_framer, 1);

    d_queue_watcher = new queue_watcher_thread(d_rcvd_pktq, d_rx_callback);
}

