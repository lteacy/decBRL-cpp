/**
 * @file vecHarness.cpp
 * Test harness for vectorised beliefs using maxsum::DiscreteFunction objets.
 */

#include <limits>
#include <exception>
#include <cfloat>
#include "dec_brl/vpi.h"
#include "dec_brl/NormalGamma.h"
#include "DiscreteFunction.h"
#include "DomainIterator.h"
#include <iostream>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

// private module namespace
namespace
{

/**
 * Check that two doubles are equal within margin of error.
 */
bool equalWithinTol_m(double v1, double v2, double tol=0.0001)
{
   return tol > (std::abs(v1-v2)/std::min(v1,v2));
}

/**
 * Function used to check for consistency between values.
 */
bool isConsistent
(
 const dec_brl::dist::NormalGamma& dist1,
 const dec_brl::dist::NormalGamma& dist2
)
{
   bool  alphaOk = equalWithinTol_m(dist1.alpha,dist2.alpha);
   bool   betaOk = equalWithinTol_m(dist1.beta,dist2.beta);
   bool lambdaOk = equalWithinTol_m(dist1.lambda,dist2.lambda);
   bool      mOk = equalWithinTol_m(dist1.m,dist2.m);
   return alphaOk & betaOk & lambdaOk & mOk;
}

/**
 * Function used to check for consistency between scalar NormalGamma 
 * and a specified element of a NormalGamma distribution defined over a
 * DiscreteFunction.
 */
bool isConsistent
(
 const dec_brl::dist::NormalGamma& otherDist,
 const dec_brl::dist::NormalGamma& scalarDist,
 const dec_brl::dist::NormalGamma_Tmpl<maxsum::DiscreteFunction>& vecDist,
 const int index
)
{
   const dec_brl::dist::NormalGamma* pCorrect = 0;
   for(int k=0; k<vecDist.m.domainSize(); ++k)
   {
      if(index==k)
      {
         pCorrect=&scalarDist;
      }
      else
      {
         pCorrect=&otherDist;
      }

      bool  alphaOk = equalWithinTol_m(pCorrect->alpha,vecDist.alpha(k));
      bool   betaOk = equalWithinTol_m(pCorrect->beta,vecDist.beta(k));
      bool lambdaOk = equalWithinTol_m(pCorrect->lambda,vecDist.lambda(k));
      bool      mOk = equalWithinTol_m(pCorrect->m,vecDist.m(k));

      if(!(alphaOk & betaOk & lambdaOk & mOk))
      {
         return false;
      }
   }
   return true;

} // isConsistent

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
      // Register some variables for the creation of DiscreteFunction objects.
      //************************************************************************
      maxsum::registerVariable(1,5);
      maxsum::registerVariable(2,4);
      maxsum::registerVariable(3,2);
   
      //************************************************************************
      // Now we create a number parameter distributions:
      // scalarParams1 - will observe 1 observation per cycle
      // scalarParams2 - will observe 2 observations per cycle
      // vecParams - will equal scalarParams1 across its domain, except for
      // one element, which should equal scalarParams2.
      //************************************************************************
      std::cout << "Initialising parameter distributions" << std::endl;
      NormalGamma scalarParams1;
      NormalGamma scalarParams2;
      NormalGamma_Tmpl<maxsum::DiscreteFunction> vecParams;

      //************************************************************************
      // Try to expand domain of belief distribution
      //************************************************************************
      expand(vecParams,1);
      maxsum::VarID otherVars[] = {2,3};
      expand(vecParams,otherVars,otherVars+2);
      maxsum::ValIndex vecIndex = vecParams.m.domainSize()/2;

      std::cout << "selected index for dual update: " << vecIndex << std::endl;

      //************************************************************************
      // Repeatedly update the normal-gamma distribution with observations from
      // the standard normal, and validate VPI after each observation.
      //************************************************************************
      const int SAMPLE_SIZE = 60;
      std::vector<double> allObs1(SAMPLE_SIZE); // store obs for future ref
      std::vector<double> allObs2(SAMPLE_SIZE);
      double mu1 = 0.0; // sample mean for first obs only
      double mu2 = 0.0; // sample mean for all observations
      for(int it=0; it<SAMPLE_SIZE; ++it)
      {
         std::cout << "******************************************" << std::endl;
         std::cout << "ITERATION: " << it << std::endl;
         std::cout << "******************************************" << std::endl;

         //*********************************************************************
         // Sample two random variates from a standard normal distribution
         //*********************************************************************
         std::cout << "Generating new observations" << std::endl;
         double obs1 = normrnd();
         double obs2 = normrnd();
         allObs1[it] = obs1;
         allObs2[it] = obs2;
         mu1 += obs1;
         mu2 += obs2;

         //*********************************************************************
         // Update scalarParams1 using only the first observation, and 
         // scalarParams2 using both observations. vecParams is updated so that
         // it equals scalarParams in all places, except one, where it equals
         // scalarParams2.
         //*********************************************************************
         std::cout << "Observing new observations" << std::endl;
         observe(scalarParams1,obs1);
         observe(scalarParams2,obs1);
         observe(scalarParams2,obs2);
         observe(vecParams,obs1);
         observe(vecParams,vecIndex,obs2);

         //*********************************************************************
         // Check hyperparameters for consistency
         //*********************************************************************
         std::cout << "Check hyperparameter consistency" << std::endl;
         for(int k=0; k<vecParams.m.domainSize(); ++k)
         {
            NormalGamma* pCorrect = &scalarParams1;
            if(k==vecIndex)
            {
               pCorrect = &scalarParams2;
            }

            if(vecParams.alpha(k)!=pCorrect->alpha)
            {
               std::cout << "Incorrect at alpha location " << k << std::endl;
               return EXIT_FAILURE;
            }

            if(vecParams.beta(k)!=pCorrect->beta)
            {
               std::cout << "Incorrect beta at location " << k << std::endl;
               return EXIT_FAILURE;
            }

            if(vecParams.lambda(k)!=pCorrect->lambda)
            {
               std::cout << "Incorrect lambda at location " << k << std::endl;
               return EXIT_FAILURE;
            }

            if(vecParams.m(k)!=pCorrect->m)
            {
               std::cout << "Incorrect mean at location " << k << std::endl;
               return EXIT_FAILURE;
            }
         }

      } // for loop

      //************************************************************************
      // Calculate the sufficient statistics for both sets of samples
      //************************************************************************
      std::cout << "Calculating sufficient statistics" << std::endl;
      mu1 /= SAMPLE_SIZE;
      mu2 /= SAMPLE_SIZE;
      double s1 = 0.0;
      double s2 = 0.0;
      for(int it=0; it<SAMPLE_SIZE; ++it)
      {
         double x1 = allObs1[it]-mu1;
         double x2 = allObs2[it]-mu2;
         s1 += x1*x1;
         s2 += x2*x2;
      }

      //************************************************************************
      // Repeat the updates using sufficient statistics, and check for 
      // consistency.
      //************************************************************************
      std::cout << "Attempting Moment Update" << std::endl;
      NormalGamma scalarSingleUpdate1;
      NormalGamma scalarSingleUpdate2;
      NormalGamma_Tmpl<maxsum::DiscreteFunction> vecSingleUpdate;

      //************************************************************************
      // Try to expand domain of belief distribution
      //************************************************************************
      expand(vecSingleUpdate,1);
      expand(vecSingleUpdate,otherVars,otherVars+2);

      //************************************************************************
      // Do the update
      //************************************************************************
      observe(scalarSingleUpdate1,mu1,s1,SAMPLE_SIZE);
      observe(scalarSingleUpdate2,mu1,s1,SAMPLE_SIZE);
      observe(scalarSingleUpdate2,mu2,s2,SAMPLE_SIZE);
      observe(vecSingleUpdate,mu1,s1,SAMPLE_SIZE);
      observe(vecSingleUpdate,vecIndex,mu2,s2,SAMPLE_SIZE);

      //************************************************************************
      // Checking for consistency
      //************************************************************************
      std::cout << "Check for Consistency" << std::endl;
      if(!isConsistent(scalarParams1,scalarSingleUpdate1))
      {
         std::cout << "Params 1 - inconsistent moment update" << std::endl;
         return EXIT_FAILURE;
      }

      if(!isConsistent(scalarParams2,scalarSingleUpdate2))
      {
         std::cout << "Params 2 - inconsistent moment update" << std::endl;
         return EXIT_FAILURE;
      }

      if(isConsistent(scalarParams1,scalarSingleUpdate2))
      {
         std::cout << "Unexpected match between updates 1 and 2" << std::endl;
         return EXIT_FAILURE;
      }

      if(!isConsistent(scalarParams1,scalarParams2,vecSingleUpdate,vecIndex))
      {
         std::cout << "inconsistent vector moment update" << std::endl;
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
   std::cout << "Passed." << std::endl;
   return EXIT_SUCCESS;

} // function main

