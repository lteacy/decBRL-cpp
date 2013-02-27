#include <iostream>
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
class SingleFactorMDP
{
public:

   /**
    * Map type used to pass action and state values around.
    */
   typedef std::map<maxsum::VarID,maxsum::ValIndex> VarMap;

private:

   /**
    * The reward function for this MDP.
    */
   maxsum::DiscreteFunction rewardFunc_i;

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
    * Variable ID for single state variable.
    */
   const static int STATE_ID = 1;

   /**
    * Variable ID for single action variable.
    */
   const static int ACTION_ID = 2;

   /**
    * ID for single Factor in the mdp.
    */
   const static int FACTOR_ID = 1;

   /**
    * Number of possible states.
    */
   const static int NUM_STATE_VALS = 2;

   /**
    * Number of possible actions
    */
   const static int NUM_ACTION_VALS = 2;

   /**
    * Default Constructor.
    */
   SingleFactorMDP() : rewardFunc_i(), state_i(), lastAction_i()
   {
      //************************************************************************
      // Register the state and action variables with the maxsum library
      //************************************************************************
      maxsum::registerVariable(STATE_ID,NUM_STATE_VALS);
      maxsum::registerVariable(ACTION_ID,NUM_ACTION_VALS);

      //************************************************************************
      // Define the reward function, such that a positive reward is received
      // only if the last action the current state.
      //************************************************************************
      rewardFunc_i.expand(STATE_ID);
      rewardFunc_i.expand(ACTION_ID);
      rewardFunc_i(0,0) = 20;
      rewardFunc_i(0,1) = 5;
      rewardFunc_i(1,0) = 5;
      rewardFunc_i(1,1) = 20;

      //************************************************************************
      // Set the current state. Note: we don't set the last action value
      // because no action has yet been performed.
      //************************************************************************
      state_i[STATE_ID] = 0;

   } // default constructor.

   /**
    * Inform learner of the factors defined by this MDP.
    */
   template<class Learner> void addFactors(Learner& learner)
   {
      //************************************************************************
      // Register a single factor that depends on the single action and state
      // variables only.
      //************************************************************************
      const int varIds[] = {STATE_ID, ACTION_ID};
      learner.addFactor(FACTOR_ID,varIds,varIds+2);

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
    */
   double act(VarMap& action)
   {
      //************************************************************************
      // Set the last action to the one being performed.
      //************************************************************************
      lastAction_i[ACTION_ID] = action[ACTION_ID];

      //************************************************************************
      // Alternative between states
      //************************************************************************
      state_i[STATE_ID] = (1+state_i[STATE_ID]) % 2;

      //************************************************************************
      // Return the reward value for the current state and last acton.
      //************************************************************************
      return rewardFunc_i(state_i[STATE_ID],lastAction_i[ACTION_ID]);

   } // method act

}; // class SingleFactorMDP

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
      out_i << "PriorState,Action,PostState,Reward,isExploratory\n";
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
      out_i << priorStates[SingleFactorMDP::STATE_ID] << ',';
      out_i << actions[SingleFactorMDP::ACTION_ID] << ',';
      out_i << postStates[SingleFactorMDP::STATE_ID] << ',';
      out_i << rewards[SingleFactorMDP::FACTOR_ID] << ',';
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
   SingleFactorMDP mdp;

   //***************************************************************************
   // Create learner
   //***************************************************************************
   std::cout << "Constructing Learner" << std::endl;
   DecQLearner learner;
   std::cout << "Adding Factors to Learner" << std::endl;
   mdp.addFactors(learner);

   //***************************************************************************
   // Define some maps to old the factor rewards, states and actions.
   // Note that the postState immediately becomes the priorState in the first
   // iteration of the for loop below.
   //***************************************************************************
   std::cout << "Constructing state, reward and action maps" << std::endl;
   SingleFactorMDP::VarMap& curStates = mdp.getState(); // THIS IS A REFERENCE
   SingleFactorMDP::VarMap postState(curStates.begin(),curStates.end());
   SingleFactorMDP::VarMap priorState;
   SingleFactorMDP::VarMap action;
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
      postState[SingleFactorMDP::STATE_ID]=curStates[SingleFactorMDP::STATE_ID];
      std::cout << "acting" << std::endl;
      reward[SingleFactorMDP::FACTOR_ID] = mdp.act(action);

      //************************************************************************
      // Update statistics. Note - if the number of max-sum iterations is 0,
      // then this indicates an exploration move.
      //************************************************************************
      if(0==maxsumIterations)
      {
         ++nExploratoryMoves;
      }
      meanReward += reward[SingleFactorMDP::FACTOR_ID];

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
   for(int i=0; i<10; ++i)
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
      postState[SingleFactorMDP::STATE_ID]=curStates[SingleFactorMDP::STATE_ID];
      std::cout << "acting" << std::endl;
      double reward = mdp.act(action);

      //************************************************************************
      // Ensure that received reward is always max (implies convergence to 
      // optimal policy)
      //************************************************************************
      if(20.0 > reward)
      {
         std::cout << "Non optimal reward after learning: " << reward
            << std::endl;
         return EXIT_FAILURE;
      }

   } // for loop

   //***************************************************************************
   // Return success if mean reward is high enough to indicate convergence
   //***************************************************************************
   return EXIT_SUCCESS;
}

