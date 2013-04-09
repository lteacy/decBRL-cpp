/**
 * @file specialHarness.cpp
 * Test harness for special functions required for certain calculations.
 */

#include "dec_brl/special.h"
#include <iostream>
#include <algorithm>

// private module namespace
namespace {

/**
 * Check that two doubles are equal within margin of error.
 */
bool equalWithinTol_m(double v1, double v2, double tol=0.002)
{
   double error = (std::abs(v1-v2)/std::abs(std::min(v1,v2)));
    return tol > error;
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
                  0.050833,0.010033,0.005008,0.000833565};
    
    double y[] = {1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,5.5,6.0,6.5,7.0,
                  7.5,8.0,8.5,9.0,9.5,10,50,100,600};
    
    // derivative of x=g(y)
    double d[] = {-0.644934,-0.268136,-0.144934,-0.090358,-0.061601,
                  -0.044643,-0.033823,-0.026503,-0.021323,-0.017524,
                  -0.014656,-0.012438,-0.010688,-0.009283,-0.008137,
                  -0.007191,-0.006401,-0.005734,-0.005166,-0.000201,
                  -5.01667e-05,-1.38966e-06};
    
    //**************************************************************************
    //  Check that we can reproduce these results with reasonable accuracy
    //**************************************************************************
    DeardenG<> deardenG;
    int errorCount = 0;
    for(int k=0; k<22; ++k)
    {
        double correctX = x[k];
        double correctY = y[k];
        double correctD = d[k];
        
        //**********************************************************************
        //  Try to calculate X and its derivative D (w.r.t. Y)
        //  Note deardenG is w.r.t. to log Y
        //**********************************************************************
        std::cout << "Calculating x[" << k << "], d[" << k << "]" << std::flush;
        deardenG.setTarget(correctX);
        DeardenG<>::Tuple tupleX = deardenG(std::log(correctY));
        double computeX = boost::math::get<0>(tupleX)+correctX;
        double computeD = boost::math::get<1>(tupleX)/correctY;
        
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
            ++errorCount;
        }
        else
        {
            std::cout << "X..";
        }
        
        if(!equalWithinTol_m(correctD, computeD))
        {
            std::cout << "BAD D: " << correctD << "!=" << computeD << std::endl;
            ++errorCount;
        }
        else
        {
            std::cout << "D..";
        }
        
        //**********************************************************************
        //  In the case of Y, we place a threshold on this because we are only
        //  interested in values more than 1 (since these will be used as
        //  alpha parameters for gamma distributions). Therefore, we actually
        //  compare against the bound, if the correctY is less than this bound.
        //**********************************************************************
        double targetY = std::max(correctY,std::exp(MIN_LOG_DEARDEN_F));
        targetY = std::min(targetY ,std::exp(MAX_LOG_DEARDEN_F));
        if(!equalWithinTol_m(targetY, computeY))
        {
            std::cout << "BAD Y: " << targetY << "!=" << computeY << std::endl;
            ++errorCount;
        }
        else
        {
            std::cout << "Y=" << computeY << " ..OK";
        }
        
        std::cout << std::endl;
        
    } // for loop
    
    if(0==errorCount)
    {
        std::cout << "All OK" << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << "Number of Errors: " << errorCount << std::endl;
        return EXIT_FAILURE;
    }
    
} // function main
