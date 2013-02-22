/**
 * @file vpi.h
 * Provides functions for calculating the Value of Perfect Information (VPI).
 */
#ifndef DECBRL_VPI_H
#define DECBRL_VPI_H

#include <cmath>
#include <cassert>
#include <limits>
#include <boost/utility/enable_if.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include "util.h"
#include "NonCentralT.h"
#include "NormalGamma.h"

/**
 * Namespace all public functions and types defined in the DecBRL library.
 */
namespace dec_brl
{
   /**
    * Calculates Teacy et al's Truncation Bias Function, as defined in http://eprints.soton.ac.uk/273201/.
    * For an input parameter, \c x, and Normal-Gamma distribution, \c dist,
    * with hyperparameters
    * \f$\rho = \langle \alpha, \beta, \lambda, m \rangle \f$,
    * the truncation bias function is defined as
    * \f[
    * \mathcal{B}_{\rho}(x) =
    * \frac{\Gamma\left(\alpha - \frac{1}{2} \right) \sqrt{\beta}
    * \left( 1+ \frac{\lambda(x-m)^2}{2\beta} \right)^{-\alpha+\frac{1}{2}}}
    * {\Gamma(\alpha)\Gamma(1/2)\sqrt{2\lambda}}
    * \f]
    * Strictly speaking, this function is not defined for \f$\alpha<0.5\f$.
    * However, in such cases, it is normally sufficient to return a very large
    * value, rather than generate an error condition. In such cases, we
    * therefore return dec_brl::Limits::infinity.
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * @param dist the Normal-Gamma parameter distribution with hyperparameters
    * \f$\rho = \langle \alpha, \beta, \lambda, m \rangle \f$.
    * @param x input for return value \f$\mathcal{B}_{\rho}(x)\f$
    * @returns \f$\mathcal{B}_{\rho}(x)\f$ or the largest possible value if
    * dist.alpha < 0.5.
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Policy> RealType truncationBias 
   (
    const dist::NormalGamma_Tmpl<RealType,Policy>& dist,
    const RealType x
   )
   {
      using namespace boost::math;

      //************************************************************************
      // When alpha<0.5, the gain is infinite.
      //************************************************************************
      if(dist.alpha<0.5)
      {
         return Limits<RealType>::infinity();
      }

      Policy policy; // Boost.Math policy used for calculations.

      //************************************************************************
      // Store parts in local variables for convenience
      //************************************************************************
      const RealType alpha = dist.alpha;
      const RealType beta = dist.beta;
      const RealType lambda = dist.lambda;
      const RealType m = dist.m;

      //************************************************************************
      // Calculate the in brackets part.
      // Notice we use log1p which is a more numerically stable way to compute
      // log(1+fraction)
      //************************************************************************
      RealType logfraction  = std::log(lambda)+2.0*std::log(std::abs(x-m));
               logfraction -= std::log(2.0)+std::log(beta);

      RealType logbrackets = log1p(std::exp(logfraction));

      //************************************************************************
      // Calculate log result for numerical stability.
      //************************************************************************
      RealType lnResult  = lgamma<RealType,Policy>(alpha-0.5, policy);
               lnResult += 0.5*(std::log(beta)-std::log(2.0)-std::log(lambda));
               lnResult += (0.5-alpha)*logbrackets;
               lnResult -= lgamma<RealType,Policy>(alpha, policy);
               lnResult -= lgamma<RealType,Policy>(0.5, policy);

      //************************************************************************
      // Return result
      //************************************************************************
      return std::exp(lnResult);

   } // truncationBias

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
   template<class RealType, class Rand> RealType sampledVPI
   (
    bool isBestAction,
    const RealType bestVal1,
    const RealType bestVal2,
    Rand& valDist,
    const int noSamples=50
   )
   {
      RealType expGain=0.0; // stores result

      //************************************************************************
      // Calculate expected gain (VPI) for 1st best action:
      // If 2nd best action turns out to be best, the gain is the difference.
      // Otherwise it is zero.
      //************************************************************************
      if(isBestAction)
      {
         for(int k=0; k<noSamples; ++k)
         {
            RealType sampledValue = valDist();
            if(sampledValue<bestVal2) expGain += bestVal2-sampledValue;
         }
      }
      //************************************************************************
      // Calculate expected gain (VPI) for any other action
      // If this action turns out to be best, the gain is the difference with
      // the 1st best action. Otherwise it is zero.
      //************************************************************************
      else
      {
         for(int k=0; k<noSamples; ++k)
         {
            RealType sampledValue = valDist();
            if(sampledValue>bestVal1) expGain += sampledValue-bestVal1;
         }
      }

      //************************************************************************
      // Average by dividing by number of samples, and return the result
      //************************************************************************
      expGain /= noSamples;
      assert(expGain>=0);
      return expGain;

   } // vpi function

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
    * @param[in] dist the parameter distribution for the action for which
    * VPI is to be calculated.
    * @return an estimate of the VPI for an action whoses value is distributed
    * according to \c valDist.
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Policy> RealType exactVPI
   (
    bool isBestAction,
    const RealType bestVal1,
    const RealType bestVal2,
    const dist::NormalGamma_Tmpl<RealType,Policy>& dist
   )
   {
      using namespace boost::math;

      // truncation bias undefined for alpha<0.5
      if(dist.alpha<0.5)
      {
         return Limits<RealType>::infinity();
      }
      
      dist::NonCentralT_Tmpl<RealType,Policy> marginal = meanMarginal(dist);
      if(isBestAction)
      {
         RealType result = truncationBias(dist,bestVal2);
         result += (bestVal2-dist.m) * cdf(marginal,bestVal2);
         assert(result>=0);
         return result;
      }
      else
      {
         RealType result = truncationBias(dist,bestVal1);
         result += (dist.m-bestVal1) * cdf(complement(marginal,bestVal1));
         assert(result>=0);
         return result;
      }

   } // vpi function

} // namespace dec_brl

#endif  // DECBRL_VPI_H
