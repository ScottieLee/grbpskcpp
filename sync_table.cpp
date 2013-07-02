#include "sync_table.h"

#include <numeric>
#include <assert.h>
#include <iostream>
sync_table::sync_table(unsigned int table_len):d_table_len(table_len), d_list_len(table_len+1)
{
}

sync_table& sync_table::make_sync_table(unsigned int tab_len)
{
    static sync_table st(tab_len);
    return st;
}

void sync_table::set_table_len(unsigned int table_len)
{
    d_table_len = table_len;
    d_list_len = table_len + 1;
}

void sync_table::push(unsigned int gt, unsigned int lt)
{
    d_gt.push_back(gt);
    d_lt.push_back(lt);
    if (d_gt.size() > d_list_len) {
        d_gt.pop_front();
        d_lt.pop_front();
    }
}

void sync_table::calc(float& offset, float& skew, float& lt_mean)
{
    for (int v = 0; v < d_list_len ; v++){
        std::cout << d_gt[v + 1] << "\t" << d_lt[v] << std::endl;
    }
    std::cout << std::endl;
    int diff_accum = 0;
    d_diff.clear();
    assert(d_gt.size() == d_list_len);
    for (int v = 0; v < d_table_len; v++) {
        int diff = d_gt[v + 1] - d_lt[v];
        d_diff.push_back(diff);
        diff_accum += diff;
    }
    offset = (float)diff_accum / d_table_len;
    int lt_accum = 0;
    for (int v = 0; v < d_table_len; v++) {
        lt_accum += d_lt[v];
    }
    lt_mean = lt_accum / d_table_len;

    float numerator = 0;
    float denominator = 0;
    for (int v = 0; v < d_table_len; v++) {
        numerator += (float)(d_lt[v] - lt_mean) * (float) (d_diff[v] - offset);
        denominator += (float) (d_lt[v] - lt_mean) * (d_lt[v] - lt_mean);
    }
    skew = numerator / denominator;
}

float sync_table::get_ltx(unsigned int gtx, float offset, float skew, float lt_mean)
{
    float ltx_float = (gtx - offset + skew * lt_mean) / ( 1 + skew);
    return ltx_float;
}

float sync_table::get_gtx(unsigned int ltx, float offset, float skew, float lt_mean)
{
    float gtx_float = ltx + offset + skew;
    return gtx_float;
}

bool sync_table::is_full()
{
    return d_gt.size() == d_list_len;
}

void sync_table::reset()
{
    d_gt.clear();
    d_lt.clear();
    d_diff.clear();
}

