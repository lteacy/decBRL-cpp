/**
 * @file Dirichlet.h
 * Defines class for presenting Dirichlet distributions
 * @author Luke Teacy
 */
#ifndef DECBRL_CPP_DIRICHLET_H
#define DECBRL_CPP_DIRICHLET_H

#include <boost/math/policies/policy.hpp>
#include "dec_brl/BoostCompatibleArray.h"

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
     * Class representing Dirichlet conjugate prior for multinomial
     * distributions.
     * @tparam IntType integer type representing domain of this distribution
     * @tparam DomainSize number of possible values, if known at compile time
     * (specifying this could make things faster).
     * @tparam Policy boost library policy for calculating math functions.
     */
    template<class IntType=int, class Policy=boost::math::policies::policy<> >
    class Dirichlet_Tmpl
    {
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        
        BoostCompatibleArray<IntType> alpha;
        
        Dirichlet_Tmpl(int size=2, IntType prior=1) : alpha()
        {
            alpha.setConstant(size,prior);
        }
        
    }; // class Dirichlet
    
    /**
     * Update distribution given single observation.
     */
    template<class IntType, class Policy> void observe
    (
     Dirichlet_Tmpl<IntType,Policy>& model,
     int observation
    )
    {
        model.alpha[observation] += 1;
    }
    
    /**
     * Update distribution given sufficient statistics for all possible
     * values.
     */
    template<class IntType, class Policy, class OtherDerived> void observe
    (
     Dirichlet_Tmpl<IntType,Policy>& model,
     const Eigen::ArrayBase <OtherDerived>& stats
    )
    {
        eigen_assert(stats.size()==model.alpha.size());
        model.alpha += stats;
    }
    
    typedef Dirichlet_Tmpl<> Dirichlet;
    
} // namespace dist
} // namespace dec_brl

#endif // DECBRL_CPP_DIRICHLET_H
