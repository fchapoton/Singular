#ifndef MODULOP_H
#define MODULOP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo p (<=32749)
*/
#include "misc/auxiliary.h"


// define if a*b is with mod instead of tables
//#define HAVE_GENERIC_MULT
// define if 1/b is from  tables
//#define HAVE_INVTABLE
// define if an if should be used
//#define HAVE_GENERIC_ADD

//#undef HAVE_GENERIC_ADD
//#undef HAVE_GENERIC_MULT
//#undef HAVE_INVTABLE

//#define HAVE_GENERIC_ADD 1
//#define HAVE_GENERIC_MULT 1
//#define HAVE_INVTABLE 1

// enable large primes (32749 < p < 2^31-)
#define NV_OPS
#define NV_MAX_PRIME 32749
#define FACTORY_MAX_PRIME 536870909

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

BOOLEAN npInitChar(coeffs r, void* p);

// inline number npMultM(number a, number b, int npPrimeM)
// // return (a*b)%n
// {
//    double ab;
//    long q, res;
//
//    ab = ((double) ((int)a)) * ((double) ((int)b));
//    q  = (long) (ab/((double) npPrimeM));  // q could be off by (+/-) 1
//    res = (long) (ab - ((double) q)*((double) npPrimeM));
//    res += (res >> 31) & npPrimeM;
//    res -= npPrimeM;
//    res += (res >> 31) & npPrimeM;
//    return (number)res;
// }
#ifdef HAVE_GENERIC_MULT
static inline number npMultM(number a, number b, const coeffs r)
{
  return (number)
    ((((unsigned long) a)*((unsigned long) b)) % ((unsigned long) r->ch));
}
static inline void npInpMultM(number &a, number b, const coeffs r)
{
  a=(number)
    ((((unsigned long) a)*((unsigned long) b)) % ((unsigned long) r->ch));
}
#else
static inline number npMultM(number a, number b, const coeffs r)
{
  long x = (long)r->npLogTable[(long)a]+ r->npLogTable[(long)b];
  #ifdef HAVE_GENERIC_ADD
  if (x>r->npPminus1M) x-=r->npPminus1M;
  #else
    x-=r->npPminus1M;
    #if SIZEOF_LONG == 8
     x += (x >> 63) & r->npPminus1M;
    #else
     x += (x >> 31) & r->npPminus1M;
    #endif
  #endif
  return (number)(long)r->npExpTable[x];
}
static inline void npInpMultM(number &a, number b, const coeffs r)
{
  long x = (long)r->npLogTable[(long)a]+ r->npLogTable[(long)b];
  #ifdef HAVE_GENERIC_ADD
  if (x>r->npPminus1M) x-=r->npPminus1M;
  #else
    x-=r->npPminus1M;
    #if SIZEOF_LONG == 8
     x += (x >> 63) & r->npPminus1M;
    #else
     x += (x >> 31) & r->npPminus1M;
    #endif
  #endif
  a=(number)(long)r->npExpTable[x];
}
#endif

#if 0
inline number npAddAsm(number a, number b, int m)
{
  number r;
    asm ("addl %2, %1; cmpl %3, %1; jb 0f; subl %3, %1; 0:"
         : "=&r" (r)
         : "%0" (a), "g" (b), "g" (m)
         : "cc");
  return r;
}
inline number npSubAsm(number a, number b, int m)
{
  number r;
  asm ("subl %2, %1; jnc 0f; addl %3, %1; 0:"
        : "=&r" (r)
        : "%0" (a), "g" (b), "g" (m)
        : "cc");
  return r;
}
#endif
#ifdef HAVE_GENERIC_ADD
static inline number npAddM(number a, number b, const coeffs r)
{
  unsigned long R = (unsigned long)a + (unsigned long)b;
  return (number)(R >= r->ch ? R - r->ch : R);
}
static inline void npInpAddM(number &a, number b, const coeffs r)
{
  unsigned long R = (unsigned long)a + (unsigned long)b;
  a=(number)(R >= r->ch ? R - r->ch : R);
}
static inline number npSubM(number a, number b, const coeffs r)
{
  return (number)((long)a<(long)b ?
                       r->ch-(long)b+(long)a : (long)a-(long)b);
}
#else
static inline number npAddM(number a, number b, const coeffs r)
{
   unsigned long res = (long)((unsigned long)a + (unsigned long)b);
   res -= r->ch;
#if SIZEOF_LONG == 8
   res += ((long)res >> 63) & r->ch;
#else
   res += ((long)res >> 31) & r->ch;
#endif
   return (number)res;
}
static inline void npInpAddM(number &a, number b, const coeffs r)
{
   unsigned long res = (long)((unsigned long)a + (unsigned long)b);
   res -= r->ch;
#if SIZEOF_LONG == 8
   res += ((long)res >> 63) & r->ch;
#else
   res += ((long)res >> 31) & r->ch;
#endif
   a=(number)res;
}
static inline number npSubM(number a, number b, const coeffs r)
{
   long res = ((long)a - (long)b);
#if SIZEOF_LONG == 8
   res += (res >> 63) & r->ch;
#else
   res += (res >> 31) & r->ch;
#endif
   return (number)res;
}
#endif

static inline number npNegM(number a, const coeffs r)
{
  return (number)((long)(r->ch)-(long)(a));
}

static inline BOOLEAN npIsZeroM (number  a, const coeffs)
{
  return 0 == (long)a;
}

// inline number npMultM(number a, number b, int npPrimeM)
// {
//   return (number)(((long)a*(long)b) % npPrimeM);
// }

// The folloing is reused inside gnumpc.cc, gnumpfl.cc and longrat.cc
long    npInt         (number &n, const coeffs r);

// The following is currently used in OPAE.cc, OPAEQ.cc and OPAEp.cc for setting their SetMap...
nMapFunc npSetMap(const coeffs src, const coeffs dst); // FIXME! BUG?

#define npEqualM(A,B,r)  ((A)==(B))


#endif
