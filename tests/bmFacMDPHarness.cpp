#include <iostream>
#include "dec_brl/DecBayesModelLearner.h"
#include "dec_brl/LearningSolver.h"
#include "dec_brl/DecQLearner.h"
#include "dec_brl/random.h"
#include "register.h"
#include "DiscreteFunction.h"
#include <algorithm>
#include <fstream>

/**
 * Private Module namespace.
 */
namespace {

/**
 * A Simple Factored MDP for testing, which consists of a single factor.
 * Returns positive reward each possible action is chosen
 * alternately.
 */
class MultiFactorMDP
{
public:

   /**
    * Map type used to pass action and state values around.
    */
   typedef std::map<maxsum::VarID,maxsum::ValIndex> VarMap;

   /**
    * Map type for passing back Factored Rewards
    */
   typedef std::map<maxsum::FactorID,double> RewardMap;

private:

   /**
    * Map containing the state variable mapped to its current value.
    */
   VarMap state_i;

   /**
    * Map containing the last performed action mapped to its current value.
    */
   VarMap lastAction_i;

public:

   /**
    * Number of reward factors.
    */
   const static int NUM_FACTORS = 4; // (numbered 1,3,5,7)
 
   /**
    * Number of state variables.
    */
   const static int NUM_STATES = 4; // (numbered 1,3,5,7)

   /**
    * Number of action variables.
    */
   const static int NUM_ACTIONS = 5; // (numbered 0,2,4,6,8)

   /**
    * Number of possible values for each variable.
    */
   const static int NUM_VALS = 2;

   /**
    * Default Constructor.
    */
   MultiFactorMDP() : state_i(), lastAction_i()
   {
      //************************************************************************
      // Register the state and action variables with the maxsum library
      //************************************************************************
      for(int v=0; v<=8; ++v)
      {
         maxsum::registerVariable(v,NUM_VALS);
      }

      //************************************************************************
      // Set the current state: first state is filled, rest are all empty
      //************************************************************************
      state_i[1] = 1;
      for(int s=3; s<=7; s+=2)
      {
         state_i[s] = 0;
      }

   } // default constructor.

   /**
    * Inform learner of the factors defined by this MDP.
    */
   template<class Learner> void addFactors(Learner& learner)
   {
      //************************************************************************
      // Register set of factors.
      // Each factor is odd numbered and depends on the (even numbered) action
      // variables either side of it, plus the identically (odd numbered) state
      // variable.
      //************************************************************************
      int varIds[3];
      for(int factor=1; factor<=7; factor+=2)
      {
         varIds[0]=factor-1;
         varIds[1]=factor;
         varIds[2]=factor+1;
         learner.addFactor(factor,varIds,varIds+3);
      }

   } // addFactors method
   
   /**
    * Get the current state value.
    */
   VarMap& getState()
   {
      return state_i;
   }

   /**
    * Get the last performed action.
    */
   const VarMap& getlastAction()
   {
      return lastAction_i;
   }

   /**
    * Perform an action and return the resulting reward.
    * @returns total reward
    */
   double act(VarMap& action, RewardMap& reward)
   {
      //************************************************************************
      // Set the last action to the one being performed.
      //************************************************************************
      lastAction_i = action;

      //************************************************************************
      // Set factored state and reward
      //************************************************************************
      double totReward = 0;
      for(int s=1; s<=7; s+=2)
      {
         //*********************************************************************
         // Positive reward for hitting a target
         //*********************************************************************
         if( (1==action[s-1]) && (0==action[s+1]) && (1==state_i[s]) )
         {
            //std::cout << "ACT: F" << s << "A" << action[s-1] << action[s+1]
            //   << "S" << state_i[s] << std::endl;
            state_i[s]=0;
            reward[s]=s*10;
         }
         //*********************************************************************
         // Negative reward for missing a target
         //*********************************************************************
         else if(1==state_i[s])
         {
            reward[s] = (-s);
         }
         //*********************************************************************
         // Generate targets if both agents pointing the wrong way
         //*********************************************************************
         else if( (0==action[s-1]) && (1==action[s+1]) )
         {
            state_i[s]=1;
            reward[s]=0;
         }
         else
         {
            reward[s]=0;
         }

         totReward += reward[s];

      } // for loop

      //************************************************************************
      // Return the total reward
      //************************************************************************
      return totReward;

   } // method act

}; // class MultiFactorMDP

/**
 * Utility function for printing maps
 */
template<class K, class V> std::ostream& operator<<
(
 std::ostream& out,
 std::map<K,V> map
)
{
   typedef typename std::map<K,V>::const_iterator Iterator;
   out << '[';
   bool isFirst=true;

   for(Iterator k=map.begin(); k!=map.end(); ++k)
   {
      if(!isFirst) out << ',';
      out << k->first << '=' << k->second;
      isFirst=false;
   }
   out << ']';
   out.flush();
   return out;
}

/**
 * Utility Class for writing results of run into a CSV file.
 */
class CSVWriter
{
private:

   /**
    * File stream that we write to.
    */
   std::ofstream out_i;

public:

   /**
    * Type for variable to value maps.
    */
   typedef std::map<maxsum::VarID,maxsum::ValIndex> VarMap;

   /**
    * Type for Factor to reward maps.
    */
   typedef std::map<maxsum::FactorID,double> RewardMap;

   /**
    * Constructor opens file and writes header.
    */
   CSVWriter(const char* const psFilename) : out_i(psFilename)
   {
      out_i << "PriorState;Action;PostState;Reward;isExploratory\n";
      out_i.flush();
   }

   /**
    * Write out result.
    */
   void write
   (
      VarMap& priorStates,
      VarMap& actions,
      VarMap& postStates,
      RewardMap& rewards,
      bool isExploratory
   )
   {
      out_i << priorStates << ';';
      out_i << actions << ';';
      out_i << postStates << ';';
      out_i << rewards << ';';
      out_i << isExploratory << '\n';
      out_i.flush();
   }

   /**
    * Destructor ensure file is closed.
    */
   virtual ~CSVWriter()
   {
      if(out_i.is_open())
      {
         out_i.flush();
         out_i.close();
      }
   }

}; // class CSVWriter

} // module namespace

/**
 * Test harness simulates simple MDP and tests planning strategies.
 */
int main(int argc, char* argv[])
{
   using namespace dec_brl;
   std::cout << "Hello world! " << std::endl;

   //***************************************************************************
   // Complain if we haven't been given a filename for writing results to file.
   //***************************************************************************
   if(2!=argc)
   {
      std::cout << "Wrong number of arguments!\n";
      std::cout << "Usage: " << argv[0] << " DataFilename" << std::endl;
      return EXIT_FAILURE;
   }

   //***************************************************************************
   // Initialse random number generator used by all stochastic functions in
   // this library.
   //***************************************************************************
   random::initRandomEngineByTime();

   //***************************************************************************
   // Create MDP Simulator
   //***************************************************************************
   std::cout << "Constructing MDP" << std::endl;
   MultiFactorMDP mdp;

   //***************************************************************************
   // Create learner that uses e-greedy q-learning as its MDP solver
   //***************************************************************************
   std::cout << "Constructing Learner" << std::endl;
   DecBayesModelLearner< LearningSolver<DecQLearner> > learner;
   std::cout << "Adding Factors to Learner" << std::endl;
   mdp.addFactors(learner);

   //***************************************************************************
   // Define some maps to old the factor rewards, states and actions.
   // Note that the postState immediately becomes the priorState in the first
   // iteration of the for loop below.
   //***************************************************************************
   std::cout << "Constructing state, reward and action maps" << std::endl;
   MultiFactorMDP::VarMap& curStates = mdp.getState(); // THIS IS A REFERENCE
   MultiFactorMDP::VarMap postState(curStates.begin(),curStates.end());
   MultiFactorMDP::VarMap priorState;
   MultiFactorMDP::VarMap action;
   std::map<maxsum::FactorID,double> reward;

   //***************************************************************************
   // Create Writer for writing simulation results to CSV file.
   //***************************************************************************
   CSVWriter writer(argv[1]);

   //***************************************************************************
   // Simulate planner interacting with environment for a number of timesteps
   //***************************************************************************
   double meanReward = 0.0;
   int nExploratoryMoves = 0;
   int timesteps = 4000;
   for(int i=0; i<timesteps; i++)
   {
      std::cout << "ITERATION: " << i << std::endl;
      //************************************************************************
      // Swap the states so that the old post state becomes the new prior state
      //************************************************************************
      std::cout << "swapping states" << std::endl;
      postState.swap(priorState);

      //************************************************************************
      // Get the learner to perform an action
      //************************************************************************
      std::cout << "choosing action" << std::endl;
      int maxsumIterations = learner.act(priorState,action);
      std::cout << "maxsum iterations: " << maxsumIterations << std::endl;
      postState=mdp.getState();
      std::cout << "acting" << std::endl;
      meanReward += mdp.act(action,reward);

      //************************************************************************
      // Update statistics. Note - if the number of max-sum iterations is 0,
      // then this indicates an exploration move.
      //************************************************************************
      if(0==maxsumIterations)
      {
         ++nExploratoryMoves;
      }

      //************************************************************************
      // Let the learner observe the received reward
      //************************************************************************
      std::cout << "observing" << std::endl;
      learner.observe(priorState,action,postState,reward);
      writer.write(priorState,action,postState,reward,0==maxsumIterations);

   } // for loop
   meanReward /= timesteps;
   std::cout << "DONE meanReward: " << meanReward
      << " Number of exploratory moves: " << nExploratoryMoves << std::endl;
   
   //***************************************************************************
   // Check for convergence by perform a few greedy actions - to be optimal
   // we should always get reward 20.
   //***************************************************************************
   std::cout << "Checking convergence..." << std::endl;
   for(int i=0; i<100; ++i)
   {
      std::cout << "CONVERGENCE ITERATION: " << i << std::endl;
      //************************************************************************
      // Swap the states so that the old post state becomes the new prior state
      //************************************************************************
      std::cout << "swapping states" << std::endl;
      postState.swap(priorState);

      //************************************************************************
      // Get the learner to perform an action
      //************************************************************************
      std::cout << "choosing action" << std::endl;
      int maxsumIterations = learner.actGreedy(priorState,action);
      std::cout << "maxsum iterations: " << maxsumIterations << std::endl;
      postState=mdp.getState();
      std::cout << "acting" << std::endl;
      double totReward = mdp.act(action,reward);
      std::cout << "totReward: " << totReward << std::endl;
      writer.write(priorState,action,postState,reward,0==maxsumIterations);

   } // for loop

   //***************************************************************************
   // Return success if mean reward is high enough to indicate convergence
   //***************************************************************************
   return EXIT_SUCCESS;
}

