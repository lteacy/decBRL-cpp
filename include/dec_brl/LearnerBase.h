/**
 * @file LearnerBase.h
 * Base class for policy learners.
 */
#ifndef DEC_BRL_LEARNER_BASE_H
#define DEC_BRL_LEARNER_BASE_H

#include "common.h"

namespace dec_brl
{
    /**
     * Base class for policy learners.
     * Provides empty implementation of required functions, which may not be
     * used by any particular derived class. Notice these are not virtual
     * functions. Instead we stick to concrete instances and templates to
     * avoid the unnecessary overhead.
     */
    template<class Rand=void> struct LearnerBase
    {
        /**
         * Used to inform learner about the structure of the factored MDP
         * it is trying to solve.
         * @param factor the id of a reward factor.
         * @param varBegin iterator to the beginning of a list containing the
         * variable id's (type maxsum::VarID) that this factor depends on.
         * @param varEnd iterator to the end of the variable id list.
         */
        template<class VarIt> void addFactor
        (
         maxsum::FactorID factor,
         VarIt varBegin,
         VarIt varEnd
        )
        {
            // BASE IMPLEMENTATION DOES NOTHING
        }
        
        /**
         * Tells the policy which variables are actions in this MDP
         */
        template<class Iterator> void setActions
        (
         Iterator actionBegin,
         Iterator actionEnd
        )
        {
            // BASE IMPLEMENTATION DOES NOTHING
        }
        
        /**
         * Sets the learner's random number generator if it needs one.
         */
        void setGenerator(Rand* random)
        {
            // BASE IMPLEMENTATION DOES NOTHING
        }
        
        /**
         * Tells the policy which variables are states in this mDP
         */
        template<class Iterator> void setStates
        (
         Iterator stateBegin,
         Iterator stateEnd
        )
        {
            // BASE IMPLEMENTATION DOES NOTHING
        }
        
        /**
         * Should return a set of actions, choosen greedily according to
         * learner's definition.
         */
        template<class ActionMap, class StateMap> void actGreedy
        (
         const StateMap& states,
         ActionMap& actions
        )
        {
            // BASE IMPLEMENTATION DOES NOTHING
        }
        
        /**
         * Chooses actions according to policy, including any exploratory moves.
         */
        template<class ActionMap, class StateMap> void act
        (
         const StateMap& states,
         ActionMap& actions
        )
        {
            // BASE IMPLEMENTATION DOES NOTHING
        }
        
        /**
         * Informs the learner of the result of an MDP step.
         */
        template<class RewardMap, class VarMap> void observe
        (
         const VarMap& priorStates,
         const VarMap& actions,
         const VarMap& postStates,
         const RewardMap& rewards
        )
        {
            // DO NOTHING
        }

        
    }; // class LearnerBase
    
} // namespace dec_brl

#endif // DEC_BRL_LEARNER_BASE_H
