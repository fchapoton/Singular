/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: sm_util.h,v 1.2 1997-07-15 13:01:08 schmidt Exp $ */

#ifndef INCL_SM_UTIL_H
#define INCL_SM_UTIL_H

//{{{ docu
//
// sm_util.h - header to sm_util.cc.
//
// Contributed by Marion Bruder <bruder@math.uni-sb.de>.
//
//}}}

#include <config.h>

#include "canonicalform.h"
#include "cf_reval.h"
#ifdef macintosh
#include <::templates:ftmpl_array.h>
#else
#include "templates/ftmpl_array.h"
#endif

typedef REArray Array<REvaluation>;

int countmonome( const CanonicalForm & f );

CanonicalForm Leitkoeffizient( const CanonicalForm & f );

void ChinesePoly( int arraylength, const CFArray & Polys, const CFArray & primes, CanonicalForm & result );

CanonicalForm dinterpol( int d, const CanonicalForm & gi, const CFArray & zwischen, const REvaluation & alpha, int s, const CFArray & beta, int ni, int CHAR );

CanonicalForm sinterpol( const CanonicalForm & gi, const REArray & xi, CanonicalForm* zwischen, int n );

#endif /* ! INCL_SM_UTIL_H */
