#ifndef CONTROL_WORD_H
#define CONTROL_WORD_H

#include <map>

static std::map<int, unsigned char> control_word;
#define SLAVE1_DATA   0x16
#define SLAVE2_DATA   0x42
#define SLAVE3_DATA   0xf2
#define MASTER_BEACON 0x55



#endif // CONTROL_WORD_H
