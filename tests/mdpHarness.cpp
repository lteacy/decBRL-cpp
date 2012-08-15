#include <iostream>
#include "SimpleMDP.h"
#include "RandomPolicy.h"
#include "random.h"
#include <algorithm>


/**
 * Test harness simulates simple MDP and tests planning strategies.
 */
int main()
{
   using namespace mdp_sim;
   double x = 2;
   double y = 3;
   std::cout << "Hello world! " << std::min<const double&>(x,y) << std::endl;

   //***************************************************************************
   // Initialse random number generator used by all stochastic functions in
   // this library.
   //***************************************************************************
   random::initRandomEngineByTime();

   //***************************************************************************
   // Create MDP Simulator
   //***************************************************************************
   SimpleMDP mdp;

   //***************************************************************************
   // Create planner
   //***************************************************************************
   RandomPolicy<SimpleMDP::ActionDomainType,SimpleMDP::StateType>
      policy(mdp.getActionDomain());

   const SimpleMDP::ActionDomainType& actionDomain
      = mdp.getActionDomain();

   std::cout << "Action Domain: size " << actionDomain.size() << std::endl;

   //***************************************************************************
   // Simulate planner interacting with environment for a number of timesteps
   //***************************************************************************
   int timesteps = 10;
   SimpleMDP::StateType state = mdp.initState();
   for(int i=0; i<timesteps; i++)
   {
      SimpleMDP::ActionType action = policy.act(state);
      SimpleMDP::RewardType reward;
      mdp.simulate(action,state,reward);

      std::cout << "timestep " << i << ": <a:" << action << " s:" << state
                << " r: " << reward << ">" << std::endl;
   }
   
   //***************************************************************************
   // Return sucessfully
   //***************************************************************************
   return 0;
}

