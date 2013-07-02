#include "mod_base.h"

gr_basic_block_sptr create_mod_block(MOD_TYPE mod_enum, mod_params* mod_p)

{
    switch (mod_enum){
        //[Add Modulation into the cases!]
        case BPSK_MOD:
            return make_bpsk_mod_sptr(mod_p);
            break;
    }
}

gr_basic_block_sptr create_demod_block(DEMOD_TYPE demod_enum, demod_params* demod_p)
{
    switch (demod_enum){
        //[Add Modulation into the cases!]
        case BPSK_DEMOD:
            return make_bpsk_demod_sptr(demod_p);
            break;
    }
}
