#ifndef PACKET_UTILS_H
#define PACKET_UTILS_H
#include <string>
#include "short_codec.h"
#include <boost/crc.hpp>
#include <boost/cstdint.hpp> //For boost CRC
#include <algorithm> //For std::for_each in CRC calculation


class packet_utils
{
public:
    packet_utils();

    // Given a mpdu, do mpdu whitenning, then CRC appending, then channel encoding, then add preamble sync word PHY header and padding
    // Returns the PPDU length.
    static unsigned int mpdu2ppdu(const std::string& mpdu, // Pass in the mpdu
                                  std::string& ppdu, // Output the PPDU
                                  short_codec_sptr // Pass in the short encoder instance pointer for phy header encoding
                                  ); //FIXME ADD CRC_LEN、Channel coding、padding to length...

    // Given the soft bits constructed by the soft frame contor, do channel decoding, then checking CRC, then dewhitenning
    // Return the length of mpdu
    static unsigned int ppdu2mpdu(const std::string& ppdu, //Pass in Data get by the soft frame contor
                                  std::string& mpdu, //Output the MPDU
                                  bool& ok
                                  );

//    static void unmake_packet(std::string ppdu, double arg1, bool& ok, std::string& mpdu);
//    static void make_mpdu(unsigned char control, unsigned int pktno, std::string& payload,std::string& mpdu_out);
//    static void unmake_mpdu(std::string& mpdu_in, unsigned char& control, unsigned int& pktno, std::string &payload);
private:


    // PHY Layer Preamble
    static const char preamble[2];
    static const std::string PREAMBLE;
    // PHY Suffix
    static const char suffix[1];
    static const std::string SUFFIX;

public:
    // Access code (sync word)
    static const std::string ACCESS_CODE_1_0;
    static const std::string ACCESS_CODE_PACK;
};

#endif // PACKET_UTILS_H
