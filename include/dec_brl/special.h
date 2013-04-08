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
#include "polygamma/polygamma.h"
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
         * For computational reasons, we actually supply the log of y as input.
         * This is because we are primarily interested in calculating the 
         * alpha parameter for gamma distributions by taking the inverse of
         * this equation. The derivative is more stable w.r.t to log alpha,
         * making it more favourable to root finding. Also, the shape of
         * the gamma distribution varies little for large alpha, so as long
         * as we can estimate its log reasonably well, we'll be close enough.
         * @param ly the natural logrithm of the input, y.
         * \f[
         * x = \ln y - \psi(y) = ly - \psi(\exp[ly])
         * \f]
         * where \f$\psi\f$ is the digamma function.
         */
        Tuple operator()(RealType ly)
        {
            //******************************************************************
            // NOTE currently boost as an implementation of digamma,
            // but no implementation for trigamma. We therefore fallback on
            // an implementations due to Jose Bernardo and BE Schneider,
            // ported from FORTRAN to C++ by John Burkardt and distributed
            // under the LGPL license. These functions are included under
            // the polygamma namespace, and so the terms of the LGPL must be
            // respected with regard to the functions contained in this
            // namespace. Eventually, we may replace these functions with code
            // more compatible with the BSD license applied to the rest of this
            // library. E.g. inclusion of a trigamma implementation in boost
            // would do nicely!
            //
            // Currently using boost digamma implementation, but digamma is
            // also available in the polygamma namespace under the LGPL license.
            // Could also use this for consistency.
            //******************************************************************
            double y = std::exp(ly);
            double x= ly - boost::math::digamma(y,policy_i);
            double d = 1 - polygamma::trigamma(y)*y;
            return Tuple(x,d);
        }
    
    }; // DeardenG class
    
    /**
     * Log of maximum value that may be returned by the deardenF function.
     * This saves on computation, and if the true value is larger, the effect
     * on our application is not big, since the shape of the gamma distribution
     * is similar for all large alpha.
     */
    const static double MAX_LOG_DEARDEN_F = 6.0;
    
    /**
     * Log of minimum value that may be returned by the deardenF function.
     * This saves on computation, but also, we're only interested in alpha
     * hyperparameters that result in proper gamma distributions i.e. > 1.0.
     */
    const static double MIN_LOG_DEARDEN_F = 0.0001;
    
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
        using namespace boost::math::tools;
        
        //**********************************************************************
        //  No closed form solution exists for this function, we have to solve
        //  by finding the root of the inverse - deardenG.
        //
        //  We start by guessing the true value.
        //**********************************************************************
        double guess = 2.0; // constant guess for now
        
        //**********************************************************************
        // Set sensible minimum and maximum values. In our case, we're only
        // interested in values that make sensible alpha values for our normal
        // gamma conjugate prior i.e. > 1.0. Also, large values of alpha do
        // no significantly change the shape of the gamma distribution, so might
        // as well limit the max to save us looking forever.
        //**********************************************************************
        boost::uintmax_t& max_iter = 10; // max no. iterations to find solution
        const int digits = 8; // no. required binary digits in solution mantissa
        DeardenG<> GFunc; // function we want to find the root for
                          // need this to return 0 for the point we are interested in, right?
                          // so need to subtract target value.
        return newton_raphson_iterate(GFunc, guess, MIN_LOG_DEARDEN_F, MAX_LOG_DEARDEN_F, digits, max_iter);
        
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
