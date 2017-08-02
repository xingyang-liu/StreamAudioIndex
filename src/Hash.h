//
// Created by watson on 01/08/17.
//

#ifndef HASH_0E_HASH_H
#define HASH_0E_HASH_H

#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_map>



#include "stdint.h" /* Replace with <stdint.h> if appropriate */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

using google::dense_hash_map;
using google::sparse_hash_map;
using namespace std;

template <class T>
unsigned int BKDRHash(const T& str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    for(std::size_t i = 0; i < str.size(); i++)
    {
        hash = (hash * seed) + str[i];
    }
    return hash;
}

template <class T>
struct my_hash
{
    size_t operator()(const T& str) const
    {
        return BKDRHash(str);
    }
};

#endif //HASH_0E_HASH_H
