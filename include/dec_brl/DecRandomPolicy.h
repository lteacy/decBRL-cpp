/**
 * @file DecRandomPolicy.h
 * Simple random baseline policy for choosing actions.
 */
#ifndef DEC_BRL_DEC_RANDOM_POLICY_H
#define DEC_BRL_DEC_RANDOM_POLICY_H

#include "dec_brl/LearnerBase.h"
#include <boost/container/flat_map.hpp>
#include <boost/random/uniform_smallint.hpp>
#include "common.h"
#include "register.h"


namespace dec_brl {
    
    /**
     * Implements a simple random baseline policy that doesn't learn anything.
     * @tparam type of random number generator used for choosing actions.
     */
    template<class Rand> class DecRandomPolicy : public LearnerBase<Rand>
    {
    private:
        
        /**
         * Random number generator.
         */
        Rand* pGenerator_i;
        
        /**
         * Type used to store action variables names and domain sizes.
         */
        typedef boost::container::flat_map<maxsum::VarID,maxsum::ValIndex> Map;
        
        /**
         * Cache of action variable names and domain sizes.
         */
        Map actions_i;
        
    public:
        
        /**
         * Default Constructor.
         *
         */
        DecRandomPolicy() : pGenerator_i(0) {}
        
        /**
         * (Deep) Copy constructor.
         */
        DecRandomPolicy(const DecRandomPolicy& rhs)
        : pGenerator_i(rhs.pGenerator_i), actions_i(rhs.actions_i) {}
        
        /**
         * (Deep) Copy assignment.
         */
        DecRandomPolicy& operator=(const DecRandomPolicy& rhs)
        {
            pGenerator_i = rhs.pGenerator_i;
            actions_i = rhs.actions_i;
            return *this;
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
            actions_i.reserve(actionEnd-actionBegin);
            for(Iterator k=actionBegin; k!=actionEnd; ++k)
            {
                actions_i[*k] = maxsum::getDomainSize(*k);
            }
            
        } // setStates function
        
        /**
         * Set the random number generator.
         */
        void setGenerator(Rand* random)
        {
            pGenerator_i = random;
        }
        
        /**
         * Chooses a set of random actions.
         */
        template<class ActionMap, class StateMap> void actGreedy
        (
         const StateMap& states,
         ActionMap& actions
        )
        {
            act(states,actions);
        }
        
        /**
         * Chooses a set of random actions.
         */
        template<class ActionMap, class StateMap> void act
        (
         const StateMap& states,
         ActionMap& actions
        )
        {
            using namespace boost::random;
            
            assert(0!=pGenerator_i); // make sure generator has been set
            
            for(Map::const_iterator k=actions_i.begin();
                k!=actions_i.end(); ++k)
            {
                maxsum::ValIndex mx = k->second;
                uniform_smallint<maxsum::ValIndex> random(0,mx-1);
                actions[k->first] = random(*pGenerator_i);
            }
        }
        
    }; // class DecRandomPolicy
    
} // namespace dec_brl

#endif // DEC_BRL_DEC_RANDOM_POLICY_H

