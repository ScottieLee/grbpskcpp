#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "soft_slicer.h"
#include <gr_io_signature.h>
#include <stdexcept>
#include <cstdio>

soft_slicer_sptr
make_soft_slicer (SLICE_MODE mode)
{
  return soft_slicer_sptr (new soft_slicer (mode));
}

soft_slicer::soft_slicer (SLICE_MODE mode)
    : gr_sync_block ("soft_slicer",
                     gr_make_io_signature (1, 1, sizeof (gr_complex)),
                     gr_make_io_signature2 (2, 2, sizeof (unsigned char),
                                                  sizeof (float)))
                    ,d_mode(mode)
{
}

inline int
soft_slicer::neg_slice(float x)
{
    return x < 0 ? 1 : 0;
}

inline int
soft_slicer::pos_slice(float x)
{
    return x < 0 ? 0 : 1;
}

int
soft_slicer::work (int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  unsigned char *out_hard = (unsigned char *) output_items[0];
  float *out_soft = (float *)         output_items[1];


  for (int i = 0; i < noutput_items; i++){
      if (d_mode == POS_MAPPING)
        out_hard[i] = pos_slice(in[i].real());
    else
        out_hard[i] = neg_slice(in[i].real());
    out_soft[i] = in[i].real();
    //fprintf(stderr, "%d ", out_hard[i]);
  }
  //fprintf(stderr, "\n");
  return noutput_items;
}
