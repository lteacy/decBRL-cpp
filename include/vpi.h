/**
 * @file vpi.h
 * Provides functions for calculating the Value of Perfect Information (VPI).
 */
#ifndef DECBRL_VPI_H
#define DECBRL_VPI_H

#include <boost/math/special_functions/gamma.hpp>
#include "NonCentralT.h"
#include "NormalGamma.h"
#include <cmath>

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
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * @param dist the Normal-Gamma parameter distribution with hyperparameters
    * \f$\rho = \langle \alpha, \beta, \lambda, m \rangle \f$.
    * @param x input for return value \f$\mathcal{B}_{\rho}(x)\f$
    * @returns \f$\mathcal{B}_{\rho}(x)\f$
    * @see http://eprints.soton.ac.uk/273201/
    */
   template<class RealType, class Policy> RealType truncationBias 
   (
    const dist::NormalGamma_Tmpl<RealType,Policy>& dist,
    const RealType x
   )
   {
      using namespace boost::math;

      Policy policy; // Boost.Math policy used for calculations.

      const RealType alpha = dist.alpha();
      const RealType beta = dist.beta();
      const RealType lambda = dist.lambda();
      const RealType m = dist.m();
      const RealType inbrackets = 1 + lambda*(x-m)*(x-m)/2.0/beta;

      RealType result  = 1/tgamma<RealType,Policy>(0.5, policy);
               result *= tgamma_ratio(alpha-0.5, alpha, policy);
               result *= std::sqrt(alpha/2.0/lambda);
               result *= std::pow(inbrackets,0.5-alpha);

      return result;
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
    * @param[in] dist the parameter distribution for the action for which
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
    const dist::NormalGamma_Tmpl<RealType,Policy>& dist
   )
   {
      using namespace boost::math;
      dist::NonCentralT_Tmpl<RealType,Policy> marginal = meanMarginal(dist);
      if(isBestAction)
      {
         RealType result = truncationBias(dist,bestVal2);
         result += (bestVal2-dist.m()) * cdf(marginal,bestVal2);
         return result;
      }
      else
      {
         RealType result = truncationBias(dist,bestVal1);
         result += (dist.m()-bestVal1) * cdf(complement(marginal,bestVal1));
         return result;
      }

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
