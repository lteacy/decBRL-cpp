/**
 * @file TransBeliefHarness.cpp
 * Test Harness for dec_brl::TransBelief class
 * @author Luke Teacy
 */

#include <dec_brl/TransBelief.h>
#include "register.h"
#include <iostream>
#include <boost/random/mersenne_twister.hpp>
#include <cmath>

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
    beliefs.setAlpha(1);
    int condInd = beliefs.condSize() / 2; // select middle value to update
    int domainInd = beliefs.domainSize() / 2;
    beliefs.observeByInd(condInd, domainInd);
    
    Eigen::Matrix<double, 2*3, 2*3*4> correctValue;
    correctValue.setConstant(1);
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
    Eigen::Vector4i condMap;    // using vectors as maps
    Eigen::Vector3i domainMap;  // e.g. condMap[var] = val    
    condMap << 0, 1, 1, 0;
    domainMap << 0, 1, 2;
    
    condInd = maxsum::sub2ind(sizes.begin(), sizes.end(),
                              condMap.begin()+1, condMap.end());
    
    domainInd = maxsum::sub2ind(sizes.begin(), sizes.end()-1,
                                domainMap.begin()+1, domainMap.end());
    
    correctValue(domainInd,condInd) += 2; // observe something twice
    beliefs.observeByMap(condMap, domainMap);
    beliefs.observeByMap(condMap, domainMap);
    
    if(beliefs.getAlpha() != correctValue)
    {
        std::cout << "Incorrect posterior hyperparameters" << std::endl;
        std::cout << beliefs.getAlpha() << std::endl;
        std::cout << "BUT SHOULD BE" << std::endl;
        std::cout << correctValue << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Map Observation OK" << std::endl;
    
    std::cout << "Posterior:" << std::endl;
    std::cout << beliefs << std::endl;
    
    //**************************************************************************
    // Manually Calculate the correct expected CPT for comparision.
    //**************************************************************************
    std::cout << "CORRECT EXPECTED CPT CALCUATION" << std::endl;
    std::cout << correctValue << std::endl;
    std::cout << "DIVIDED BY" << std::endl;
    
    Eigen::RowVectorXd totals(correctValue.cols());
    totals = correctValue.colwise().sum();
    
    std::cout << totals << std::endl;
    std::cout << "EQUALS" << std::endl;
    
    correctValue.array().rowwise() /= totals.array();
    
    std::cout << correctValue << std::endl;
    
    std::cout << "MARGINALS SHOULD BE ONE" << std::endl;
    totals = correctValue.colwise().sum();

    std::cout << totals << std::endl;
    
    if(!totals.isApproxToConstant(1))
    {
        std::cout << "INCORRECT MARGINALS IN TEST HARNESS CODE" << std::endl;
        return EXIT_FAILURE;
    }
    
    //**************************************************************************
    // Check that the mean function works
    //**************************************************************************
    std::cout << "PERFORMING CHECK..." << std::endl;
    Eigen::MatrixXd expCPT;
    beliefs.getMean(expCPT);
    
    if( (correctValue.rows()!=expCPT.rows()) ||
        (correctValue.cols()!=expCPT.cols()) ||
        (!expCPT.isApprox(correctValue))     )
    {
        std::cout << "Incorrect expected CPT" << std::endl;
        std::cout << expCPT << std::endl;
        std::cout << "BUT SHOULD BE" << std::endl;
        std::cout << correctValue << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "expCPT is" << std::endl;
    std::cout << expCPT << std::endl << "ALL OK" << std::endl;
    
    //**************************************************************************
    //  Try to get the expCPT for a specific set of conditions
    //**************************************************************************
    std::cout << "Checking specific expected CPT..." << std::endl;
    Eigen::VectorXd secondCPT;
    beliefs.getMeanByMap(secondCPT,condMap);
    
    std::cout << secondCPT.transpose() << std::endl;
    std::cout << expCPT.col(condInd).transpose() << std::endl;
    
    if( (secondCPT.rows()!=expCPT.rows()) || (secondCPT.cols()!=1) ||
        (!secondCPT.isApprox(expCPT.col(condInd))) )
    {
        std::cout << "Inconsistent expectations" << std::endl;
        std::cout << "max difference: " << std::endl;
        Eigen::VectorXd diff = secondCPT-expCPT.col(condInd);
        std::cout << diff.transpose().array().abs() << std::endl;
        return EXIT_FAILURE;
    }
    
    //**************************************************************************
    //  Check that the random generator works
    //**************************************************************************
    std::cout << "Trying to sample some CPTs" << std::endl;
    const int NUM_SAMPLES = 100;
    boost::mt19937 randGenerator; // generates uniform random numbers
    Eigen::MatrixXd oldSample(expCPT);
    Eigen::MatrixXd newSample(expCPT);
    Eigen::MatrixXd sampleMean(expCPT.rows(),expCPT.cols());
    for(int k=0; k<NUM_SAMPLES; ++k)
    {
        //*********************************************************************
        //  Get a sampled CPT and update sum for future reference
        //*********************************************************************
        beliefs.sample(randGenerator,newSample);
        sampleMean += newSample;
        
        //*********************************************************************
        //  Check that its not the same as the previous sample, which would be
        //  odd.
        //*********************************************************************
        if(oldSample.isApprox(newSample))
        {
            std::cout << "this sample matches the last sample - VERY UNLIKELY";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
        
        //*********************************************************************
        //  Check that this is a valid CPT (each column sums to one)
        //*********************************************************************
        Eigen::RowVectorXd sampleTotals = newSample.colwise().sum();
        if(!sampleTotals.isApproxToConstant(1))
        {
            std::cout << "Sampled totals do not all sum to one:" << std::endl;
            std::cout << sampleTotals << std::endl;
            return EXIT_FAILURE;
        }
        
        //*********************************************************************
        //  Store this sample for comparsion in the next iteration
        //*********************************************************************
        newSample.swap(oldSample);
        
    } // for loop
    
    //**************************************************************************
    //  Ensure that sample mean is reasonably close to the actual mean
    //**************************************************************************
    std::cout << "Mean Sampled CPT" << std::endl;
    sampleMean /= NUM_SAMPLES;
    const double VAR_UPPER_BOUND = 0.02; // based on quick Diriclet calculation
    const double SAMPLE_STDERR = std::sqrt(VAR_UPPER_BOUND/NUM_SAMPLES);
    const double ACCEPT_PRECISION = SAMPLE_STDERR*8; // no more than 8 std dev
    std::cout << sampleMean << std::endl;
    
    if(!sampleMean.isApprox(expCPT,ACCEPT_PRECISION))
    {
        std::cout << "Some sample means differ by more than expected\n";
        std::cout << "DIFFERENCE: (threshold " << ACCEPT_PRECISION << ")\n";
        std::cout << (sampleMean-expCPT).array().abs() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Sample mean within acceptable precision: ";
    std::cout << ACCEPT_PRECISION << std::endl;
    
    //**************************************************************************
    // If we get this far, everything passed.
    //**************************************************************************
    std::cout << "All TransBelief tests passed" << std::endl;
    return EXIT_SUCCESS;
    
} // function main