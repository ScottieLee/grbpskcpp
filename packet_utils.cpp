#include "packet_utils.h"
#include "top_block_simulation.h"
#include <assert.h>
#include "string_functions.h"
#include "scrambler.h"
#include <iostream>

const char packet_utils::preamble[2] = {0xA4, 0xF2};
const std::string packet_utils::PREAMBLE = std::string((char*)preamble, 2);

const char packet_utils::suffix[1] = {0x55};
const std::string packet_utils::SUFFIX = std::string((char*)suffix, 1);

const std::string packet_utils::ACCESS_CODE_1_0 = "00000011010001110111011011000111001001110010100010010101101100001111110010111000100010010011100011011000110101110110101001001111";
const std::string packet_utils::ACCESS_CODE_PACK= string_1_0_to_packed_string(ACCESS_CODE_1_0);

packet_utils::packet_utils()
{
}

//FIXME!!!! This function changes the input mpdu!!!!!! WHY?
unsigned int packet_utils::mpdu2ppdu(const std::string& mpdu, std::string& ppdu, short_codec_sptr phy_header_encoder)
{
    // scrambling
    std::string mpdu_whitened(mpdu); // Copy
    scrambler sc;
    char* mpdu_whitened_ptr = const_cast<char*>(mpdu_whitened.data());
    for(int v = 0; v < mpdu_whitened.size(); v++){
        unsigned char next_byte = sc.next_byte();
        *(mpdu_whitened_ptr + v ) ^= next_byte;
    }

    // make phy header
    unsigned int len = mpdu.size(); //Len of Bytes
    assert(len < 4096);
    std::vector<bool> mpdu_len(24, 0);  // Use 24 bit representing length
    for (int v = 0; v < 24; v++){
        mpdu_len[v] = (len >> (23 - v)) & 0x01;
    }
    phy_header_encoder -> load_input(mpdu_len);
    phy_header_encoder -> process();
    std::vector<bool> phy_header_vec = phy_header_encoder -> result();// After header coding, header bit length should be 120.
//    std::cout << "Phy header vec:" << std::endl;
//    for (int v = 0; v < phy_header_vec.size(); v++) {
//        std::cout << phy_header_vec[v] << "\t";
//    }
    std::cout << std::endl;
    assert(phy_header_vec.size() == 120);
    std::string phy_header_1_0;
    for(int v = 0; v < phy_header_vec.size(); v++){
        phy_header_1_0 = phy_header_1_0 + (phy_header_vec[v]?"1":"0");
    }

    // Add
    std::string phy_header_pack;
    string_1_0_to_packed_string(phy_header_1_0, phy_header_pack);
    //std::cout << "MPDU:" << string_to_hex(mpdu) << std::endl;
    ppdu = PREAMBLE + ACCESS_CODE_PACK + phy_header_pack + mpdu + SUFFIX;
    //std::cout << "Len = " << ppdu.size()<< std::endl;
    std::cout << string_to_hex(ppdu) << std::endl;
}


unsigned int packet_utils::ppdu2mpdu(const std::string& ppdu, std::string& mpdu, bool& ok)
{
    std::cout << "Receive Frame!!!" << std::endl;
    float* ppdu_parser = (float*) ppdu.data();

    // Decode it!

    // Dewhiten it !

    // CRC check!!
    for (int v = 0; v < ppdu.size()/sizeof(float); v++){
        std::cout << (*(ppdu_parser + v)>0? 0 : 1) << "\t";
    }
    std::cout << std::endl;
    std::cout << ppdu.size()<< std::endl;
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
