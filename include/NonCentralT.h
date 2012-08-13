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
    * Class Representing Normal-Gamma distributions.
    * @tparam RealType scalar type used for parameters and return values.
    * @tparam Policy Boost.Math policy used to calculate results. This effects
    * result accuracy, but the default policy is normally suffice.
    * In Bayesian Analysis, this class of distribution is conjugate
    * for Normal distributions with unknown mean and variance.
    */
   template<class RealType=double, class Policy=boost::math::policies:policy<> >
      class NormalGamma
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

   }; // class NormalGamma distribution

   /**
    * Convenience typedef for distributions that use the
    * default template parameters.
    */
   typedef NormalGamma<> NormalGamma;

   /**
    * Class Representing non-central Student's t distribution with
    * location and scale parameters. This class is defined using the
    * conventions and concepts of the boost math libraries.
    */
   template<class RealType, class Policy> class NonCentralT
      : private boost::math::students_t_distribution<RealType, Policy>
   {
   private:

      // NOTE: Inheritance from students_t_distribution is kept private
      // because these distributions are concrete types. Pointers of type
      // students_t_distribution* to NonCentralT objects might not result
      // in expected behaviour, because functions are not virtual.
      // Keeping inheritance private therefore discourages incorrect use.

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
       * Constructor
       * @param df degrees of freedom for this distribution
       * @param loc location parameter for this distribution
       * @param scale scale parameter for this distribution
       */
      NonCentralT
      (
       RealType df,
       RealType loc=0,
       RealType scale=1,
      )
      : boost::math::students_t_distribution(df), loc_i(loc), scale_i(scale) {}

      /**
       * Returns the degrees of freedom for this distribution.
       */
      RealType degrees_of_freedom() const
      {
         return boost::math::students_t_distribution::degrees_of_freedom();
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
    * Convenience typedef for distributions that use the
    * default template parameters.
    */
   typedef NonCentralT<> NonCentralT;

} // namespace dist
} // namespace dec_brl


#endif // DECBRL_NONCENTRALT_H
