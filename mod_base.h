#ifndef MOD_BASE_H
#define MOD_BASE_H

#include <list>
#include <boost/any.hpp>
#include <gr_hier_block2.h> //This is a hierachical block

//[Add moduation block headers here]
#include "bpsk_hierblock.h"

//[Define moduation block ptr types here]
typedef boost::shared_ptr<bpsk_mod> bpsk_mod_sptr; // define a sptr for handling
typedef boost::shared_ptr<bpsk_demod> bpsk_demod_sptr; // define a sptr for handling

//[Add modulation into enums]
enum MOD_TYPE{
    BPSK_MOD = 0
};
typedef enum MOD_TYPE mod_t;

enum DEMOD_TYPE {
    BPSK_DEMOD = 0
};
typedef enum DEMOD_TYPE demod_t;





// Base class of mod params
class mod_params{
private:
};

// Base class of demod params
class demod_params{
private:
};



gr_basic_block_sptr create_mod_block(MOD_TYPE mod_enum, mod_params* mod_p);

gr_basic_block_sptr create_demod_block(DEMOD_TYPE demod_enum, demod_params* demod_p);



#endif // MOD_BASE_H
