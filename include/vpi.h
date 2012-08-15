/**
 * @file vpi.h
 * Provides functions for calculating the Value of Perfect Information (VPI).
 */
#ifndef DECBRL_VPI_H
#define DECBRL_VPI_H

#include <boost/math/special_functions/gamma.hpp>
#include "NonCentralT.h"
#include "NormalGamma.h"

/**
 * Namespace all public functions and types defined in the DecBRL library.
 */
namespace dec_brl
{
   /**
    * Calculates Teacy et al's Truncation Bias Function.
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * @param valDist the Normal-Gamma parameter distribution for an action's
    * value.
    * @param policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Policy> RealType trunctionBias 
   (
    const dist::NormalGamma_Tmpl<RealType,Policy>& valDist,
    const Policy& policy
   )
   {
      return 0;
   }

   /**
    * Calculates Teacy et al's Truncation Bias Function.
    * This version of the function uses the default Math.Boost policy to
    * calculate values.
    * @tparam RealType scalar type used for parameters and return values.
    * @param valDist the Normal-Gamma parameter distribution for an action's
    * value.
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Policy> RealType trunctionBias 
   (
    const dist::NormalGamma_Tmpl<RealType,Policy>& valDist
   )
   {
      return 0;
   }

   /**
    * Calculates the Value of Perfect Information (VPI) analytically,
    * given that an actions value distribution is a noncentral t 
    * distribution. In particular, the value distribution takes this form
    * in Bayesian Q-learning. This return value is calculated according
    * to Teacy et al's solution.
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * @param[in] isBestAction true iff calculating vpi for the 1st best action.
    * @param[in] bestVal1 the expected value of the 1st best action.
    * @param[in] bestVal2 the expected value of the 2nd best action.
    * @param[in] valDist the parameter distribution for the action for which
    * VPI is to be calculated.
    * @return an estimate of the VPI for an action whoses value is distributed
    * according to \c valDist.
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Policy> RealType vpi
   (
    bool isBestAction,
    const RealType bestVal1,
    const RealType bestVal2,
    const dist::NormalGamma_Tmpl<RealType,Policy>& valDist
   )
   {
      return 0;

   } // vpi function

   /**
    * Calculates the Value of Perfect Information (VPI) using monte carlo
    * sampling. This method is approximate, but works for any value
    * distribution from which random values can be sampled.
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Random Number Generator type for \c valDist parameter.
    * @param isBestAction true iff calculating vpi for the first best action.
    * @param bestVal1 the expected value of the 1st best action.
    * @param bestVal2 the expected value of the 2nd best action.
    * @param valDist functor or function pointer, which can be used to generate
    * samples from the value distribution.
    * @param noSamples the number of samples used to generate the estimate.
    * Larger values for \c noSamples produce more accurate estimates, but these
    * take longer to compute.
    * @return an estimate of the VPI for an action whoses value is distributed
    * according to \c valDist.
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Rand> RealType vpi
   (
    bool isBestAction,
    const RealType bestVal1,
    const RealType bestVal2,
    const Rand& valDist,
    const int noSamples=50
   )
   {
      return 0;

   } // vpi function

} // namespace dec_brl

#endif  // DECBRL_VPI_H
