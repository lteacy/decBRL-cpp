/**
 * @file vecHarness.cpp
 * Test harness for vectorised beliefs using maxsum::DiscreteFunction objets.
 */

#include <limits>
#include <exception>
#include <cfloat>
#include "vpi.h"
#include "NormalGamma.h"
#include "DiscreteFunction.h"
#include "DomainIterator.h"
#include <iostream>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

// private module namespace
namespace
{
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
      maxsum::ValIndex index = vecParams.m.domainSize()/2;

      std::cout << "selected index for dual update: " << index << std::endl;

      //************************************************************************
      // Repeatedly update the normal-gamma distribution with observations from
      // the standard normal, and validate VPI after each observation.
      //************************************************************************
      for(int sampleSize=0; sampleSize<60; ++sampleSize)
      {
         std::cout << "******************************************" << std::endl;
         std::cout << "ITERATION: " << sampleSize << std::endl;
         std::cout << "******************************************" << std::endl;

         //*********************************************************************
         // Sample two random variates from a standard normal distribution
         //*********************************************************************
         std::cout << "Generating new observations" << std::endl;
         double obs1 = normrnd();
         double obs2 = normrnd();

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
         observe(vecParams,index,obs2);

         //*********************************************************************
         // Check hyperparameters for consistency
         //*********************************************************************
         std::cout << "Check hyperparameter consistency" << std::endl;
         for(int k=0; k<vecParams.m.domainSize(); ++k)
         {
            NormalGamma* pCorrect = &scalarParams1;
            if(k==index)
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

   }
   catch(std::exception& e)
   {
      std::cout << "Caught error: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   
   //***************************************************************************
   // If we get this far, everything is ok.
   //***************************************************************************
   std::cout << "Test harness not yet implemented." << std::endl;
   return EXIT_FAILURE;

} // function main

