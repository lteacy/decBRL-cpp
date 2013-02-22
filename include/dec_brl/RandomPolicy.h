#ifndef DEC_BRL_RANDOM_POLICY_H
#define DEC_BRL_RANDOM_POLICY_H

#include "random.h"

namespace dec_brl
{

   /**
    * Implements a random policy for a given action domain.
    * For now, we assume the action space is discrete, and select actions from
    * a uniform distribution.
    */
   template<class ActionDomainType, class StateType> class RandomPolicy
   {
   private:

      const ActionDomainType& actionDomain_i;

   public:

      RandomPolicy(const ActionDomainType& actions)
         : actionDomain_i(actions) {}

      /**
       * Return random action from action domain.
       */
      typename ActionDomainType::value_type act(const StateType& state)
      {
         int index = random::unidrnd(0,actionDomain_i.size()-1); 
         return actionDomain_i[index];
      }

   }; // class RandomPolicy


} // namespace dec_brl

#endif // DEC_BRL_RANDOM_POLICY_H
