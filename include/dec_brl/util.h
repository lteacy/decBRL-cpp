/**
 * @file util.h
 * Utility functions, types and constants.
 */
#ifndef DECBRL_UTIL_H
#define DECBRL_UTIL_H

#include <boost/utility/enable_if.hpp>
#include <iostream>
#include <boost/container/flat_map.hpp>
#include <cmath>
#include <limits>

/**
 * Namespace all public functions and types defined in the DecBRL library.
 */
namespace dec_brl
{
    /**
     * Default discount factor for infinite horizon MDPs.
     */
    const double DEFAULT_GAMMA = 0.95;
    
    /**
     * Provides a cross-platform representation of infinity.
     * If this platform has infinity defined, we use it, otherwise we use the
     * maximum value for a given type. This decision is made at compile time.
     */
    template<class RealType,
    bool test=std::numeric_limits<RealType>::has_infinity> struct Limits
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
    
    /**
     * Print boost flat_maps for diagnostics.
     * Output format: {K=V,K=V,...}
     */
    template<class K, class V> std::ostream& operator<<
    (
     std::ostream& out,
     const boost::container::flat_map<K,V>& map
    )
    {
        using namespace boost::container;
        typedef typename flat_map<K,V>::const_iterator Iterator;
        out << "{";
        bool printComma = false;
        for(Iterator k=map.begin(); k!=map.end(); ++k)
        {
            if(printComma)
            {
                out << ',';
            }
            else
            {
                printComma=true;
 
            }
            out << k->first << '=' << k->second;
        }
        out << "}";
        return out;
    }
    
    
} // namespace dec_brl

#endif  // DECBRL_UTIL_H
