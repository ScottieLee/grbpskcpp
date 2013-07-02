#ifndef SOFT_SLICER_H
#define SOFT_SLICER_H

#include <gr_sync_block.h>
enum SLICE_MODE {
    POS_MAPPING = 1,
    NEG_MAPPING = 2
};

class soft_slicer;
typedef boost::shared_ptr<soft_slicer> soft_slicer_sptr;

soft_slicer_sptr make_soft_slicer ( SLICE_MODE mode);

/*!
 * \brief slice float binary symbol outputting 1 bit output
 * \ingroup converter_blk
 *
 * POS_MAPPING Slicing x <  0 --> 0
 * NEG_MAPPING Slicing x <  0 --> 1
 */
class soft_slicer : public gr_sync_block
{
  friend soft_slicer_sptr make_soft_slicer ( SLICE_MODE mode);
  soft_slicer ( SLICE_MODE mode);
private:
  SLICE_MODE d_mode;
public:
  int work (int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);

  int neg_slice(float x);
  int pos_slice(float x);
};

#endif // SOFT_SLICER_H
