#ifndef TX_PATH_HIERBLOCK_H
#define TX_PATH_HIERBLOCK_H

#include <gr_message_source.h>
#include <gr_multiply_const_cc.h> // Baseband power control, useful for BER simulation
#include <gr_hier_block2.h> //This is a hierachical block

#include "mod_base.h"
#include "packet_utils.h"

class tx_path_hierblock;

typedef boost::shared_ptr< tx_path_hierblock > tx_path_hierblock_sptr; // define a sptr for handling


tx_path_hierblock_sptr make_tx_path_hierblock_sptr (mod_params*     mod_param, // Params of modulations
                                                    mod_t      mode_type,     // Enum Type of modulations
                                                    float    tx_amplitude,
                                                    int      msg_queue_limit);


class tx_path_hierblock: public gr_hier_block2 {
private:
    tx_path_hierblock (mod_params*     mod_param,
                       mod_t           mode_type,
                       float           tx_amplitude,
                       int             msg_queue_limit
                      );// Private Contor.
    friend tx_path_hierblock_sptr make_tx_path_hierblock_sptr(mod_params*     mod_param,
                                                              mod_t           mode_type,
                                                              float           tx_amplitude,
                                                              int             msg_queue_limit);// Private Contor. // Sptr handles construction and controlling of the block instance.

    // declaration of block sptrs:
    gr_message_source_sptr                       d_message_source;
    gr_basic_block_sptr                          d_mod_block; // Polynorphic sptr
    gr_multiply_const_cc_sptr                    d_power_control;



    short_codec_sptr d_short_encoder;
    // data members
    float d_tx_amplitude; // Max is 1.0

public:
    bool send_pkt (std::string mpdu, bool eof); // Given Mac Layer Data Unit, make it into Phy Layer Frame
    // accessors:
    void set_tx_amplitude (float ampl)
    {
        d_tx_amplitude = ampl;
        gr_complex ampl_complex(ampl, ampl);
        return d_power_control->set_k(ampl_complex);
    }
    float get_tx_amplitude()
    {
        return d_tx_amplitude;
    }

};

inline bool tx_path_hierblock::send_pkt(std::string mpdu, bool eof)
{
    gr_message_sptr msg;
    if (eof)
        msg = gr_make_message(1);
    else{
        std::string ppdu;
        packet_utils::mpdu2ppdu(mpdu, ppdu, d_short_encoder);
        msg = gr_make_message_from_string(ppdu);
//        std::cout << "PPDULEN = " << ppdu.size() << "Send need time " << ppdu.size()*8.0/500 << " ms" << std::endl;
    }
    d_message_source->msgq()->insert_tail(msg);
    return true;
}

#endif // TX_PATH_HIERBLOCK_H
