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
   public:

      /**
       * Default value for alpha hyperparameter.
       */
      static const RealType DEFAULT_ALPHA;

      /**
       * Default value for beta hyperparameter.
       */
      static const RealType DEFAULT_BETA;

      /**
       * Default value for m hyperparameter
       */
      static const RealType DEFAULT_M;

      /**
       * Default value for lamba hyperparameter.
       */
      static const RealType DEFAULT_LAMBDA;

   private:

      /**
       * The alpha hyperparmeter.
       */
      RealType alpha_i;

      /**
       * The beta hyperparameter.
       */
      RealType beta_i;

      /**
       * The lambda hyperparameter.
       */
      RealType lambda_i;

      /**
       * The m hyperparmeter.
       */
      RealType m_i;

   public:

      /**
       * Type used to represent real values in this object.
       */
      typedef RealType value_type;

      /**
       * Boost.Math calculation policy used by this object.
       */
      typedef Policy policy_type;

      /**
       * Constructs a new distribution with specified parameters.
       * @param[in] a value for alpha hyperparameter.
       * @param[in] b value for beta hyperparameter.
       * @param[in] l value for lambda hyperparmeter.
       * @param[in] m value for m hyperparameter.
       */
      NormalGamma_Tmpl
      (
       RealType a=DEFAULT_ALPHA,
       RealType b=DEFAULT_BETA,
       RealType l=DEFAULT_LAMBDA,
       RealType m=DEFAULT_M
      )
      : alpha_i(a), beta_i(b), lambda_i(l), m_i(m) {}

      /**
       * Accessor method for alpha hyperparameter.
       */
      RealType alpha() const { return alpha_i; }

      /**
       * Accessor method for beta hyperparameter.
       */
      RealType beta() const { return beta_i; }

      /**
       * Accessor method for the lambda hyperparameter.
       */
      RealType lambda() const { return lambda_i; }

      /**
       * Accessor method for the m hyperparameter.
       */
      RealType m() const { return m_i; }

      /**
       * Accessor method for alpha hyperparameter.
       */
      void alpha(RealType a) { alpha_i=a; }

      /**
       * Accessor method for beta hyperparameter.
       */
      void beta(RealType b) { beta_i=b; }

      /**
       * Accessor method for the lambda hyperparameter.
       */
      void lambda(RealType l) { lambda_i=l; }

      /**
       * Accessor method for the m hyperparameter.
       */
      void m(RealType m) { m_i=m; }

   }; // class NormalGamma_Tmpl distribution

   /**
    * Default value for alpha hyperparameter.
    */
   template<class RealType, class Policy> const RealType
      NormalGamma_Tmpl<RealType,Policy>::DEFAULT_ALPHA = 0.00000001;

   /**
    * Default value for beta hyperparameter.
    */
   template<class RealType, class Policy> const RealType
      NormalGamma_Tmpl<RealType,Policy>::DEFAULT_BETA
         = 0.00000000000000001;

   /**
    * Default value for m hyperparameter
    */
   template<class RealType, class Policy> const RealType
      NormalGamma_Tmpl<RealType,Policy>::DEFAULT_M = 0.0;

   /**
    * Default value for lamba hyperparameter.
    */
   template<class RealType, class Policy> const RealType
      NormalGamma_Tmpl<RealType,Policy>::DEFAULT_LAMBDA
         = 0.00000000000000000000001;

   /**
    * Convenience typedef for distributions that use the
    * default template parameters.
    */
   typedef NormalGamma_Tmpl<> NormalGamma;

   /**
    * Constructs a new NonCentralT distribution representing the marginal
    * distribution of the mean, for an unknown Gaussian distribution.
    * @param[in] dist the parameter distribution of the unknown Gaussian.
    * @returns the marginal distribution of the mean.
    */
   template<class RealType, class Policy> NonCentralT_Tmpl<RealType,Policy>
      meanMarginal(const NormalGamma_Tmpl<RealType,Policy>& dist)
   {
      RealType df = 2*dist.alpha();
      RealType loc = dist.m();
      RealType scale = std::sqrt(dist.beta()/dist.lambda()/dist.alpha());
      return NonCentralT_Tmpl<RealType,Policy>(df,loc,scale);
   }

   /**
    * Updates a parameter distribution given an observation drawn from its
    * target distribution. The update equations here are based on:
    * Section 7.6 of M. DeGroot and M. Schervish. Probability & Statistics.
    * Addison-Wesley, 3rd edition, 2002. Precise update equations used here
    * are
    * \f{eqnarray*}{
    * \alpha' &=& \alpha + \frac{1}{2} \\
    * \beta' &=& \beta + \frac{\lambda(x-m)^2}{2(\lambda+1)} \\
    * \lambda' &=& \lambda + 1 \\
    * m' &=& \frac{\lambda m + x}{\lambda+1}
    * \f}
    * @param paramDist the parameter distribution to update.
    * @param[in] x an observation drawn from the target distribution.
    */
   template<class RealType, class Policy> void observe
      (NormalGamma_Tmpl<RealType,Policy>& paramDist, RealType x)
   {
      RealType oldAlpha = paramDist.alpha();
      RealType oldBeta = paramDist.beta();
      RealType oldLambda = paramDist.lambda();
      RealType oldM = paramDist.m();

      RealType newAlpha = oldAlpha + 0.5;
      RealType newLambda = oldLambda + 1;
      RealType newM = (oldLambda*oldM + x) / newLambda;
      RealType newBeta = oldBeta + oldLambda*(x-oldM)*(x-oldM)/2.0/newLambda;

      paramDist.alpha(newAlpha);
      paramDist.beta(newBeta);
      paramDist.lambda(newLambda);
      paramDist.m(newM);

   } // function observe

} // namespace dist
} // namespace dec_brl

#endif // DECBRL_NORMALGAMMA_H
