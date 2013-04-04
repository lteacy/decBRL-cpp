/**
 * @file DirichletHarness.cpp
 * Test harness for dec_brl::Dirichlet type.
 * @author Luke Teacy
 */

#include <iostream>
#include "dec_brl/Dirichlet.h"

int main()
{
    using namespace dec_brl::dist;
    using namespace dec_brl;
    
    //**************************************************************************
    // Try to construct a new Dirichlet or two
    //**************************************************************************
    Dirichlet a, b(4,2), c(3);
    
    std::cout << "a = " << a.alpha << std::endl;
    std::cout << "b = " << b.alpha << std::endl;
    std::cout << "c = " << c.alpha << std::endl;
    
    //**************************************************************************
    //  Try observing some values
    //**************************************************************************
    std::cout << "Trying observations...\n";
    observe(a,1);
    Eigen::Array<int,1,Eigen::Dynamic> obs_b(b.alpha.size());
    obs_b << 10, 11, 12, 13;
    observe(b,obs_b);
    
    Eigen::Array<int,1,3> obs_c;
    obs_c << 10, 11, 12;
    observe(c,obs_c);
    
    std::cout << "a = " << a.alpha << std::endl;
    std::cout << "b = " << b.alpha << std::endl;
    std::cout << "c = " << c.alpha << std::endl;
    
    return EXIT_SUCCESS;
    
} // main function