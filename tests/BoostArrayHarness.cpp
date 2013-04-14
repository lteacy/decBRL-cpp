/**
 * @file BoostArrayHarness.cpp
 * Test harness for dec_brl::BoostCompatibleArray type.
 * @author Luke Teacy
 */

#include <iostream>
#include <boost/math/tools/precision.hpp>
#include "dec_brl/BoostCompatibleArray.h"

int main()
{
    using namespace dec_brl;
    
    //**************************************************************************
    //  Construct examples
    //**************************************************************************
    std::cout << "Constructing..." << std::endl;
    BoostCompatibleArray<double> x, y(3);
    x.resize(3);
    x << 1,2,3;
    BoostCompatibleArray<double> w(x), z(y);
    
    std::cout << "w: " << w << std::endl;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "z: " << z << std::endl;

    //**************************************************************************
    //  Try assignment
    //**************************************************************************
    std::cout << "Assigning..." << std::endl;
    y = 4;
    z = x;
    x = 5;
    w = 10;
    
    std::cout << "w: " << w << std::endl;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "z: " << z << std::endl;
    
    //**************************************************************************
    //  Try arthimetic
    //**************************************************************************
    std::cout << "Trying arithmetic" << std::endl;
    z += 100;
    z -= 50;
    z *= 2;
    z /= 4;
    w = (z + 200);
    x *= z;
    x = 0;
    x /= z;
    x = 0;
    x += z;
    x = 0;
    x -= z;
    y = z - 50 + w * x;
    
    std::cout << "w: " << w << std::endl;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "z: " << z << std::endl;
    
    //**************************************************************************
    // Misc Functions
    //**************************************************************************
    typedef BoostCompatibleArray<float> RealType;
    //typedef int RealType;
    int digits = boost::math::tools::digits<RealType>();
    RealType max = boost::math::tools::max_value<RealType>();
    RealType min = boost::math::tools::min_value<RealType>();
    RealType logMax = boost::math::tools::log_max_value<RealType>();
    RealType logMin = boost::math::tools::log_min_value<RealType>();
    RealType epsilon = boost::math::tools::epsilon<RealType>();
    
    std::cout << "digits: " << digits << std::endl;
    std::cout << "max: " << max << std::endl;
    std::cout << "min: " << min << std::endl;
    std::cout << "logMax: " << logMax << std::endl;
    std::cout << "logMin: " << logMin << std::endl;
    std::cout << "epsilon: " << epsilon << std::endl;
    
    return EXIT_SUCCESS;
    
} // main function