#ifndef TR_UTILS_H
#define TR_UTILS_H

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

// Explanation for the bit-fiddling uneducated: if x is a power of 2, it will
// have a single bit set (lets say it's in position N).
// Its predecessor then will necessarily have only the bits from 0 to N-1 set.
// Therefore, ORing a power of 2 with it's predecessor nets 0.
// This is not valid for 0, so we include a check for it.

#define IS_POWER_OF_2(x) ((x) != 0 && ((x) & ((x)-1)) == 0)

// Compile-time assertion macro.
#define VERIFY(e) extern char (*ct_assert(void)) [sizeof(char[1 - 2*!(e)])]




#endif // #ifndef TR_UTILS_H