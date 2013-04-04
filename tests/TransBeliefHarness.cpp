/**
 * @file TransBeliefHarness.cpp
 * Test Harness for dec_brl::TransBelief class
 * @author Luke Teacy
 */

#include <dec_brl/TransBelief.h>
#include "register.h"
#include <iostream>


/**
 * Main function.
 */
int main()
{
    //**************************************************************************
    // Register some test variables with the maxsum library
    //**************************************************************************
    std::cout << "Registering maxsum variables..." << std::endl;
    maxsum::registerVariable(1, 2);
    maxsum::registerVariable(2, 3);
    maxsum::registerVariable(3, 4);
    
    //**************************************************************************
    // Create belief distribution for transition probability over variables 1
    // and 2, given 1,2 and 3.
    //**************************************************************************
    std::cout << "Constructing beliefs..." << std::endl;
    Eigen::Vector3i vars;
    vars << 1,2,3;
    dec_brl::TransBelief beliefs(vars,vars.head(2));
    
    std::cout << "Beliefs:" << std::endl;
    std::cout << beliefs << std::endl;
    
    //**************************************************************************
    //  Ensure that is equal to prior
    //**************************************************************************
    
    //**************************************************************************
    //  Try to set hyperparameters to some constant scalar
    //**************************************************************************
    
    //**************************************************************************
    //  Try to observe a few different values based on linear indices
    //**************************************************************************
    
    //**************************************************************************
    // Try to observe a few different values based on sub indices
    //**************************************************************************
    
    //**************************************************************************
    //  Try to observe a few different values based on mapped indices
    //**************************************************************************
    
} // function main