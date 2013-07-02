#ifndef SYNC_TABLE_H
#define SYNC_TABLE_H

#include <vector>
#include <deque>
class sync_table
{
private:
    std::deque<unsigned int>       d_gt;
    std::deque<unsigned int>       d_lt;
    std::vector<unsigned int >      d_diff;
    unsigned int                    d_table_len;
    unsigned int                    d_list_len;
    sync_table(unsigned int table_len);

public:
    static sync_table& make_sync_table(unsigned int tab_len); //singleton pattern
    void set_table_len(unsigned int table_len);
    unsigned int get_table_len() { return d_table_len; }
    void push(unsigned int gt, unsigned int lt);
    bool is_full();
    void reset();
    void calc (float& offset, float& skew, float& lt_mean);
    int   get_len_now() { return d_gt.size();}
    float get_ltx(unsigned int gtx, float offset, float skew, float lt_mean); // calc ltx given gtx
    float get_gtx(unsigned int ltx, float offset, float skew, float lt_mean); // calc gtx given ltx
};

#endif // SYNC_TABLE_H
