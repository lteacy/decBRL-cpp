/**
 * @file TransBelief.h
 * Defines class for representing Bayesian beliefs about factored transition
 * probabilities using Diriclet conjugate priors.
 */
#ifndef DEC_BRL_TRANS_BELIEF_H
#define DEC_BRL_TRANS_BELIEF_H

#include <Eigen/Dense>
#include <vector>
#include "common.h"
#include <iostream>

namespace dec_brl {
    
    /**
     * Represents Bayesian Beliefs about a factored transition CPT using
     * Dirichlet conjugate priors.
     */
    class TransBelief;
    
    /**
     * Produce text representation of beliefs for diagnostics.
     */
    std::ostream& operator<<(std::ostream& out, const TransBelief& beliefs);
    
    /**
     * Represents Bayesian Beliefs about a factored transition CPT using
     * Dirichlet conjugate priors.
     */
    class TransBelief
    {
    private:
        
        /**
         * Matrix of Dirichlet hyperparameters for this belief distribution.
         */
        Eigen::MatrixXd alpha_i;
        
        /**
         * Input (condition) variables for the Conditional Probability Table
         */
        Eigen::VectorXi condVars_i;
        
        /**
         * Domain size cache for condition variables.
         */
        Eigen::VectorXi condSize_i;
        
        /**
         * Output (domain) variables for the Conditional Probability Table.
         */
        Eigen::VectorXi domainVars_i;
        
        /**
         * Domain size cache for the domain variable sizes.
         */
        Eigen::VectorXi domainSize_i;
        
    public:
        
        // Make new operator work with eigen3 library
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        
        /**
         * Construct a belief distribution for a conditional probability
         * table CPT with specified condition and domain variables.
         * @pre condition and domain variable IDs must be registered with
         * maxsum library.
         * @tparam VecType1 vector type with size() returning number of
         * contained elements of type maxsum::VarID, and [] operator providing
         * access to each element.
         * @tparam VecType2 same semantics as VecType2, but for 2nd parameter.
         * @param cond vector of registered condition variable ids
         * @param domain vector of registered domain variable ids.
         */
        template<class VecType1, class VecType2> TransBelief
            (const VecType1& cond, const VecType2& domain)
        : alpha_i(), condVars_i(cond.size()), condSize_i(cond.size()),
          domainVars_i(domain.size()), domainSize_i(domain.size())
        {
            //******************************************************************
            // Set 
            //******************************************************************
            
            //******************************************************************
            //******************************************************************
            
            //******************************************************************
            //
            //******************************************************************
            
        } // constructor
        
        /**
         * Accessor method for Dirichlet hyperparameters.
         */
        const Eigen::MatrixXd& getAlpha() const
        {
            return alpha_i;
        }
        
        /**
         * Set all hyperparameters to constant scalar
         */
        void setAlpha(double scalar)
        {
            
        }
        
        /**
         * Update beliefs based on observed condition and domain variables
         * stored in associative arrays.
         * @param condInd linear index of observed condition variables
         * @param domainInd linear index of observed domain variables
         */
        void observeByInd(int condInd, int domainInd)
        {
            
        }
        
        /**
         * Update beliefs based on observed condition and domain variables,
         * whose values are passed in vectors. Each vector is specifed by
         * an iterator to the first and one past the last values.
         * for example, std::vector::begin() and std::vector::end() qualify
         * as valid iterators for this purpose.
         */
        template<class It1, class It2> void observeByVec
            (It1 condStart, It1 condEnd, It2 domainStart, It2 domainEnd)
        {
            
        }
        
        /**
         * Update beliefs based on observed condition and domain variables
         * stored in associative arrays. Each map should overload the 
         * [] operator s.t. <code>map[x]</code> is the value observed for
         * the variable with maxsum::VarID <code>x</code>.
         * @param condInd map of observed condition variable values
         * @param domainInd map of observed observed domain variable values
         */
        template<class M1, class M2> void observeByMap(M1 condInd, M2 domainInd)
        {
            
        }
        
        /**
         * Get << operator access to private members so that can be printed.
         */
        friend std::ostream& dec_brl::operator<<(std::ostream& out, const TransBelief& beliefs);
        
    }; // class TransBelief
    
} // namespace dec_brl


#endif // DEC_BRL_TRANS_BELIEF_H