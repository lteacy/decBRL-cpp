/**
 * @file TransBelief.h
 * Defines class for representing Bayesian beliefs about factored transition
 * probabilities using Diriclet conjugate priors.
 */
#ifndef DEC_BRL_TRANS_BELIEF_H
#define DEC_BRL_TRANS_BELIEF_H

#include <boost/random/gamma_distribution.hpp>
#include  <boost/random/uniform_01.hpp>
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
     * Represents a transition probability matrix randomly generated from a
     * TransBelief parameter distribution. Objects of this class are only valid
     * while the parent object exists. If the parent object is destructed,
     * then the behaviour of a TransProb object is undefined. This behaviour
     * allows the implmentation to be simple, efficient, and sufficient for
     * our purposes. Although making the class work beyond the lifetime of
     * the parent shouldn't be too difficult.
     */
    class SampledTransProb;
    
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
        
        /**
         * Let Sampled CPTs shared their parents domain.
         */
        friend SampledTransProb;
        
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
         * Get expected CPT for given condition (as linear column index)
         */
        template<class Derived> void getMeanByInd
        (
         Eigen::DenseBase<Derived>& result,
         int condInd
        )
        {
            result.derived().resize(1,alpha_i.cols());
            result = alpha_i.col(condInd) / alpha_i.col(condInd).sum();
        }
        
        /**
         * Get expected CPT for given vector of conditional variable values.
         */
        template<class It, class Derived> void getMeanByVec
        (
         Eigen::DenseBase<Derived>& result,
         It condStart,
         It condEnd
        )
        {
            int condInd = maxsum::sub2ind(condSize_i.begin(), condSize_i.end(),
                                          condStart, condEnd);
            getMeanByInd(result, condInd);
        }
        
        /**
         * Get expected CPT for given mapped conditional variables.
         */
        template<class M1, class Derived> void getMeanByMap
        (
         Eigen::DenseBase<Derived>& result,
         M1& condMap
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
            //  Use value cache to find right CPT to return
            //******************************************************************
            getMeanByVec(result, condValueCache_i.begin(),
                         condValueCache_i.end());
            
        } // method getMeanByMap
        
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
        ) const
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
    
    /**
     * Represents a transition probability matrix randomly generated from a
     * TransBelief parameter distribution. Objects of this class are only valid
     * while the parent object exists. If the parent object is destructed,
     * then the behaviour of a TransProb object is undefined. This behaviour
     * allows the implmentation to be simple, efficient, and sufficient for
     * our purposes. Although making the class work beyond the lifetime of
     * the parent shouldn't be too difficult.
     */
    class SampledTransProb
    {
    private:
        
        /**
         * The transition belief object that created this object.
         */
        const TransBelief& parent_i;
        
        /**
         * The conditional probability CPT that defines this distribution.
         */
        Eigen::MatrixXd cpt_i;
        
        /**
         * Statically allocated vector for storing conditional variable values.
         * Putting this here avoids unnecessary temporary.
         * @see TransBelief::observeByMap
         */
        Eigen::VectorXi condCache_i;
        
        /**
         * Statically allocated vector for storing conditional variable values.
         * Putting this here avoids unnecessary temporary.
         * @see TransBelief::observeByMap
         */
        Eigen::VectorXi domainCache_i;
        
    public:
        
        // Make new operator work with eigen3 library
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        
        /**
         * Constructor
         * @tparam Rand Type of boost random generator used for sampling.
         * @param parent this object's parent parameter distribution.
         * @param generator random generator used for sampling.
         */
        template<class Rand>
        SampledTransProb(const TransBelief& parent, Rand& generator)
        : parent_i(parent), cpt_i(), condCache_i(parent.condVars_i.size()),
        domainCache_i(parent.domainVars_i.size())
        {
            drawNewCPT(generator);
        }
        
        /**
         * Accessor to CPT.
         */
        const Eigen::MatrixXd& getCPT() const
        {
            return cpt_i;
        }
        
        /**
         * Resample this CPT from the parent distribution.
         * @tparam Rand Type of boost random generator used for sampling.
         * @param generator random generator used for sampling.
         */
        template<class Rand> void drawNewCPT(Rand& generator)
        {
            parent_i.sample(generator,cpt_i);
            
        } // function drawNewCPT
        
        /**
         * Samples next states, given previous states and actions.
         * @param[in] generator random generator used for sampling.
         * @param[in] condVars map of condition variables (states and actions)
         * to values.
         * @param[out] domainVars map in which to store next state values.
         */
        template<class Rand, class CondMap, class DomainMap>
        void drawNextStates
        (
         Rand& generator,
         CondMap& condVars,
         DomainMap& domainVars
        )
        {
            //******************************************************************
            //  Get linear index for conditional distribution
            //******************************************************************
            for(int k=0; k<parent_i.condVars_i.size(); ++k)
            {
                int var = parent_i.condVars_i(k);
                int val = condVars[var];
                condCache_i(k) = val;
            }
            
            int condInd = maxsum::sub2ind(parent_i.condSize_i.begin(),
                                          parent_i.condSize_i.end(),
                                          condCache_i.begin(),
                                          condCache_i.end());
            
            //******************************************************************
            //  Draw a number between 0 and 1
            //******************************************************************
            boost::random::uniform_01<> unirnd;
            double draw = unirnd(generator);
            
            //******************************************************************
            //  follow the cumulative probability function up to the draw
            //******************************************************************
            int domainInd = 0;
            double cdf = 0.0;
            for(; domainInd<parent_i.domainSize(); ++domainInd)
            {
                cdf += cpt_i(domainInd,condInd);
                if(cdf>=draw)
                {
                    break;
                }
            }
            
            //******************************************************************
            //  get the corresponding domain variables values 
            //******************************************************************
            maxsum::ind2sub(parent_i.domainSize_i, domainInd, domainCache_i);
            for(int k=0; k<domainCache_i.size(); ++k)
            {
                int var = parent_i.domainVars_i[k];
                int val = domainCache_i[k];
                domainVars[var] = val;
            }
            
            
        } // function drawNextStates
        
    }; // class TransProb
    
} // namespace dec_brl


#endif // DEC_BRL_TRANS_BELIEF_H