/**
 * @file NonCentralT.h
 * Defines a class representing non-central Student's t distributions with 
 * location and scale parameters.
 */
#ifndef DECBRL_NONCENTRALT_H
#define DECBRL_NONCENTRALT_H

#include <boost/math/distributions/students_t.hpp>

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
    * Class Representing non-central Student's t distribution with
    * location and scale parameters. This class is defined using the
    * conventions and concepts of the boost math libraries.
    */
   template<class RealType=double,class Policy=boost::math::policies::policy<> >
   class NonCentralT_Tmpl
      : private boost::math::students_t_distribution<RealType, Policy>
   {
   private:

      // NOTE: Inheritance from students_t_distribution is kept private
      // because these distributions are concrete types. Pointers of type
      // students_t_distribution* to NonCentralT objects might not result
      // in expected behaviour, because functions are not virtual.
      // Keeping inheritance private therefore discourages incorrect use.

      /**
       * Convenience typedef for base class.
       */
      typedef boost::math::students_t_distribution<RealType, Policy> BaseClass;

      /**
       * Location (the mode) of this distribution.
       */
      RealType loc_i;

      /**
       * The scale of this distribution.
       */
      RealType scale_i;

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
      : BaseClass(df), loc_i(loc), scale_i(scale) {}

      /**
       * Returns the degrees of freedom for this distribution.
       */
      RealType degrees_of_freedom() const
      {
         return BaseClass::degrees_of_freedom();
      }

      /**
       * Returns the scale parameter for this distribution.
       */
      RealType scale() const { return scale_i; }

      /**
       * Returns the location parameter (mode) of this distribution.
       */
      RealType location() const { return loc_i; }

   }; // class NonCentralT

   /**
    * Convenience typedef for distributions that uses the
    * default template parameters.
    */
   typedef class NonCentralT_Tmpl<> NonCentralT;

   /**
    * Class used to generate non-central t distributed random variates.
    */
   template<class RealType=double,class Policy=boost::math::policies::policy<> >
   class NonCentralTRandom_Tmpl
   {
   public:

      /**
       * Creates a new random generator for the specified distribution.
       */
      NonCentralTRandom_Tmpl(const NonCentralT_Tmpl<RealType,Policy>& dist) {}
      
      /**
       * Returns a random variate distributed according to this
       * generator's distribution.
       */
      RealType operator()() { return 0.0; }

   }; // class NonCentralTRandom_Tmpl

   /**
    * Convenience typedef for random generator that uses default
    * template parameters.
    */
   typedef class NonCentralTRandom_Tmpl<> NonCentralTRandom;

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
    * Overloads the Boost.Math library standard_deviation function for
    * dec_brl::dist::NonCentralT_Tmpl objects.
    * @returns the standard deviation of the non-central t distribution.
    */
   template <class RealType, class Policy> RealType standard_deviation
   (
    const dec_brl::dist::NonCentralT_Tmpl<RealType, Policy>& dist
   )
   {
      return 0;
   }

   /**
    * Overloads the Boost.Math library variance function for
    * dec_brl::dist::NonCentralT_Tmpl objects.
    * @returns the variance of the non-central t distribution.
    */
   template <class RealType, class Policy> RealType variance
   (
    const dec_brl::dist::NonCentralT_Tmpl<RealType, Policy>& dist
   )
   {
      return 0;
   }

} // namespace math
} // namespace boost


#endif // DECBRL_NONCENTRALT_H
