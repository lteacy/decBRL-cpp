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
    using namespace dec_brl;
    
    //**************************************************************************
    // Register some test variables with the maxsum library
    //**************************************************************************
    std::cout << "Registering maxsum variables..." << std::endl;
    Eigen::Vector3i vars, sizes;
    vars << 1,2,3;
    sizes << 2,3,4;
    
    maxsum::registerVariables(vars.begin(), vars.end(),
                              sizes.begin(), sizes.end());
    
    //**************************************************************************
    // Create belief distribution for transition probability over variables 1
    // and 2, given 1,2 and 3.
    //**************************************************************************
    std::cout << "Constructing beliefs..." << std::endl;
    dec_brl::TransBelief beliefs(vars,vars.head(2));
    
    std::cout << "Beliefs:" << std::endl;
    std::cout << beliefs << std::endl;
    
    //**************************************************************************
    // Check that the reported domain sizes are correct.
    // Should be the product of the domain sizes for corresponding variables
    //**************************************************************************
    const int CORRECT_COND_SIZE = sizes[0]*sizes[1]*sizes[2];
    const int CORRECT_DOMAIN_SIZE = sizes[0]*sizes[1];
    if(beliefs.condSize()!=CORRECT_COND_SIZE)
    {
        std::cout << "Incorrect conditional size: " << beliefs.condSize()
        << " should be " << (2*3*4) << std::endl;
        return EXIT_FAILURE;
    }
    
    if(beliefs.domainSize() != CORRECT_DOMAIN_SIZE)
    {
        std::cout << "Incorrect domain size: " << beliefs.domainSize()
        << " should be " << (2*3) << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Domain sizes: OK" << std::endl;
    
    //**************************************************************************
    //  Ensure that is equal to prior
    //**************************************************************************
    if(!beliefs.getAlpha().isApproxToConstant(TransBelief::DEFAULT_ALPHA))
    {
        std::cout << "Hyperparameters incorrect - should all be "
        << TransBelief::DEFAULT_ALPHA << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Prior OK" << std::endl;
    
    //**************************************************************************
    //  Try to set hyperparameters to some constant scalar
    //**************************************************************************
    const double NEW_PRIOR = 2.5;
    beliefs.setAlpha(NEW_PRIOR);
    
    std::cout << "Beliefs:" << std::endl;
    std::cout << beliefs << std::endl;
    
    if(!beliefs.getAlpha().isApproxToConstant(NEW_PRIOR))
    {
        std::cout << "Hyperparameters incorrect after set - should all be "
        << NEW_PRIOR << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Prior reset OK" << std::endl;
    
    //**************************************************************************
    //  Try to observe a few different values based on linear indices
    //**************************************************************************
    int condInd = beliefs.condSize() / 2; // select middle value to update
    int domainInd = beliefs.domainSize() / 2;
    beliefs.observeByInd(condInd, domainInd);
    
    Eigen::Matrix<double, 2*3, 2*3*4> correctValue;
    correctValue.setConstant(NEW_PRIOR);
    correctValue(domainInd,condInd) += 1;
    
    if(beliefs.getAlpha() != correctValue)
    {
        std::cout << "Incorrect posterior hyperparameters" << std::endl;
        std::cout << beliefs.getAlpha() << std::endl;
        std::cout << "BUT SHOULD BE" << std::endl;
        std::cout << correctValue << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Linear Observation OK" << std::endl;
    
    //**************************************************************************
    // Try to observe a few different values based on sub indices
    //**************************************************************************
    Eigen::Vector3i condVals;
    Eigen::Vector2i domainVals;
    condVals << 1, 0, 2;
    domainVals << 0, 1;
    
    condInd = maxsum::sub2ind(sizes.begin(), sizes.end(),
                              condVals.begin(), condVals.end());
    
    domainInd = maxsum::sub2ind(sizes.begin(), sizes.end()-1,
                                domainVals.begin(), domainVals.end());
    
    correctValue(domainInd,condInd) += 1;
    beliefs.observeByVec(condVals.begin(), condVals.end(), domainVals.begin(), domainVals.end());
    
    if(beliefs.getAlpha() != correctValue)
    {
        std::cout << "Incorrect posterior hyperparameters" << std::endl;
        std::cout << beliefs.getAlpha() << std::endl;
        std::cout << "BUT SHOULD BE" << std::endl;
        std::cout << correctValue << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Vector Observation OK" << std::endl;
    
    //**************************************************************************
    //  Try to observe a few different values based on mapped indices
    //**************************************************************************
    correctValue(domainInd,condInd) += 1; // observe same again
    beliefs.observeByMap(condInd, domainInd);
    
    condVals << 1, 1, 0;
    domainVals << 1, 2;
    
    condInd = maxsum::sub2ind(sizes.begin(), sizes.end(),
                              condVals.begin(), condVals.end());
    
    domainInd = maxsum::sub2ind(sizes.begin(), sizes.end()-1,
                                domainVals.begin(), domainVals.end());
    
    correctValue(domainInd,condInd) += 1; // observe something else
    beliefs.observeByMap(condInd, domainInd);
    
    if(beliefs.getAlpha() != correctValue)
    {
        std::cout << "Incorrect posterior hyperparameters" << std::endl;
        std::cout << beliefs.getAlpha() << std::endl;
        std::cout << "BUT SHOULD BE" << std::endl;
        std::cout << correctValue << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Map Observation OK" << std::endl;
    
    //**************************************************************************
    // Check that the mean function works
    //**************************************************************************
    
    //**************************************************************************
    //  Check that the random generator works
    //**************************************************************************
    
    //**************************************************************************
    // If we get this far, everything passed.
    //**************************************************************************
    std::cout << "All TransBelief tests passed" << std::endl;
    return EXIT_SUCCESS;
    
} // function main