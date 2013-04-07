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
     * Class used to calculate Dearden's g function and its derivative.
     * This is required to calculate g's inverse, the f function.
     */
    template<class RealType=double,
             class PolicyType=boost::math::policies::policy<> >
    class DeardenG
    {
    private:
        
        /**
         * Boost library calculation policy.
         */
        PolicyType policy_i;
        
    public:
        
        /**
         * Type of tuple returned by this classes () operator
         */
        typedef boost::math::tuple<RealType,RealType> Tuple;
        
        /**
         * Default constructor.
         */
        DeardenG() : policy_i() {}
        
        /**
         * Returns the result of this function.
         * This function is defined as
         * \f[
         * x = \log y - \psi(y)
         * \f]
         * where \f$\psi\f$ is the digamma function.
         */
        Tuple operator()(RealType y)
        {
            // NOTE currently boost as an implementation of digamma,
            // but no implementation for trigamma. We therefore fallback on
            // an implementations due to Jose Bernardo and BE Schneider,
            // ported from FORTRAN to C++ by John Burkardt and distributed
            // under the GPL license. Need to check license compatiblity
            // first though....
            double x = std::log(y) - boost::math::digamma(y);
            //double d = 1/y - trigamma TODO
            return Tuple(x,0);
        }
    
    }; // DeardenG class
    
    /**
     * Dearden's f function.
     * Required for calculating variational updates.
     * This function is defined as the inverse to Dearden's g function.
     * @param[in] x input \f$x\f$ to function
     * @param[in] policy boost library calculation policy
     * @returns \f$y\f$ such that \f[
     * x = \log y - \psi(y)
     * \f]
     */
    template<class RealType, class PolicyType>
    RealType deardenF(RealType x, PolicyType policy)
    {
        return 0;
        
    } // deardenF
    
    /**
     * Dearden's f function.
     * Required for calculating variational updates.
     * This function is defined as the inverse to Dearden's g function.
     * This version uses the default boost library calculation policy.
     * @param[in] x input \f$x\f$ to function
     * @returns \f$y\f$ such that \f[
     * x = \log y - \psi(y)
     * \f]
     */
    template<class RealType> RealType deardenF(RealType x)
    {
        boost::math::policies::policy<> policy;
        return deardenF(x,policy);
        
    } // deardenF
    
} // namespace dec_brl


#endif // DEC_BRL_SPECIAL_H