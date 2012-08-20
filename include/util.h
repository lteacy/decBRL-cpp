/**
 * @file util.h
 * Utility functions, types and constants.
 */
#ifndef DECBRL_UTIL_H
#define DECBRL_UTIL_H

#include <boost/utility/enable_if.hpp>
#include <cmath>
#include <limits>

/**
 * Namespace all public functions and types defined in the DecBRL library.
 */
namespace dec_brl
{
   /**
    * Provides a cross-platform representation of infinity.
    * If this platform has infinity defined, we use it, otherwise we use the
    * maximum value for a given type. This decision is made at compile time.
    */
   template<class RealType, bool test=std::numeric_limits<RealType>::has_infinity> struct Limits
   {
      static const RealType infinity();
   };

   /**
    * Provides a cross-platform representation of infinity.
    * If this platform has infinity defined, we use it, otherwise we use the
    * maximum value for a given type. This decision is made at compile time.
    */
   template<class RealType> struct Limits<RealType,false>
   {
      static inline const RealType infinity()
      {
         return std::numeric_limits<RealType>::max();
      }
   };

   /**
    * Provides a cross-platform representation of infinity.
    * If this platform has infinity defined, we use it, otherwise we use the
    * maximum value for a given type. This decision is made at compile time.
    */
   template<class RealType> struct Limits<RealType,true>
   {
      static inline const RealType infinity()
      {
         return std::numeric_limits<RealType>::infinity();
      }
   };

} // namespace dec_brl

#endif  // DECBRL_UTIL_H
