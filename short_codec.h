#ifndef SHORT_CODEC_H
#define SHORT_CODEC_H
#include <stdlib.h>
#include <boost/make_shared.hpp>
#include <vector>

class short_codec;
typedef boost::shared_ptr<short_codec> short_codec_sptr;
class short_codec {
  private:
      std::vector<bool>     d_input_buf;
      std::vector<bool>     d_output_buf;
  protected:
      std::vector<bool>& get_input_buf()  { return d_input_buf; }
      std::vector<bool>& get_output_buf() { return d_output_buf;}
  public:
      short_codec();
      virtual ~short_codec() {}
      void load_input(const std::vector<bool>& input) {
          d_input_buf.clear();
          d_input_buf = input;
      }
      void clear()
      { 
          d_input_buf.clear();
          d_output_buf.clear();
      }
      virtual bool process() { return true; }
      const std::vector<bool>& result() { return d_output_buf;}
};


class short_encoder: public short_codec {
  protected:
      std::vector<bool>& get_encode_buf() { return get_input_buf(); }
      std::vector<bool>& get_outbuf()     { return get_output_buf(); }
  public:
      short_encoder();
      virtual ~short_encoder() {}
};

class short_decoder: public short_codec {
  protected:
      std::vector<bool>& get_decode_buf() { return get_input_buf(); }
      std::vector<bool>& get_outbuf()     { return get_output_buf(); }
  public:
      short_decoder();
      virtual ~short_decoder() {}

};

class repetition_encoder: public short_encoder {
  private:
      int d_repetition_factor;
  public:
      virtual bool process();
      repetition_encoder(int repetition_factor);
};

class repetition_decoder: public short_decoder {
  private:
      int d_repetition_factor;
  public:
      virtual bool process();
      repetition_decoder(int repetition_factor);
};

#endif
