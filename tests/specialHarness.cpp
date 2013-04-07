/**
 * @file specialHarness.cpp
 * Test harness for special functions required for certain calculations.
 */

#include "dec_brl/special.h"
#include <iostream>

// private module namespace
namespace {

/**
 * Check that two doubles are equal within margin of error.
 */
bool equalWithinTol_m(double v1, double v2, double tol=0.01)
{
    return tol > (std::abs(v1-v2)/std::min(v1,v2));
}
    
} // module namespace

/**
 * Main function implementing tests.
 */
int main()
{
    using namespace dec_brl;
    
    std::cout << "Hello world!" << std::endl;
    
    //**************************************************************************
    //  Hard coded test set from matlab
    //**************************************************************************
    double x[] = {0.577216,0.368975,0.270363,0.213134,0.175828,0.149606,
                  0.130177,0.115206,0.103320,0.093655,0.085642,0.078891,
                  0.073126,0.068146,0.063800,0.059975,0.056583,0.053554,
                  0.050833};
    
    double y[] = {1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,5.5,6.0,6.5,7.0,
                  7.5,8.0,8.5,9.0,9.5,10};
    
    // derivative of x=g(y)
    double d[] = {-0.644934,-0.268136,-0.144934,-0.090358,-0.061601,
                  -0.044643,-0.033823,-0.026503,-0.021323,-0.017524,
                  -0.014656,-0.012438,-0.010688,-0.009283,-0.008137,
                  -0.007191,-0.006401,-0.005734,-0.005166};
    
    //**************************************************************************
    //  Check that we can reproduce these results with reasonable accuracy
    //**************************************************************************
    DeardenG<> deardenG;
    for(int k=0; k<20; ++k)
    {
        double correctX = x[k];
        double correctY = y[k];
        double correctD = d[k];
        
        //**********************************************************************
        //  Try to calculate X and its derivative D (w.r.t. Y)
        //**********************************************************************
        std::cout << "Calculating x[" << k << "], d[" << k << "]" << std::flush;
        DeardenG<>::Tuple tupleX = deardenG(correctY);
        double computeX = boost::math::get<0>(tupleX);
        double computeD = boost::math::get<1>(tupleX);
        
        //**********************************************************************
        //  Try to estimate Y
        //**********************************************************************
        std::cout << " and y[" << k << "]" << std::endl;
        double computeY = deardenF(correctX);
        
        //**********************************************************************
        //  Check the results
        //**********************************************************************
        std::cout << "Checking results...";
        
        if(!equalWithinTol_m(correctX, computeX))
        {
            std::cout << "BAD X: " << correctX << "!=" << computeX << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "X..";
        
        
        if(!equalWithinTol_m(correctD, computeD))
        {
            std::cout << "BAD D: " << correctD << "!=" << computeD << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "D..";
        
        if(!equalWithinTol_m(correctY, computeY))
        {
            std::cout << "BAD Y: " << correctY << "!=" << computeY << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Y..";
        
        std::cout << "OK" << std::endl;
        
    } // for loop
    
} // function main