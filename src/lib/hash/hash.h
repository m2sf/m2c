/*  Gemeral purpose 32-bit hash function
 *
 *  hash.h
 *
 *  This file ("hash.h") is in the public domain.
 */

#ifndef HASH_H
#define HASH_H

// initial value
#define HASH_INITIAL 0

// iterative value
#define HASH_NEXT_CHAR(_hash,_ch) \
    (_ch + (_hash << 6) + (_hash << 16) - _hash)

// final value
#define HASH_FINAL(_hash) (hash & 0x7FFFFFFF)

#endif /* HASH_H */

/* END OF FILE */