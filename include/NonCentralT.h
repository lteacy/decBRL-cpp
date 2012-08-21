/**
 * @file NonCentralT.h
 * Defines a class representing non-central Student's t distributions with 
 * location and scale parameters.
 */
#ifndef DECBRL_NONCENTRALT_H
#define DECBRL_NONCENTRALT_H

#include <cassert>
#include <boost/math/distributions/students_t.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/random/uniform_real.hpp>
#include "util.h"

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
    * Class representing a scaled distribution of some type.
    * This class is defined using the
    * conventions and concepts of the boost math libraries.
    */
   template<class BaseDistribution, class RealType=double>
   class Scaled 
   {
   public:

      /**
       * Boost.Math calculation policy used by this object.
       */
      typedef typename BaseDistribution::policy_type policy_type;

      /**
       * Type of base distribution.
       */
      typedef BaseDistribution base_type;

   private:

      /**
       * Location (the mode) of this distribution.
       */
      RealType loc_i;

      /**
       * The scale of this distribution.
       */
      RealType scale_i;

      /**
       * Underlying base distribution
       */
      BaseDistribution base_i;

   public: 

      /**
       * Constructs a new distribution with specified parameters.
       * @param base the base distribution to be scaled
       * @param loc location parameter for this distribution
       * @param scale scale parameter for this distribution
       */
      Scaled
      (
       const BaseDistribution& base,
       RealType loc=0,
       RealType scale=1
      )
      : loc_i(loc), scale_i(scale), base_i(base) {}

      /**
       * Copy constructor.
       */
      Scaled(Scaled<BaseDistribution,RealType>& s)
      : loc_i(s.loc_i), scale_i(s.scale_i), base_i(s.base_i) {}

      /**
       * Copy assignment.
       */
      virtual Scaled<BaseDistribution,RealType>& operator=
      (
       Scaled<BaseDistribution,RealType>& s
      )
      {
         loc_i = s.loc_i;
         scale_i = s.scale_i;
         base_i = s.base_i;
         return *this;
      }

      /**
       * Returns a t distribution with the same degrees of freedom as
       * this distribution.
       */
      const BaseDistribution& base() const
      {
         return base_i;
      }

      /**
       * Returns the scale parameter for this distribution.
       */
      RealType scale() const { return scale_i; }

      /**
       * Returns the location parameter (mode) of this distribution.
       */
      RealType location() const { return loc_i; }

   }; // class Scaled

   /**
    * Base class used to identify Non-Central T distributions.
    * Currently this class serves no other purpose than to help in template
    * selection. It is therefore only used at compile time, and has no
    * affect at runtime.
    */
   class AbstractNonCentralT {};

   /**
    * Class Representing non-central Student's t distribution with
    * location and scale parameters. This class is defined using the
    * conventions and concepts of the boost math libraries.
    */
   template<class RealType=double,class Policy=boost::math::policies::policy<> >
   class NonCentralT_Tmpl : public AbstractNonCentralT, public
      Scaled<boost::math::students_t_distribution<RealType, Policy>, RealType>
   {
   private:

      typedef Scaled<boost::math::students_t_distribution
         <RealType, Policy>, RealType> BaseClass;

      /**
       * Used to generator random numbers in range [0,1].
       */
      static boost::uniform_real<RealType> unirand_s;

   public: 

      /**
       * Type of value required by the () operator for generating random
       * variates from this distribution. This is required to implement
       * the Random Distribution concept of the Boost Random library.
       */
      typedef RealType input_type;

      /**
       * Type used to represent real values in this object.
       * Used by Boost Random library.
       */
      typedef RealType result_type;

      /**
       * Type of base distribution.
       */
      typedef typename BaseClass::base_type base_type;

      /**
       * Required to implement the Boost library Random Distribution concept.
       * Ensures independence between previous and subsequent random variates
       * generated by the () operator. However, only i.i.d. samples are
       * generated, this function does nothing.
       */
      void reset() {}

      /**
       * Generates a random variate from this distribution using a given
       * random number generator.
       */
      template<typename Engine> RealType operator()(Engine& engine);

      /**
       * Constructs a new distribution with specified parameters.
       * @param df degrees of freedom for this distribution
       * @param loc location parameter for this distribution
       * @param scale scale parameter for this distribution
       */
      NonCentralT_Tmpl
      (
       const NonCentralT_Tmpl<RealType,Policy>& dist
      )
      : BaseClass(dist.base(), dist.location() ,dist.scale()) {}

      /**
       * Constructs a new distribution with specified parameters.
       * @param df degrees of freedom for this distribution
       * @param loc location parameter for this distribution
       * @param scale scale parameter for this distribution
       */
      NonCentralT_Tmpl
      (
       RealType df,
       RealType loc=0,
       RealType scale=1
      )
      : BaseClass(base_type(df),loc,scale) {}

      /**
       * Returns the degrees of freedom for this distribution.
       */
      RealType degrees_of_freedom() const
      {
         return BaseClass::base().degrees_of_freedom();
      }

      /**
       * Returns true iff this distribution has finite variance.
       */
      bool hasVariance()
      {
         return BaseClass::base().degrees_of_freedom()>2;
      }

   }; // class NonCentralT

   /**
    * Convenience typedef for distributions that uses the
    * default template parameters.
    */
   typedef class NonCentralT_Tmpl<> NonCentralT;

} // namespace dist
} // namespace dec_brl

/**
 * The standard namespace for the Boost collection of libraries.
 * Here, we extend the boost namespace so that certain functions
 * are overloaded to work with our data types.
 * @see http://www.boost.org/
 */
namespace boost
{

/**
 * The standard namespace for the Boost.Math library.
 * Here, we extend the boost namespace so that certain functions
 * are overloaded to work with our data types.
 * @see http://www.boost.org/
 */
namespace math
{
   /**
    * Overloads the Boost.Math library variance function for
    * Scaled distributions.
    * @pre degrees of freedom must be greater than 2.
    * @returns the variance of the non-central t distribution.
    */
   template <class Dist>
   typename boost::enable_if
   <boost::is_base_of<dec_brl::dist::AbstractNonCentralT,Dist>,
   typename Dist::base_type::value_type>::type variance
   (
    const Dist& dist
   )
   {
      typedef typename Dist::base_type::value_type RealType;

      if(dist.degrees_of_freedom()<2)
      {
         return dec_brl::Limits<RealType>::infinity();
      }

      RealType result = dist.scale()*dist.scale()*variance(dist.base());
      assert(result>0);
      return result;
   }

   /**
    * Overloads the Boost.Math library variance function for
    * Scaled distributions.
    * @pre degrees of freedom must be greater than 2.
    * @returns the variance of the non-central t distribution.
    */
   template <class Dist>
   typename boost::disable_if
   <boost::is_base_of<dec_brl::dist::AbstractNonCentralT,Dist>,
   typename Dist::base_type::value_type>::type variance
   (
    const Dist& dist
   )
   {
      typedef typename Dist::base_type::value_type RealType;
      RealType result = dist.scale()*dist.scale()*variance(dist.base());
      assert(result>0);
      return result;
   }

   /**
    * Overloads the Boost.Math library standard_deviation function for
    * scaled distributions.
    * @returns the standard deviation of the non-central t distribution.
    */
   template <class Dist>
   typename boost::disable_if
   <boost::is_base_of<dec_brl::dist::AbstractNonCentralT,Dist>,
   typename Dist::base_type::value_type>::type standard_deviation
   (
    const Dist& dist
   )
   {
      typedef typename Dist::base_type::value_type RealType;
      RealType result = dist.scale()*standard_deviation(dist.base());
      assert(result>0);
      return result;
   }

   /**
    * Overloads the Boost.Math library standard_deviation function for
    * NonCentralT distributions.
    * @returns the standard deviation of the non-central t distribution.
    */
   template <class Dist>
   typename boost::enable_if
   <boost::is_base_of<dec_brl::dist::AbstractNonCentralT,Dist>,
   typename Dist::base_type::value_type>::type standard_deviation
   (
    const Dist& dist
   )
   {
      typedef typename Dist::base_type::value_type RealType;

      if(dist.degrees_of_freedom()<2)
      {
         return dec_brl::Limits<RealType>::infinity();
      }

      RealType result = dist.scale()*standard_deviation(dist.base());
      assert(result>0);
      return result;
   }

   /**
    * The cumulative distribution function for scaled distributions.
    * Overloads the Boost.Math library equivalent for our own distribution type.
    */
   template <class Dist> typename Dist::base_type::value_type cdf
   (
    const Dist& dist,
    const typename Dist::base_type::value_type x
   )
   {
      typedef typename Dist::base_type::value_type RealType;
      RealType unscaled_param = (x-dist.location()) / dist.scale();
      return cdf(dist.base(),unscaled_param);
   }

   /**
    * The cumulative distribution function for the complement of a scaled
    * distribution.
    */
   template <class Dist> typename Dist::base_type::value_type cdf
   (
    const complemented2_type<Dist, typename Dist::base_type::value_type> c
   )
   {
      typedef typename Dist::base_type::value_type RealType;
      RealType unscaled_param = (c.param-c.dist.location()) / c.dist.scale();
      complemented2_type<typename Dist::base_type, RealType>
         unscaled_c(c.dist.base(),unscaled_param);
      return cdf(unscaled_c);
   }

   /**
    * The quantile (inverse-cdf) for a scaled distributions.
    * @param dist the probability distribution
    * @param p the position of the quantile requested.
    * @pre p must be in range [0,1].
    */
   template<class Dist> inline typename Dist::base_type::value_type quantile
   (
    const Dist& dist,
    const typename Dist::base_type::value_type& p
   )
   {
      typename Dist::base_type::value_type tQuantile = quantile(dist.base(),p);
      return dist.scale()*tQuantile+dist.location();
   }

   /**
    * The quantile (inverse-cdf) for the complement NonCentralT distribution.
    * @param dist the probability distribution
    * @param p the position of the quantile requested.
    * @pre p must be in range [0,1].
    */
   template <class Dist> inline typename Dist::base_type::value_type quantile
   (
    const complemented2_type<Dist, typename Dist::base_type::value_type> c
   )
   {
      typedef typename Dist::base_type::value_type value_type;
      complemented2_type<typename Dist::base_type, value_type>
         unscaled_c(c.dist.base(),c.param);
      value_type tQuantile = quantile(unscaled_c);
      return c.dist.scale()*tQuantile+c.dist.location();
   }

} // namespace math
} // namespace boost

namespace dec_brl {
namespace dist {

   /**
    * Used to generator random numbers in range [0,1].
    */
   template<class RealType, class Policy> boost::uniform_real<RealType>
      NonCentralT_Tmpl<RealType,Policy>::unirand_s(0,1); 

   /**
    * Generates a random variate from this distribution using a given
    * random number generator.
    */
   template<class RealType, class Policy> template<typename Engine>
      RealType NonCentralT_Tmpl<RealType,Policy>::operator()(Engine& engine)
   {
      RealType p = unirand_s(engine);
      RealType result = boost::math::quantile(*this,p);
      return result;

   } // operator()

}} // namespace dec_brl::dist



#endif // DECBRL_NONCENTRALT_H
