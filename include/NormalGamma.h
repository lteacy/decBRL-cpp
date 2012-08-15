/**
 * @file NormalGamma.h
 * Defines a class representing the Normal-Gamma class of conjugate parameter
 * distributions for Gaussian distributions with unknown mean and variance.
 */
#ifndef DECBRL_NORMALGAMMA_H
#define DECBRL_NORMALGAMMA_H

#include "NonCentralT.h"

/**
 * Namespace all public functions and types defined in the DecBRL library.
 */
namespace dec_brl {

/**
 * Namespace for classes that represent probability distributions commonly
 * used in Bayesian Reinforcement Learning and Bayesian Analysis.
 * These classes are designed to work with distribution functions and classes
 * in the Boost.Math library.
 * @see http://www.boost.org
 */
namespace dist {

   /**
    * Class Representing Normal-Gamma distributions.
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * In Bayesian Analysis, this class of distribution is conjugate
    * for Normal distributions with unknown mean and variance.
    */
   template<class RealType=double, class Policy=boost::math::policies::policy<> >
   class NormalGamma_Tmpl
   {
   private:
   public:

      /**
       * Type used to represent real values in this object.
       */
      typedef RealType value_type;

      /**
       * Boost.Math calculation policy used by this object.
       */
      typedef Policy policy_type;

   }; // class NormalGamma_Tmpl distribution

   /**
    * Convenience typedef for distributions that use the
    * default template parameters.
    */
   typedef NormalGamma_Tmpl<> NormalGamma;

   /**
    * Constructs a new NonCentralT distribution representing the marginal
    * distribution of the mean, for an unknown Gaussian distribution.
    * @param[in] the parameter distribution of the unknown Gaussian.
    * @returns the marginal distribution of the mean.
    */
   template<class RealType, class Policy> NonCentralT_Tmpl<RealType,Policy>
      meanMarginal(const NormalGamma_Tmpl<RealType,Policy>& paramDist)
   {
      return NonCentralT_Tmpl<RealType,Policy>(1);
   }

   /**
    * Updates a parameter distribution given an observation drawn from its
    * target distribution.
    * @param paramDist the parameter distribution to update.
    * @param[in] x an observation drawn from the target distribution.
    */
   template<class RealType, class Policy> void observe
      (NormalGamma_Tmpl<RealType,Policy>& paramDist, RealType x)
   {

   } // function observe

} // namespace dist
} // namespace dec_brl

#endif // DECBRL_NORMALGAMMA_H
