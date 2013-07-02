#include "tx_path_hierblock.h"
#include <gr_io_signature.h>

tx_path_hierblock_sptr make_tx_path_hierblock_sptr(mod_params*     mod_param,
                                                   mod_t           mode_type,
                                                   float           tx_amplitude,
                                                   int             msg_queue_limit)
{
    return gnuradio::get_initial_sptr(new tx_path_hierblock( mod_param,
                                                             mode_type,
                                                             tx_amplitude,
                                                             msg_queue_limit =4
                                                             ));
}


tx_path_hierblock::tx_path_hierblock(     mod_params*     mod_param,
                                          mod_t         mod_type,
                                          float    tx_amplitude,
                                          int      msg_queue_limit):
             gr_hier_block2("tx_path_hierblock",
                            gr_make_io_signature(0, 0, 0),
                            gr_make_io_signature(1, 1, sizeof(gr_complex))
                           ),
             d_tx_amplitude(tx_amplitude)
{

    //setup for gr_message_source
    d_message_source = gr_make_message_source(sizeof(char), msg_queue_limit);

    //setup for mod
    d_mod_block = create_mod_block(mod_type, mod_param); // Factory function creating modulation block

    //setup for power control
    d_power_control = gr_make_multiply_const_cc(1);
    set_tx_amplitude(d_tx_amplitude);



    d_short_encoder = boost::make_shared<repetition_encoder>(5);

    //Connect them up
    this -> connect (d_message_source, 0, d_mod_block, 0);
    this -> connect (d_mod_block, 0, d_power_control, 0);
    this -> connect (d_power_control, 0, this->basic_block(), 0);
}

