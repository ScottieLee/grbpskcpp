#ifndef RX_PATH_HIERBLOCK_H
#define RX_PATH_HIERBLOCK_H

#include <gr_fir_filter_ccf.h>    // For channel filter
#include "mod_base.h"
#include "correlate_sync_word.h"
#include "soft_frame_contor.h"
#include <gr_message.h>
#include "queue_watcher_thread.h" // For watcher thread pulling out from msg queue
#include <gr_hier_block2.h> //This is a hierachical block
#include <gr_firdes.h>

class rx_path_hierblock;
typedef boost::shared_ptr<rx_path_hierblock> rx_path_hierblock_sptr; // define a sptr for handling

rx_path_hierblock_sptr make_rx_path_hierblock_sptr (demod_params*   demod_param,
                                                    demod_t         demode_type,
                                                    callbacktype1   rx_callback,
                                                    unsigned int    samples_per_symbol,
                                                    unsigned int    correlate_threshold,
                                                    const std::string& access_code,
                                                    short_codec_sptr phy_codec
                                                    );// FIXME Add default parameters for access_code!

class rx_path_hierblock: public gr_hier_block2 {
private:
    rx_path_hierblock (demod_params*     mod_param,
                       demod_t          mode_type,
                       callbacktype1   rx_callback,
                       unsigned int    samples_per_symbol,
                       unsigned int    correlate_threshold,
                       const std::string& access_code,
                       short_codec_sptr phy_codec
                       ); //Private Contor.

    friend rx_path_hierblock_sptr make_rx_path_hierblock_sptr(demod_params*     demod_param,
                                                              demod_t          demode_type,
                                                              callbacktype1   rx_callback,
                                                              unsigned int    samples_per_symbol,
                                                              unsigned int    correlate_threshold,
                                                              const std::string& access_code,
                                                              short_codec_sptr phy_codec
                                                              );
                                                              // Sptr handles construction and controlling of the block instance.

    // declaration of block sptrs:
    gr_basic_block_sptr                      d_demod_block;
    std::vector<float>                       d_channel_filter_taps;
    gr_fir_filter_ccf_sptr                   d_channel_filter;
    gr_msg_queue_sptr                        d_rcvd_pktq;
    correlate_sync_word_sptr                 d_correlator;
    soft_frame_contor_sptr                   d_framer;
    queue_watcher_thread*                    d_queue_watcher;

    // data members
    callbacktype1    d_rx_callback;
    unsigned int     d_samples_per_symbol;
    unsigned int     d_correlate_threshold;
    std::string      d_access_code;   // 1_0_string!
    short_codec_sptr d_phy_codec_sptr;
public:
    // accessors:


};

#endif // RX_PATH_HIERBLOCK_H
