#ifndef DEC_BRL_RANDOM_POLICY_H
#define DEC_BRL_RANDOM_POLICY_H

#include "random.h"

namespace dec_brl
{

   /**
    * Implements a factored Q Learning policy using maxsum and e-greedy
    * exploration. 
    */
   class decQLearner
   {
   private:


   public:

      /**
       * Constructor.
       */
      RandomPolicy() : {}

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

