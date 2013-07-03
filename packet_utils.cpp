#include "packet_utils.h"
#include "top_block_simulation.h"
#include <assert.h>
#include "string_functions.h"
#include "scrambler.h"
#include <iostream>

#define VERBOSE 0
const char packet_utils::preamble[2] = {0xA4, 0xF2};
const std::string packet_utils::PREAMBLE = std::string((char*)preamble, 2);

const char packet_utils::suffix[1] = {0x55};
const std::string packet_utils::SUFFIX = std::string((char*)suffix, 1);

const std::string packet_utils::ACCESS_CODE_1_0 = "00000011010001110111011011000111001001110010100010010101101100001111110010111000100010010011100011011000110101110110101001001111";
const std::string packet_utils::ACCESS_CODE_PACK= string_1_0_to_packed_string(ACCESS_CODE_1_0);


packet_utils::packet_utils()
{
}

unsigned int packet_utils::mpdu2ppdu(const std::string& mpdu, std::string& ppdu, short_codec_sptr phy_header_encoder)
{
    // scrambling
    char* mpdu_whitened = new char[mpdu.size() + sizeof(unsigned short)]; // Create a char string which size is length of mpdu plus length of CRC
    memcpy(mpdu_whitened, mpdu.data(), sizeof(char)*mpdu.size());
    scrambler sc; // If this is static, the whitenning patterns are different every call.
                  // If this is non-static, the whitenning patterns are same every call.
    for(int v = 0; v < mpdu.size(); v++){
        unsigned char next_byte = sc.next_byte();
        *(mpdu_whitened + v ) ^= next_byte;
    }

    // Appending CRC
    boost::crc_optimal<16, 0x1021, 0xFFFF, 0, false, false> crc_ccitt;
    crc_ccitt = std::for_each( mpdu_whitened, mpdu_whitened + mpdu.size(), crc_ccitt );
    unsigned short crc_result = crc_ccitt();
    memcpy(mpdu_whitened + sizeof(char)*mpdu.size(), &crc_result, sizeof(unsigned short));
    std::string mpdu_with_CRC = std::string(mpdu_whitened, mpdu_whitened + sizeof(char)*mpdu.size() + sizeof(unsigned short));

    // Channel coding
    std::string mpdu_coded = mpdu_with_CRC;

    // make phy header
    unsigned int len = mpdu_coded.size(); //Len of Bytes
    assert(len < 4096);
    std::vector<bool> mpdu_len(24, 0);  // Use 24 bit representing length
    for (int v = 0; v < 24; v++){
        mpdu_len[v] = (len >> (23 - v)) & 0x01;
    }
    phy_header_encoder -> load_input(mpdu_len);
    phy_header_encoder -> process();
    std::vector<bool> phy_header_vec = phy_header_encoder -> result();// After header coding, header bit length should be 120.
#if VERBOSE
    std::cout << "Phy header vec:" << std::endl;
    for (int v = 0; v < phy_header_vec.size(); v++) {
        std::cout << phy_header_vec[v] << "\t";
    }
    std::cout << std::endl;
#endif
    assert(phy_header_vec.size() == 120); // The header must be 120 bit till now.
    std::string phy_header_1_0;
    for(int v = 0; v < phy_header_vec.size(); v++){
        phy_header_1_0 = phy_header_1_0 + (phy_header_vec[v]?"1":"0");
    }

    // Add
    std::string phy_header_pack;
    string_1_0_to_packed_string(phy_header_1_0, phy_header_pack);
    ppdu = PREAMBLE + ACCESS_CODE_PACK + phy_header_pack + mpdu_coded + SUFFIX;

    // Free allocated heap
    delete[] mpdu_whitened;
}


unsigned int packet_utils::ppdu2mpdu(const std::string& ppdu, std::string& mpdu, bool& ok)
{
    //The ppdu data are soft bits, each bit is a float.
    float* ppdu_parser = (float*) (ppdu.data()); // Reinterpret cast to float[]

    // Decode it!
    int mpdu_len_with_crc = ppdu.size()/sizeof(float)/8;
    char* decoded_mpdu = new char[mpdu_len_with_crc];
    for (int v = 0; v < mpdu_len_with_crc ; v++){
        decoded_mpdu[v] = 0x00;
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+0)>0 ? 0 : 1) << 7 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+1)>0 ? 0 : 1) << 6 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+2)>0 ? 0 : 1) << 5 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+3)>0 ? 0 : 1) << 4 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+4)>0 ? 0 : 1) << 3 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+5)>0 ? 0 : 1) << 2 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+6)>0 ? 0 : 1) << 1 );
        decoded_mpdu[v] |= ( (*(ppdu_parser + v*8+7)>0 ? 0 : 1));
    }



    // CRC check!!
    boost::crc_optimal<16, 0x1021, 0xFFFF, 0, false, false> crc_ccitt;
    crc_ccitt = std::for_each( decoded_mpdu, decoded_mpdu + mpdu_len_with_crc - sizeof(short), crc_ccitt );
    unsigned short crc_result = crc_ccitt();
    unsigned short rev_crc;
    memcpy(&rev_crc, decoded_mpdu + mpdu_len_with_crc - sizeof(short), sizeof(short));
    ok =(rev_crc == crc_result) ? true : false;
    // Dewhiten it !
    scrambler sc; // If this is static, the whitenning patterns are different every call.
                  // If this is non-static, the whitenning patterns are same every call.
    for(int v = 0; v < mpdu_len_with_crc; v++){
        unsigned char next_byte = sc.next_byte();
        *(decoded_mpdu + v ) ^= next_byte;
    }
    // Give mpdu
    mpdu = std::string(decoded_mpdu, decoded_mpdu + mpdu_len_with_crc - sizeof(short));
}

//unsigned int packet_utils::make_packet(const std::string &mpdu, std::string &ppdu, )
//{
//    unsigned short mpdu_length = mpdu.length();
//    assert(mpdu_length < 4096);
//    std::string header;
//    make_header(mpdu_length, header);
//    char suffix[1] = {0x55};
//    std::string suffix_str = std::string((char*) suffix, 1);
//    ppdu = PREAMBLE + "" + header + mpdu + suffix_str + suffix_str;

//}
//void packet_utils::make_header(unsigned short mpdu_length, std::string& header)
//{
// // NOTICE!!!!!!!!!!!! THIS INVOLVES BYTE ORDER ISSUE!!!!
////    std::cout << "mpdu length = " << mpdu_length << std::endl;
//    unsigned short mpdu_length_tmp = mpdu_length & 0x0fff ;
//    char* p = (char*)&mpdu_length;
//    std::string str = std::string((char*)&mpdu_length_tmp, sizeof(unsigned short));
//    std::reverse(str.begin(), str.end()); //BYTE ORDER!!
//    header = str + str;  //double repeat;
////    std::cout << string_to_hex(header) << std::endl;

//}

//void packet_utils::unmake_packet(std::string ppdu, double arg1, bool& ok, std::string& mpdu)
//{
//    mpdu = ppdu;
//    ok =true;
//}


//void packet_utils::make_mpdu(unsigned char control, unsigned int pktno, std::string& payload, std::string& mpdu_out)
//{
//    std::string control_str;
//    conv_uchar_to_string(control, control_str);
//    control_str = repeat(control_str, 8);
//    std::string pktno_str;
//    conv_uint_to_string(pktno, pktno_str);
//    pktno_str = repeat(pktno_str, 4);
//    mpdu_out = control_str + pktno_str + payload;
//    // scrambling
//    scrambler sc;  //not good style
//    char* mpdu_out_ptr = const_cast<char*>(mpdu_out.data());
//    for(int v = 0; v < mpdu_out.size(); v++){
//        unsigned char next_byte = sc.next_byte();
//        *(mpdu_out_ptr + v ) ^= next_byte;
//    }
////    std::cout << string_to_hex(mpdu_out) << std::endl;
//}
//void packet_utils::unmake_mpdu(std::string& mpdu_in, unsigned char& control, unsigned int& pktno, std::string &payload)
//{
//    // discrambling
//    scrambler sc;  //not good style
//    char* mpdu_in_ptr = const_cast<char*>(mpdu_in.data());
//    for(int v = 0; v < mpdu_in.size(); v++){
//        unsigned char next_byte = sc.next_byte();
//        *(mpdu_in_ptr + v ) ^= next_byte;
//    }

//    std::string::iterator it = mpdu_in.begin();
//    payload = mpdu_in.substr(24, 24 + 32 * 4 );// 8* control + 4* pktno
//    std::vector<unsigned char> control_vec(mpdu_in.begin(), it + 8);
//    std::vector<int>           count1;
//    std::vector<int>::iterator max_pos1;
//    int max_index1;
//    for (int v = 0; v < 8; v++)
//    {
//        count1.push_back ( (int) std::count(control_vec.begin(), control_vec.end(), control_vec[v]) );
//    }
//    max_pos1 = max_element (count1.begin(), count1.end());
//    max_index1 = std::distance(count1.begin(),max_pos1);
//    control =  control_vec[max_index1];
//    count1.clear();
//    std::vector<unsigned int> pktno_vec(4);
//    std::string tmp = mpdu_in.substr(8,12);
//    conv_string_to_uint(tmp, pktno_vec[0]);
//    tmp = mpdu_in.substr(12,16);
//    conv_string_to_uint(tmp,pktno_vec[1]);
//    tmp = mpdu_in.substr(16,20);
//    conv_string_to_uint(tmp,pktno_vec[2]);
//    tmp = mpdu_in.substr(20,24);
//    conv_string_to_uint(tmp,pktno_vec[3]);

//    for (int v = 0; v < 4; v++)
//    {
//        count1.push_back ( (int) std::count(pktno_vec.begin(), pktno_vec.end(), pktno_vec[v]) );
//    }
//    max_pos1 = max_element (count1.begin(), count1.end());
//    max_index1 = std::distance(count1.begin(),max_pos1);
//    pktno =  pktno_vec[max_index1];
//}
