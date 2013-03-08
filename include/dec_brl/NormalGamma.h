/**
 * @file NormalGamma.h
 * Defines a class representing the Normal-Gamma class of conjugate parameter
 * distributions for Gaussian distributions with unknown mean and variance.
 */
#ifndef DECBRL_NORMALGAMMA_H
#define DECBRL_NORMALGAMMA_H

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include "DiscreteFunction.h"
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

      /**
       * The alpha hyperparmeter.
       */
      RealType alpha;

      /**
       * The beta hyperparameter.
       */
      RealType beta;

      /**
       * The lambda hyperparameter.
       */
      RealType lambda;

      /**
       * The m hyperparmeter.
       */
      RealType m;

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
      : alpha(a), beta(b), lambda(l), m(m) {}

      /**
       * Copy constructor.
       */
      NormalGamma_Tmpl(const NormalGamma_Tmpl& x)
      : alpha(x.alpha), beta(x.beta), lambda(x.lambda),
        m(x.m) {}

      /**
       * Copy assignment.
       */
      NormalGamma_Tmpl& operator=(const NormalGamma_Tmpl& x)
      {
          alpha = x.alpha;
           beta = x.beta;
         lambda = x.lambda;
              m = x.m;

         return *this;     
      }

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
      RealType df = dist.alpha*2;
      RealType loc = dist.m;
      RealType scale = std::sqrt(dist.beta/dist.lambda/dist.alpha);
      return NonCentralT_Tmpl<RealType,Policy>(df,loc,scale);
   }

   /**
    * Updates a parameter distribution given sufficient statistics for a sample
    * drawn from the target distribution. The update equations here are based on
    * Section 7.6 of M. DeGroot and M. Schervish. Probability & Statistics.
    * Addison-Wesley, 3rd edition, 2002. Precise update equations used here
    * are
    * \f{eqnarray*}{
    * \alpha' &=& \alpha + \frac{n}{2} \\
    * \beta' &=& \beta + \frac{s^2_n}{2} + \frac{n \lambda (\bar{x}_n-m)^2}{2(\lambda+n)} \\
    * \lambda' &=& \lambda + n \\
    * m' &=& \frac{\lambda m + n \bar{x}_n}{\lambda+n}
    * \f}
    * @param paramDist the parameter distribution to update.
    * @param[in] sm sample mean for observations:
    * \f{displaymath}{
    * \bar{x}_n = \frac{1}{n} \sum^n_{i=1} x_i
    * \f}
    * @param[in] s2 sum of squared squares for observations:
    * \f{displaymath}{
    * s^2_n = \sum^n_{i=1} (x_i - \bar{x}_n)^2
    * \f}
    * @param[in] n the number of observations.
    */
   template<class RealType, class ValType, class Policy> 
   typename boost::disable_if
      < boost::is_base_of<maxsum::DiscreteFunction,RealType> >::type
   observe
   (
    NormalGamma_Tmpl<RealType,Policy>& paramDist,
    const ValType sm,
    const ValType s2,
    const int n
   )
   {
   } // observe

   /**
    * Updates a parameter distribution given sufficient statistics for a sample
    * drawn from the target distribution. The update equations here are based on
    * Section 7.6 of M. DeGroot and M. Schervish. Probability & Statistics.
    * Addison-Wesley, 3rd edition, 2002. Precise update equations used here
    * are
    * \f{eqnarray*}{
    * \alpha' &=& \alpha + \frac{n}{2} \\
    * \beta' &=& \beta + \frac{s^2_n}{2} + \frac{n \lambda (\bar{x}_n-m)^2}{2(\lambda+n)} \\
    * \lambda' &=& \lambda + n \\
    * m' &=& \frac{\lambda m + n \bar{x}_n}{\lambda+n}
    * \f}
    * @param paramDist the parameter distribution to update.
    * @param[in] sm sample mean for observations:
    * \f{displaymath}{
    * \bar{x}_n = \frac{1}{n} \sum^n_{i=1} x_i
    * \f}
    * @param[in] s2 sum of squared squares for observations:
    * \f{displaymath}{
    * s^2_n = \sum^n_{i=1} (x_i - \bar{x}_n)^2
    * \f}
    * @param[in] n the number of observations.
    */
   template<class RealType, class ValType, class Policy> 
   typename boost::enable_if
      < boost::is_base_of<maxsum::DiscreteFunction,RealType> >::type
   observe
   (
    NormalGamma_Tmpl<RealType,Policy>& paramDist,
    const ValType sm,
    const ValType s2,
    const int n
   )
   {
   } // observe

   /**
    * Updates a parameter distribution given sufficient statistics for a sample
    * drawn from the target distribution.
    * This version applies update only to a specific element of a
    * maxsum::DiscreteFunction value.
    * The update equations here are based on
    * Section 7.6 of M. DeGroot and M. Schervish. Probability & Statistics.
    * Addison-Wesley, 3rd edition, 2002. Precise update equations used here
    * are
    * \f{eqnarray*}{
    * \alpha' &=& \alpha + \frac{n}{2} \\
    * \beta' &=& \beta + \frac{s^2_n}{2} + \frac{n \lambda (\bar{x}_n-m)^2}{2(\lambda+n)} \\
    * \lambda' &=& \lambda + n \\
    * m' &=& \frac{\lambda m + n \bar{x}_n}{\lambda+n}
    * \f}
    * @param paramDist the parameter distribution to update.
    * @param index scalar index to specific element of DiscreteFunction value to
    * update.
    * @param[in] sm sample mean for observations:
    * \f{displaymath}{
    * \bar{x}_n = \frac{1}{n} \sum^n_{i=1} x_i
    * \f}
    * @param[in] s2 sum of squared squares for observations:
    * \f{displaymath}{
    * s^2_n = \sum^n_{i=1} (x_i - \bar{x}_n)^2
    * \f}
    * @param[in] n the number of observations.
    */
   template<class ValType, class Policy> void observe
   (
    NormalGamma_Tmpl<maxsum::DiscreteFunction,Policy>& paramDist,
    maxsum::ValIndex index,
    const ValType sm,
    const ValType s2,
    const int n
   )
   {
   } // observe

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
   template<class RealType, class ValType, class Policy> 
   typename boost::disable_if
      < boost::is_base_of<maxsum::DiscreteFunction,RealType> >::type
   observe(NormalGamma_Tmpl<RealType,Policy>& paramDist, ValType x)
   {
      RealType oldAlpha = paramDist.alpha;
      RealType oldBeta = paramDist.beta;
      RealType oldLambda = paramDist.lambda;
      RealType oldM = paramDist.m;

      RealType newAlpha = oldAlpha + 0.5;
      RealType newLambda = oldLambda + 1;
      RealType newM = (oldLambda*oldM + x) / newLambda;
      RealType newBeta = oldBeta + oldLambda*(oldM-x)*(oldM-x)/2.0/newLambda;

      paramDist.alpha = newAlpha;
      paramDist.beta = newBeta;
      paramDist.lambda = newLambda;
      paramDist.m = newM;

   } // function observe

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
   template<class RealType, class ValType, class Policy> 
   typename boost::enable_if
      < boost::is_base_of<maxsum::DiscreteFunction,RealType> >::type
   observe (NormalGamma_Tmpl<RealType,Policy>& paramDist, ValType x)
   {
      //************************************************************************
      // Convenience references
      //************************************************************************
      RealType& alpha = paramDist.alpha;
      RealType& beta = paramDist.beta;
      RealType& lambda = paramDist.lambda;
      RealType& m = paramDist.m;

      //************************************************************************
      // Update hyperparameters. Here we use assigment operators as much as 
      // possible, because these are more efficient for DiscreteFunction
      // objects. For primitive types, this shouldn't make much difference to
      // performance.
      //************************************************************************

      // lambda = lambda + 1 (but need to perserve old value for now)
      RealType newLambda(lambda);
      newLambda += 1;

      // a = a + 0.5
      alpha += 0.5;

      // tmp = lambda*(m-x)^2/(2*[lambda+1])  (used for beta update)
      RealType tmp(m);
      tmp -= x;          // result: m-x
      tmp *= tmp;        // result: (m-x)^2
      tmp *= lambda;     // result: lambda*(m-x)^2
      tmp /= 2.0;        // result: lambda*(m-x)^2/2
      tmp /= newLambda;  // result: lambda*(m-x)^2/(2*[lambda+1])

      // b = b + lambda*(m-x)^2/(2*[lambda+1])
      beta += tmp;

      // m = (m*lambda+x) / (lambda + 1)
      m *= lambda;
      m += x;
      m /= newLambda;

      // apply lambda update by swapping pointers (more efficient)
      lambda.swap(newLambda);

   } // function observe

   /**
    * Updates a parameter distribution given an observation drawn from its
    * target distribution. This version applies the update only to a 
    * specific element of a DiscreteFunction's domain.
    * The update equations here are based on:
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
    * @param[in] index the index of the parameter distribution to update.
    */
   template<class ValType, class Policy> 
   void observe
   (
    NormalGamma_Tmpl<maxsum::DiscreteFunction,Policy>& paramDist,
    maxsum::ValIndex index,
    ValType x
   )
   {
      maxsum::ValType oldAlpha = paramDist.alpha(index);
      maxsum::ValType oldBeta = paramDist.beta(index);
      maxsum::ValType oldLambda = paramDist.lambda(index);
      maxsum::ValType oldM = paramDist.m(index);

      maxsum::ValType newAlpha = oldAlpha + 0.5;
      maxsum::ValType newLambda = oldLambda + 1;
      maxsum::ValType newM = (oldLambda*oldM + x) / newLambda;
      maxsum::ValType newBeta = oldBeta +
         oldLambda*(oldM-x)*(oldM-x)/2.0/newLambda;

      paramDist.alpha(index) = newAlpha;
      paramDist.beta(index) = newBeta;
      paramDist.lambda(index) = newLambda;
      paramDist.m(index) = newM;

   } // function observe

   /**
    * Expands the domain of NormalGamma parameter distributions with
    * DiscreteFunction parameters. When maxsum::DiscreteFunction is used
    * to define the parameters of a distribution, different parameter values
    * may be recorded for each member of a DiscreteFunction's domain. This
    * function expands the domain of a NormalGamma distributions parameters, so
    * that it includes the named variables registered by the maxsum library.
    */
   template<class Policy, class ValType> void expand
   (
    NormalGamma_Tmpl<maxsum::DiscreteFunction,Policy>& paramDist,
    ValType var
   )
   {
      paramDist.alpha.expand(var);
      paramDist.beta.expand(var);
      paramDist.lambda.expand(var);
      paramDist.m.expand(var);
   }

   /**
    * Expands the domain of NormalGamma parameter distributions with
    * DiscreteFunction parameters. When maxsum::DiscreteFunction is used
    * to define the parameters of a distribution, different parameter values
    * may be recorded for each member of a DiscreteFunction's domain. This
    * function expands the domain of a NormalGamma distributions parameters, so
    * that it includes the named variables registered by the maxsum library.
    */
   template<class Policy, class Iterator> void expand
   (
    NormalGamma_Tmpl<maxsum::DiscreteFunction,Policy>& paramDist,
    Iterator varBegin,
    Iterator varEnd
   )
   {
      paramDist.alpha.expand(varBegin,varEnd);
      paramDist.beta.expand(varBegin,varEnd);
      paramDist.lambda.expand(varBegin,varEnd);
      paramDist.m.expand(varBegin,varEnd);
   }

} // namespace dist
} // namespace dec_brl

#endif // DECBRL_NORMALGAMMA_H
