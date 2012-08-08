#include "SimpleMDP.h"
namespace mdp_sim
{

   /**
    * Default constructor.
    * Sets up action and state domains.
    */
   SimpleMDP::SimpleMDP()
      : actionDomain_i(2), curState_i(0)
   {

      actionDomain_i[0] = 0;
      actionDomain_i[1] = 1;

   } // SimpleMDP default constructor

   /**
    * Returns the next state and reward for a given action.
    */
   void SimpleMDP::simulate
   (
    const ActionType& action,
    StateType& state,
    RewardType& reward
   )
   {
      curState_i += action;

      if(2<curState_i)
      {
         reward = 10;
         curState_i = 0;
      }
      else
      {
         reward = -1;
      }
      state = curState_i;
      
   } // SimpleMDP::simulate

} // namespace mdp_sim
