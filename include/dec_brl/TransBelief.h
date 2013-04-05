/**
 * @file TransBelief.h
 * Defines class for representing Bayesian beliefs about factored transition
 * probabilities using Diriclet conjugate priors.
 */
#ifndef DEC_BRL_TRANS_BELIEF_H
#define DEC_BRL_TRANS_BELIEF_H

#include <boost/random/gamma_distribution.hpp>
#include "EigenWithPlugin.h"
#include <vector>
#include "common.h"
#include "register.h"
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
        
        /**
         * Statically allocated vector for storing conditional variable values.
         * Putting this here avoids unnecessary temporary.
         * @see TransBelief::observeByMap
         */
        Eigen::VectorXi condValueCache_i;
        
        /**
         * Statically allocated vector for storing conditional variable values.
         * Putting this here avoids unnecessary temporary.
         * @see TransBelief::observeByMap
         */
        Eigen::VectorXi domainValueCache_i;
        
    public:
        
        // Make new operator work with eigen3 library
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        
        /**
         * Default prior value for alpha hyperparameters.
         */
        const static double DEFAULT_ALPHA;
        
        /**
         * Construct a belief distribution for a conditional probability
         * table CPT with specified condition and domain variables.
         * @pre condition and domain variable IDs must be registered with
         * maxsum library.
         * @tparam VecType1 vector type with size() returning number of
         * contained elements of type maxsum::VarID, and [] operator providing
         * access to each element.
         * @tparam VecType2 same semantics as VecType2, but for 2nd parameter.
         * @param cond vector of registered condition variable ids.
         * @param domain vector of registered domain variable ids.
         * @param priorAlpha prior value for all \f$\alpha_i\f$ hyperparameters.
         */
        template<class VecType1, class VecType2> TransBelief
        (
         const VecType1& cond,
         const VecType2& domain,
         const double priorAlpha=DEFAULT_ALPHA
        )
        : alpha_i(), condVars_i(cond.size()), condSize_i(cond.size()),
          domainVars_i(domain.size()), domainSize_i(domain.size()),
          condValueCache_i(cond.size()), domainValueCache_i(domain.size())
        {
            //******************************************************************
            // Set conditional variables and cache their registered domain
            // sizes.
            //******************************************************************
            for(int k=0; k<cond.size(); ++k)
            {
                condVars_i[k] = cond[k];
                condSize_i[k] = maxsum::getDomainSize(cond[k]);
            }
            
            //******************************************************************
            // Do the same for the domain variables
            //******************************************************************
            for(int k=0; k<domain.size(); ++k)
            {
                domainVars_i[k] = domain[k];
                domainSize_i[k] = maxsum::getDomainSize(domain[k]);
            }
            
            //******************************************************************
            //  Initialise the Dirichlet Hyperparameters and set them all
            //  equal to the specified prior value.
            //******************************************************************
            alpha_i.setConstant(domainSize_i.prod(), condSize_i.prod(),
                                priorAlpha);
            
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
            alpha_i.setConstant(scalar);
        }
        
        /**
         * Returns the total size of the conditional domain of the CPT.
         */
        int condSize() const
        {
            return alpha_i.cols();
        }
        
        /**
         * Returns the total domain size of the CPT.
         */
        int domainSize() const
        {
            return alpha_i.rows();
        }
        
        /**
         * Update beliefs based on observed condition and domain variables
         * stored in associative arrays.
         * @param condInd linear index of observed condition variables
         * @param domainInd linear index of observed domain variables
         */
        void observeByInd(int condInd, int domainInd)
        {
            alpha_i(domainInd,condInd) += 1;
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
            int condInd = maxsum::sub2ind(condSize_i.begin(), condSize_i.end(),
                                          condStart, condEnd);
            
            int domainInd = maxsum::sub2ind(domainSize_i.begin(),
                                            domainSize_i.end(),
                                            domainStart, domainEnd);
            
            observeByInd(condInd, domainInd);
        }
        
        /**
         * Update beliefs based on observed condition and domain variables
         * stored in associative arrays. Each map should overload the 
         * [] operator s.t. <code>map[x]</code> is the value observed for
         * the variable with maxsum::VarID <code>x</code>.
         * @param condMap map of observed condition variable values
         * @param domainMap map of observed observed domain variable values
         */
        template<class M1, class M2> void observeByMap
        (
         M1& condMap,
         M2& domainMap
        )
        {
            using namespace Eigen;
            
            //******************************************************************
            //  Extract conditional variable values from map
            //******************************************************************
            for(int k=0; k<condVars_i.size(); ++k)
            {
                int var = condVars_i[k];
                int val = condMap[var];
                condValueCache_i[k] = val;
            }
            
            //******************************************************************
            //  Extract domain variable values from map
            //******************************************************************
            for(int k=0; k<domainVars_i.size(); ++k)
            {
                domainValueCache_i[k] = domainMap[domainVars_i[k]];
            }
            
            //******************************************************************
            //  Use value cache to find and update observed element
            //******************************************************************
            observeByVec(condValueCache_i.begin(), condValueCache_i.end(),
                         domainValueCache_i.begin(), domainValueCache_i.end());
        
        } // observeByMap
        
        /**
         * Returns the expected CPT given the current beliefs.
         * @param[out] eigen object in which to store result.
         */
        template<class Derived>
        void getMean(Eigen::DenseBase<Derived>& result)
        {
            result.derived().resize(alpha_i.rows(),alpha_i.cols());
            result=alpha_i.array().rowwise() / alpha_i.array().colwise().sum();
        }
        
        /**
         * Generate a sampled CPT from the Dirichlet distributions.
         * @tparam RandType boost::random UniformRandomNumberGenerator type.
         * @tparam Derived eigen library type for output array.
         * @param random random generator used to generate samples.
         * @param[out] cpt the sampled conditional probability table.
         */
        template<class RandType, class Derived>
        void sample
        (
         RandType& random,
         Eigen::DenseBase<Derived>& cpt
        )
        {
            using namespace boost::random;
            //******************************************************************
            //  Initialise Random Sample using Gamma distribution independent
            //  variates.
            //******************************************************************
            cpt.derived().resize(alpha_i.rows(),alpha_i.cols());
            gamma_distribution<double> gamrnd;
            for(int k=0; k<cpt.size(); ++k)
            {
                gamma_distribution<double>::param_type params(alpha_i(k),1);
                gamrnd.param(params);
                cpt(k) = gamrnd(random);
            }
            
            //******************************************************************
            //  In theory, we now just need to normalise, and we're done.
            //******************************************************************
            Eigen::Array<double, 1, Eigen::Dynamic> totals;
            totals = cpt.colwise().sum();
            cpt.derived().array().rowwise() /= totals;
            
        } // method sample
        
        /**
         * Get << operator access to private members so that can be printed.
         */
        friend std::ostream& dec_brl::operator<<(std::ostream& out, const TransBelief& beliefs);
        
    }; // class TransBelief
    
    /**
     * Default prior value for alpha hyperparameters.
     */
    const double TransBelief::DEFAULT_ALPHA = 1;
    
} // namespace dec_brl


#endif // DEC_BRL_TRANS_BELIEF_H