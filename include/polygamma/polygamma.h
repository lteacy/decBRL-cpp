/**
 * @file polygamma.h
 * Provides an implementation of the digamma and trigamma functions, which
 * are defined as the 1st and 2nd derivatives of the log gamma function.
 * This code is distributed under the GNU Lesser General Public License.
 * @author Original C++ code by John Burkardt, based on FORTRAN code by
 * Jose Bernardo, and BE Schneider. Library Interface and description
 * modified by Luke Teacy.
 * @see http://en.wikipedia.org/wiki/Polygamma_function
 */
#ifndef POLYGAMMA_H
#define POLYGAMMA_H

#include <cassert>

/**
 * Namespace containing implementations of the digamma and trigamma functions.
 * Currently, the implementation of these functions is provided by code made
 * available by John Burkardt under the GNU Lesser General Public License.
 * Therefore, the terms of this license must be respected with regard to this
 * proportion of the code, unless replaced by some other implementation of these 
 * functions made available under some other license.
 */
namespace polygamma {

    /**
     * Implementation of the digamma function
     * @param[in] x input to the digamma function
     * @param[out] ifault set to non-zero value if an error occurs during
     * calculation.
     * @returns \f[
     * y= \psi(x) = \frac{\Gamma'(x)}{\Gamma(x)} = \frac{d}{dx} \; \ln \Gamma(x)
     * \f]
     * @see http://en.wikipedia.org/wiki/Digamma_function
     */
    double digamma ( double x, int *ifault );
    
    /**
     * Implementation of the digamma function
     * @param[in] x input to the digamma function
     * @post assertion fails if an error occurs.
     * @returns \f[
     * y= \psi(x) = \frac{\Gamma'(x)}{\Gamma(x)} = \frac{d}{dx} \; \ln \Gamma(x)
     * \f]
     * @see http://en.wikipedia.org/wiki/Digamma_function
     */
    inline double digamma(double x)
    {
        int flaut;
        double result = digamma(x,&flaut);
        assert(0==flaut);
        return result;
    }
    
    /**
     * Returns test data for the digamma function.
     */
    void psi_values ( int *n_data, double *x, double *fx );
    
    /**
     * Implementation of the digamma function
     * @param[in] x input to the digamma function
     * @param[out] ifault set to non-zero value if an error occurs during
     * calculation.
     * @returns \f[
     * y = \psi_1(x) = \frac{d}{dx} \; \frac{\Gamma'(x)}{\Gamma(x)}
     * \f]
     * @see http://en.wikipedia.org/wiki/Trigamma_function
     */
    double trigamma ( double x, int *ifault );
    
    /**
     * Implementation of the trigamma function
     * @param[in] x input to the trigamma function
     * @post assertion fails if an error occurs.
     * @returns \f[
     * y = \psi(x) = \frac{d}{dx} \; \frac{\Gamma'(x)}{\Gamma(x)}
     * \f]
     * @see http://en.wikipedia.org/wiki/Trigamma_function
     */
    inline double trigamma(double x)
    {
        int flaut;
        double result = trigamma(x,&flaut);
        assert(0==flaut);
        return result;
    }
    
    /**
     * Returns test data for the trigamma function.
     */
    void trigamma_values ( int *n_data, double *x, double *fx );

}

#endif // POLYGAMMA_H
