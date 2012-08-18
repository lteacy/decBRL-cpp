/**
 * @file vpiHarness.cpp
 * Test harness for functions in vpi.h and related code.
 */

#include <exception>
#include <cfloat>
#include "vpi.h"
#include "NormalGamma.h"
#include <iostream>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

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
const double STANDARD_ERROR_COEFFICIENT = 1.96 / 50.0;

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
   NonCentralT meanMarginalDistribution = meanMarginal(paramDist);
   double stdevOfMean = standard_deviation(meanMarginalDistribution);
   double stdErr = stdevOfMean * STANDARD_ERROR_COEFFICIENT;

   //***************************************************************************
   // Check that both VPI values are equal within tolerance
   //***************************************************************************
   double diff = vpi1-vpi2;
   if( (diff > stdErr) || (diff < -stdErr) )
   {
      std::cout << "Inconsistent VPI: vpi1=" << vpi1 << " vpi2=" << vpi2
         << " differ by " << diff << std::endl;
      return false;
   }

   //***************************************************************************
   // Check that both are positive values
   //***************************************************************************
   if( (0>vpi1) || (0>vpi2) )
   {
      std::cout << "Negative VPI: vpi1=" << vpi1 << " vpi2=" << vpi2
         << std::endl;
      return false;
   }

   return true;

} // function consistentVPI_m

/**
 * Test VPI for given parameter distribution
 */
bool testVPI(const NormalGamma& paramDist)
{
   using namespace dec_brl;

   //***************************************************************************
   // Test VPI with increasing best value
   //***************************************************************************
   double prevBestVPI = -DBL_MAX;
   double prevNotBestVPI = DBL_MAX;
   for(double bestVal=-10; bestVal<10; bestVal+=0.25) 
   {
      //************************************************************************
      // Calculate VPI analytically (for both best action and other action)
      //************************************************************************
      double exactBestVPI = exactVPI(true, bestVal, bestVal, paramDist);
      double exactNotBestVPI = exactVPI(false, bestVal, bestVal, paramDist);

      //************************************************************************
      // Calculate VPI by sampling (for both best action and other action)
      //************************************************************************
      NonCentralT marginal = meanMarginal(paramDist);
      NonCentralTRandom generator(marginal);
      double sampledBestVPI = sampledVPI(true, bestVal, bestVal, generator);
      double sampledNotBestVPI = sampledVPI(false, bestVal, bestVal, generator);

      //************************************************************************
      // Check that sampled and exact calculations are consistent within
      // tolerance.
      //************************************************************************
      if(!consistentVPI_m(paramDist,sampledBestVPI,exactBestVPI))
      {
         std::cout << "Sampled best VPI is inconsistent with exact best."
            << std::endl;
         return false;
      }

      if(!consistentVPI_m(paramDist,sampledNotBestVPI,exactNotBestVPI))
      {
         std::cout << "Sampled not best VPI is inconsistent with exact not"
            " best." << std::endl;
         return false;
      }

      //************************************************************************
      // Check that gain increases or decreases correctly depending on 
      // best (or 2nd best) value.
      //************************************************************************
      if(prevBestVPI >= exactBestVPI)
      {
         std::cout << "VPI for best action should be an increasing function "
            " of the 2nd best action value." << std::endl;
         return false;
      }

      if(prevNotBestVPI <= exactNotBestVPI)
      {
         std::cout << "VPI for non best actions should be a decreasing "
            "function of the 1st best action value." << std::endl;
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
      // Construct a random number generator for a standard normal distribution.
      //************************************************************************
      boost::mt19937 rnd;  // uniform random number generator
      boost::normal_distribution<> normal; // normal distribution
      boost::variate_generator<boost::mt19937&,boost::normal_distribution<> >
         normrnd(rnd,normal); // generates normal distributed random variates.
   
      //************************************************************************
      // Construct a normal-gamma distribution with default hyperparameters.
      //************************************************************************
      NormalGamma paramDist;
   
      //************************************************************************
      // Repeatedly update the normal-gamma distribution with observations from
      // the standard normal, and validate VPI after each observation.
      //************************************************************************
      double prevBestVPI = DBL_MAX;
      double prevNotBestVPI = DBL_MAX;
      for(int sampleSize=0; sampleSize<60; ++sampleSize)
      {
         //*********************************************************************
         // Run batch of VPI tests given the current state of the parameter
         // distribution
         //*********************************************************************
         if(!testVPI(paramDist))
         {
            std::cout << "Main VPI tests failed at " << sampleSize
               << " observations." << std::endl;
            return EXIT_FAILURE;
         }
   
         //*********************************************************************
         // Calculate vpi exactly, and make sure it always gets smaller as the
         // number of observations increases.
         //*********************************************************************
         double bestVPI = exactVPI(true,1.0,-1.0,paramDist);
         double notBestVPI = exactVPI(false,1.0,-1.0,paramDist);
   
         if(bestVPI >= prevBestVPI)
         {
            std::cout << "Best VPI should always decrease with entropy.\n";
            return EXIT_FAILURE;
         }
   
         if(notBestVPI >= prevNotBestVPI)
         {
            std::cout << "Not-Best VPI should always decrease with entropy.\n";
            return EXIT_FAILURE;
         }
   
         //*********************************************************************
         // Store the current VPI values for later comparision.
         //*********************************************************************
         prevBestVPI = bestVPI;
         prevNotBestVPI = notBestVPI;
   
         //*********************************************************************
         // Update the parameter distribution with a new standard normal
         // observation. This should decrease the entropy of the parameter
         // distribution.
         //*********************************************************************
         double observation = normrnd();
         observe(paramDist,observation);
   
      } // for loop

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

