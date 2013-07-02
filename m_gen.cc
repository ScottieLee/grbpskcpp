#include "m_gen.h"
#include <stdio.h>

int main()
{
    m_gen m_generator;
    unsigned char m_hex[515];
    for(int v = 0; v < 515; v++){
        m_hex[v] = m_generator.next_byte();
    }
    printf("The hex form of m sequence is:\n");
    for(int v = 0; v < 515; v++)
    {
        printf("%.2x,", m_hex[v]);
    }
    printf("\nThe binary form of m sequence is:\n");
    for(int v = 0; v < 515; v++){
        printf("%d", (m_hex[v] & 0x80) >> 7);
        printf("%d", (m_hex[v] & 0x40) >> 6);
        printf("%d", (m_hex[v] & 0x20) >> 5);
        printf("%d", (m_hex[v] & 0x10) >> 4);
        printf("%d", (m_hex[v] & 0x08) >> 3);
        printf("%d", (m_hex[v] & 0x04) >> 2);
        printf("%d", (m_hex[v] & 0x02) >> 1);
        printf("%d", m_hex[v] & 0x01);
    }
    printf("\n");
    return 0;
}
