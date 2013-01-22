#ifndef DEC_BRL_SIMPLE_MDP_H
#define DEC_BRL_SIMPLE_MDP_H

#include<vector>

namespace dec_brl
{

   /**
    * A simple MDP for test purposes.
    * positive reward returned if action "HIT" is chosen twice in a row.
    */
   class SimpleMDP
   {
   public:

      typedef double RewardType;
      typedef int ActionType;
      typedef int StateType;
      typedef std::vector<ActionType> ActionDomainType;

   private:

      ActionDomainType actionDomain_i;
      StateType curState_i;

   public:

      /**
       * Default constructor.
       * Sets up action and state domains.
       */
      SimpleMDP();

      /**
       * Returns an initial state
       */
      StateType initState() { return curState_i; }

      /**
       * Returns the next state and reward for a given action.
       */
      void simulate
      (
       const ActionType& action,
       StateType& state,
       RewardType& reward
      );

      /**
       * Accessor method for the action domain
       */
      const ActionDomainType& getActionDomain()
      {
         return actionDomain_i;
      }

   }; // class SimpleMDP

} // namespace dec_brl

#endif // DEC_BRL_SIMPLE_MDP_H
