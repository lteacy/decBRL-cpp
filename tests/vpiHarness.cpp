/**
 * @file vpiHarness.cpp
 * Test harness for functions in vpi.h and related code.
 */

#include <limits>
#include <exception>
#include <cfloat>
#include "dec_brl/vpi.h"
#include "dec_brl/NormalGamma.h"
#include <iostream>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "DiscreteFunction.h"

// private module namespace
namespace
{
using namespace dec_brl::dist;

/**
 * Number of samples used for testing VPI Monte Carlo estimation.
 */
const int MC_SAMPLE_SIZE_M = 2500;

/**
 * The standard error for Monte Carlo estimates, expressed as the number of
 * standard deviations from the sample mean.
 * Value is defined as <CODE>1.96/sqrt(MC_SAMPLE_SIZE)</CODE>, where 
 * 1.96 is the 0.95 quantile for the standard normal distribution.
 */
const double STANDARD_ERROR_COEFFICIENT = 4.0 / 50.0;

/**
 * Check that two doubles are equal within margin of error.
 */
bool equalWithinTol_m(double v1, double v2, double tol=0.0001)
{
   return tol > (std::abs(v1-v2)/std::min(v1,v2));
}

/**
 * For a given NormalGamma parameter distribtuion, returns true iff
 * VPI calculation is consistent and non-negative when performed
 * analytically, and by sampling.
 * @param[in] paramDist the parameter distribution for the unknown action value.
 * @param[in] vpi1 VPI calculated using one method (analytical or sampled)
 * @param[in] vpi2 VPI calculated by the other method (analytical or sampled).
 */
bool consistentVPI_m(const NormalGamma& paramDist, double vpi1, double vpi2)
{
   //***************************************************************************
   // Calculate expected sampling error for given sample size
   // This is based on the 0.95 quantile of the normal approximation to the
   // sample distribution.
   //***************************************************************************
   using namespace boost::math;
   NonCentralT meanMarginalDistribution = meanMarginal(paramDist);
   if(!meanMarginalDistribution.hasVariance())
   {
      std::cout << "\nSkipping consistency check due to infinite "
         "marginal variance." << std::endl;
      return true;
   }
   double stdevOfMean = standard_deviation(meanMarginalDistribution);
   double stdErr = stdevOfMean * STANDARD_ERROR_COEFFICIENT;

   //***************************************************************************
   // Check that both VPI values are equal within tolerance
   //***************************************************************************
   double diff = vpi1-vpi2;
   if( (diff > stdErr) || (diff < -stdErr) )
   {
      std::cout << "\nInconsistent VPI: vpi1=" << vpi1 << " vpi2=" << vpi2
         << " differ by " << diff << std::endl;
      return false;
   }

   //***************************************************************************
   // Check that both are positive values
   //***************************************************************************
   if( (0>vpi1) || (0>vpi2) )
   {
      std::cout << "\nNegative VPI: vpi1=" << vpi1 << " vpi2=" << vpi2
         << std::endl;
      return false;
   }

   return true;

} // function consistentVPI_m
   
/**
 * Test that VPI is consistent when calculated in different ways for a 
 * value function represented by a maxsum::DiscreteFunction object.
 */
bool testVecVPI()
{
   //***************************************************************************
   // Register some variables with the maxsum library for testing
   //***************************************************************************
   maxsum::registerVariable(1,2);
   maxsum::registerVariable(2,2);
   
   //***************************************************************************
   // Create a NormalGamma distribution for a full value function
   //***************************************************************************
   maxsum::DiscreteFunction zeroFunc;
   zeroFunc.expand(1);
   zeroFunc.expand(2);
   
   dec_brl::dist::NormalGamma_Tmpl<maxsum::DiscreteFunction> dist(zeroFunc,zeroFunc,
                                                         zeroFunc,zeroFunc);
   dist.alpha(0) = 1;
   dist.alpha(1) = 2;
   dist.alpha(2) = 3;
   dist.alpha(3) = 4;
   
   dist.beta(0) = 10;
   dist.beta(1) = 25;
   dist.beta(2) = 30;
   dist.beta(3) = 100;
   
   dist.lambda(0) = 1;
   dist.lambda(1) = 1;
   dist.lambda(2) = 4;
   dist.lambda(3) = 4;
   
   dist.m(0) = -2;
   dist.m(1) = 100;
   dist.m(2) = 10;
   dist.m(3) = 30;
   
   //***************************************************************************
   // Calculate VPI the vectorised way
   //***************************************************************************
   maxsum::DiscreteFunction vecResult;
   dec_brl::exactVPI(dist,vecResult);
   
   //***************************************************************************
   // Ensure result as correct domain size
   //***************************************************************************
   if(vecResult.domainSize()!=dist.m.domainSize())
   {
      std::cout << "Wrong size for VPI DiscreteFunction result." << std::endl;
      return false;
   }
   
   //***************************************************************************
   // Find first and second best values required for scalar results
   //***************************************************************************
   maxsum::ValIndex firstBestInd = dist.m.argmax();
   maxsum::ValIndex secondBestInd = dist.m.argmax2(firstBestInd);
   maxsum::ValType firstBestVal = dist.m(firstBestInd);
   maxsum::ValType secondBestVal = dist.m(secondBestInd);
   
   //***************************************************************************
   // Check for consistency against scalar results
   //***************************************************************************
   for(int k=0; k<dist.m.domainSize(); ++k)
   {
      //************************************************************************
      // Get distribution for just the current value
      //************************************************************************
      NormalGamma_Tmpl<maxsum::ValType> scalarDist(dist.alpha(k),
                                                   dist.beta(k),
                                                   dist.lambda(k),
                                                   dist.m(k));
      
      //************************************************************************
      // Calculate scalar VPI
      //************************************************************************
      bool isBestAction = (k==firstBestInd);
      maxsum::ValType scalarVPI = dec_brl::exactVPI(isBestAction,firstBestVal,
                                           secondBestVal,scalarDist);
      
      //************************************************************************
      // Check for consistency
      //************************************************************************
      if(!equalWithinTol_m(scalarVPI,vecResult(k)))
      {
         return false;
      }
      
   } // for
   
   //***************************************************************************
   // Test passed if we get this far
   //***************************************************************************
   return true;
   
} // testVecVPI

/**
 * Test VPI for given parameter distribution
 */
bool testVPI(const NormalGamma& paramDist)
{
   using namespace dec_brl;

   //***************************************************************************
   // Test VPI with increasing best value
   //***************************************************************************
   double prevBestVPI = -Limits<double>::infinity();
   double prevNotBestVPI = Limits<double>::infinity();
   for(double bestVal=-10; bestVal<10; bestVal+=0.25) 
   {
      std::cout << "\nBestval: " << bestVal << ' ' << std::flush;
      //************************************************************************
      // Calculate VPI analytically (for both best action and other action)
      //************************************************************************
      std::cout << "." << std::flush;
      double exactBestVPI = exactVPI(true, bestVal, bestVal, paramDist);
      double exactNotBestVPI = exactVPI(false, bestVal, bestVal, paramDist);

      //************************************************************************
      // Calculate VPI by sampling (for both best action and other action)
      //************************************************************************
      std::cout << "." << std::flush;
      NonCentralT marginal = meanMarginal(paramDist);
      boost::mt19937 rng;
      boost::variate_generator<boost::mt19937&, NonCentralT>
         generator(rng,marginal);
      std::cout << "." << std::flush;
      double sampledBestVPI = sampledVPI(true, bestVal, bestVal, generator,
            MC_SAMPLE_SIZE_M);
      double sampledNotBestVPI = sampledVPI(false, bestVal, bestVal, generator,
            MC_SAMPLE_SIZE_M);

      //************************************************************************
      // Check that sampled and exact calculations are consistent within
      // tolerance.
      //************************************************************************
      std::cout << "." << std::flush;
      if(!consistentVPI_m(paramDist,sampledBestVPI,exactBestVPI))
      {
         std::cout << "\nSampled best VPI is inconsistent with exact best."
            << std::endl;
         return false;
      }

      if(!consistentVPI_m(paramDist,sampledNotBestVPI,exactNotBestVPI))
      {
         std::cout << "\nSampled not best VPI is inconsistent with exact not"
            " best." << std::endl;
         return false;
      }

      //************************************************************************
      // Check that gain increases or decreases correctly depending on 
      // best (or 2nd best) value.
      //************************************************************************
      std::cout << "." << std::flush;
      if(prevBestVPI > exactBestVPI)
      {
         std::cout << "\nVPI for best action should be an increasing function "
            " of the 2nd best action value." << std::endl;
         return false;
      }

      if(prevNotBestVPI < exactNotBestVPI)
      {
         std::cout << "\nVPI for non best actions should be a decreasing "
            "function of the 1st best action value: " <<
            prevNotBestVPI << " < " << exactNotBestVPI << std::endl;
         return false;
      }

      //************************************************************************
      // Store the exact results for later comparision.
      //************************************************************************
      prevBestVPI = exactBestVPI;
      prevNotBestVPI = exactNotBestVPI;

   } // for loop

   //***************************************************************************
   // If we get this far, everything is ok, so we return true.
   //***************************************************************************
   return true;

} // function testVPI

} // private module namespace

/**
 * Main function for harness - executes all tests.
 */
int main()
{
   try
   {
      using namespace dec_brl;
      using namespace dec_brl::dist;
      std::cout << "Hello World!" << std::endl;
      
      //************************************************************************
      // Determine if this platform has its own representation of infinity,
      // and make a note in the log
      //************************************************************************
      if(std::numeric_limits<double>::has_infinity)
      {
         std::cout << "Platform has infinity" << std::endl;
      }
      else
      {
         std::cout << "Platform has no infinity, using max instead"
            << std::endl;
      }

      //************************************************************************
      // Construct a random number generator for a standard normal distribution.
      //************************************************************************
      boost::mt19937 rnd;  // uniform random number generator
      boost::normal_distribution<> normal; // normal distribution
      boost::variate_generator<boost::mt19937&,boost::normal_distribution<> >
         normrnd(rnd,normal); // generates normal distributed random variates.
      
      //************************************************************************
      //  Test DiscreteFunction version of VPI to make sure it is consistent
      //************************************************************************
      if(!testVecVPI())
      {
         std::cout << "VPI inconsistent for DiscreteFunction" << std::endl;
         return EXIT_FAILURE;
      }
      
      //************************************************************************
      // Construct a normal-gamma distribution with default hyperparameters.
      // Has to start with enough observations to make marginal variance
      // finite.
      //************************************************************************
      std::cout << "Initialising parameter distribution" << std::endl;
      NormalGamma paramDist;
      double observation = normrnd();
      observe(paramDist,observation);

      //************************************************************************
      // Repeatedly update the normal-gamma distribution with observations from
      // the standard normal, and validate VPI after each observation.
      //************************************************************************
      double prevBestVPI = Limits<double>::infinity();
      double prevNotBestVPI = Limits<double>::infinity();
      bool bestVPIChange=false;
      bool nonBestVPIChange=false;
      for(int sampleSize=0; sampleSize<60; ++sampleSize)
      {
         std::cout << "\n****************************************" << std::endl;
         std::cout << "ITERATION: " << sampleSize << std::endl;
         std::cout << "****************************************" << std::endl;
         //*********************************************************************
         // Update the parameter distribution with a new standard normal
         // observation. This should decrease the entropy of the parameter
         // distribution.
         //*********************************************************************
         std::cout << "Generating new observation" << std::endl;
         observation = normrnd();
         std::cout << "Observing new observation" << std::endl;
         observe(paramDist,observation);

         //*********************************************************************
         // Run batch of VPI tests given the current state of the parameter
         // distribution
         //*********************************************************************
         std::cout << "Performing main tests" << std::flush;
         if(!testVPI(paramDist))
         {
            std::cout << "Main VPI tests failed at " << sampleSize
               << " observations." << std::endl;
            return EXIT_FAILURE;
         }
   
         //*********************************************************************
         // Store the current VPI values for later comparsion.
         //*********************************************************************
         double bestVPI = exactVPI(true,1.0,-1.0,paramDist);
         double notBestVPI = exactVPI(false,1.0,-1.0,paramDist);

         if(prevBestVPI!=bestVPI)
         {
            prevBestVPI = bestVPI;
            bestVPIChange=true;
         }

         if(prevNotBestVPI!=notBestVPI)
         {
            prevNotBestVPI = notBestVPI;
            nonBestVPIChange=true;
         }
   
      } // for loop

      //************************************************************************
      // Ensure that some change in VPI happened at some point
      //************************************************************************
      if(!(bestVPIChange || nonBestVPIChange))
      {
         std::cout << "VPI never changed after any observations.\n";
         return EXIT_FAILURE;
      }
      else if(!bestVPIChange)
      {
         std::cout << "Best VPI never changed after any observations.\n";
         return EXIT_FAILURE;
      }
      else if(!nonBestVPIChange)
      {
         std::cout << "Non-Best VPI never changed after any observations.\n";
         return EXIT_FAILURE;
      }

   }
   catch(std::exception& e)
   {
      std::cout << "Caught error: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   
   //***************************************************************************
   // If we get this far, everything is ok.
   //***************************************************************************
   std::cout << "All VPI tests passed." << std::endl;
   return EXIT_SUCCESS;

} // function main

