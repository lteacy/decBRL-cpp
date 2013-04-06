/**
 * @file special.h
 * Definition of special functions required for certain calculations.
 */
#ifndef DEC_BRL_SPECIAL_H
#define DEC_BRL_SPECIAL_H

#include <boost/math/tools/tuple.hpp>
#include <boost/math/policies/policy.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/math/special_functions/digamma.hpp>
#include <cmath>

namespace dec_brl {
    
    /**
     * Dearden's f function.
     * Required for calculating variational updates.
     */
    template<class RealType, class PolicyType>
    RealType deardenF(RealType x, PolicyType policy)
    {
        return 0;
        
    } // deardenF
    
    /**
     * Dearden's f function.
     * Required for calculating variational updates.
     */
    template<class RealType> RealType deardenF(T x)
    {
        boost::math::policies::policy<> policy;
        return deardenF(x,policy);
        
    } // deardenF
    
} // namespace dec_brl


#endif // DEC_BRL_SPECIAL_H