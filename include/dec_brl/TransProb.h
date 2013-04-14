/**
 * @file TransProb.h
 * Defines a factored Transition Probability matrix from which we can draw
 * samples.
 */
#ifndef DECBRL_TRANSPROB_H
#define DECBRL_TRANSPROB_H

#include  <boost/random/uniform_01.hpp>
#include "EigenWithPlugin.h"
#include "common.h"
#include "register.h"

namespace dec_brl {
    
    /**
     * Represents a transition probability matrix. Unlike a
     * dec_brl::SampledTransProb, objects of this class are standalone, and do
     * not shared resources with a dec_brl::TransBelief object.
     */
    class TransProb
    {
    private:
        
        /**
         * The conditional probability CPT that defines this distribution.
         */
        Eigen::MatrixXd cpt_i;
        
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
         * Construct a new transition probability matrix with specified
         * condition and domain variables.
         * @pre condition and domain variable IDs must be registered with
         * maxsum library.
         * @tparam VecType1 vector type with size() returning number of
         * contained elements of type maxsum::VarID, and [] operator providing
         * access to each element.
         * @tparam VecType2 same semantics as VecType2, but for 2nd parameter.
         * @param cond vector of registered condition variable ids.
         * @param domain vector of registered domain variable ids.
         * @post CPT is initially empty, and must be set using setCPT
         */
        template<class Vec1, class Vec2> TransProb
        (
         const Vec1& cond,
         const Vec2& domain
        )
        : cpt_i(), condVars_i(cond.size()), condSize_i(cond.size()),
          domainVars_i(domain.size()), domainSize_i(domain.size()),
          condCache_i(cond.size()), domainCache_i(domain.size())
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
            
        } // constructor
        
        /**
         * Accessor to CPT.
         */
        const Eigen::MatrixXd& getCPT() const
        {
            return cpt_i;
        }
        
        /**
         * Accessor to CPT.
         * @param Eigen library type used to initialise CPT
         */
        template<class Derived> void setCPT
        (
         const Eigen::DenseBase<Derived>& cpt
        ) 
        {
            cpt_i = cpt;
        }
        
        /**
         * Returns the total size of the conditional domain of the CPT.
         */
        int condSize() const
        {
            return cpt_i.cols();
        }
        
        /**
         * Returns the total domain size of the CPT.
         */
        int domainSize() const
        {
            return cpt_i.rows();
        }
        
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
            for(int k=0; k<condVars_i.size(); ++k)
            {
                int var = condVars_i(k);
                int val = condVars[var];
                condCache_i(k) = val;
            }
            
            int condInd = maxsum::sub2ind(condSize_i.begin(),
                                          condSize_i.end(),
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
            for(; domainInd<domainSize(); ++domainInd)
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
            maxsum::ind2sub(domainSize_i, domainInd, domainCache_i);
            for(int k=0; k<domainCache_i.size(); ++k)
            {
                int var = domainVars_i[k];
                int val = domainCache_i[k];
                domainVars[var] = val;
            }
            
            
        } // function drawNextStates
        
    }; // class TransProb
    
} // namespace dec_brl

#endif // DECBRL_TRANSPROB_H
