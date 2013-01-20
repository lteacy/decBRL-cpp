#ifndef DEC_BRL_DEC_Q_LEARNER_H
#define DEC_BRL_DEC_Q_LEARNER_H

#include "random.h"
#include "MaxSumController.h"

namespace dec_brl {

/**
 * Implements a factored Q Learning policy using maxsum and e-greedy
 * exploration. 
 */
class DecQLearner
{
private:

   /**
    * Alpha parameter.
    * The weight to place in new reward estimates.
    */
   double alpha_i;

   /**
    * Gamma parameter.
    * The discount factor for future rewards.
    * Must be in open range (0,1).
    */
   double gamma_i;

   /**
    * Epsilon parameter.
    * Probability for choosing an exploratory action.
    */
   double epsilon_i;

   /**
    * MaxSumController used to choose best action.
    */
   maxsum::MaxSumController maxsum_i;

   /**
    * Estimated Q-values stored as DiscreteFunctions.
    */
   std::vector<maxsum::DiscreteFunction> qValues_i;

public:

   /**
    * Default weight to place in new reward estimates.
    */
   static const double DEFAULT_ALPHA;

   /**
    * Default MDP discount factor for future rewards.
    */
   static const double DEFAULT_GAMMA;

   /**
    * Default Probability of choosing explortory (random) actions.
    */
   static const double DEFAULT_EPSILON;

   /**
    * Constructor.
    * 
    */
   DecQLearner
   (
    double alpha=DEFAULT_ALPHA,
    double gamma=DEFAULT_GAMMA,
    double epsilon=DEFAULT_EPSILON,
    int maxIterations=maxsum::MaxSumController::DEFAULT_MAX_ITERATIONS,
    maxsum::ValType maxnorm=maxsum::MaxSumController::DEFAULT_MAXNORM_THRESHOLD
   )
   : alpha_i(alpha), gamma_i(gamma_i), epsilon_i(epsilon),
     maxsum_i(maxIterations,maxnorm), qValues_i()
   {}

   /**
    * Adds a Q-Value factor to the factor graph.
    * Adds a factored Q-Value to the factor graph, given a specified unique
    * factor id, and a list of variable ids on which this factored depends.
    * At this point, we do not distinguish between state and action variables.
    * The Q Learner assumes that states variables are those passed to the act
    * and observe functions, while action variables are any other variables on
    * which the Q-value factored depend.
    * @tparam VarIt iterator type over list of maxsum::VarID values.
    * @param[in] factor unique ID for this factor
    * @param[in] varBegin iterator to the beginning of list of this factor's
    * variables.
    * @param[in] varEnd iterator to the end of the list of this factor's
    * variables.
    * @pre all specified variables must be pre registered with the maxsum
    * library.
    * @see maxsum::register
    */
   template<class VarIt> void addFactor
   (
    maxsum::FactorID factor,
    VarIt varBegin,
    VarIt varEnd,
   )
   {

   } // addFactor

   /**
    * Return the next actions selected by the Q-Learner
    * Current states are specified in a read-only map, while actions
    * are specified through a writable map passed as a parameter.
    * The minimum requirement for the state and action map types is that
    * they implement operator[], begin(), and end() with the same semantics
    * as std::map<maxsum::VarID,maxsum::ValIndex>. Obviously, this type will
    * do nicely, but the user is free to provide their own compatible type.
    * @tparam ActionMap type of map used to store action selections
    * @tparam StateMap type of map used to store current states
    * @param[in] map of state variable ids to their current values.
    * @param[out] map that will be populated with action values.
    * @pre states contains mapped values for each state.
    * @post each action variable will be mapped to its selected value.
    */
   template<class ActionMap, class StateMap> void act
   (
    const StateMap& states,
    ActionMap& actions
   )
   {
      //************************************************************************
      // Flip a coin to decide whether to explore (with probablity epsilon)
      // or to exploit by acting greedily w.r.t. to current estimate.
      //************************************************************************

      //************************************************************************
      // If this is an exploratory move, just choose random actions
      //************************************************************************

      //************************************************************************
      // Otherwise, condition the MaxSumController on the current states.
      //************************************************************************

      //************************************************************************
      // Run max-sum to optimise the set of actions
      //************************************************************************

      //************************************************************************
      // Populate the action map with the optimised actions.
      //************************************************************************

   } // act

   /**
    * Update Q-Value estimates best on estimates.
    * Updates the factored Q-Values for given observed factored rewards, and
    * given successor states, assuming that the last states and actions where
    * as defined immediately after the last call to the act() function.
    * @tparam RewardMap maps maxsum::FactorID to rewards (double)
    * @tparam StateMap maps maxsum::VarID to maxsum::ValIndex
    * @param states map of all state values immediately after performing
    * the last actions returned by act().
    * @param rewards map of all observed rewards to their corresponding
    * Q-value factors.
    * @post Q-value estimates will be updated according to observed factored
    * rewards.
    */
   template<class RewardMap, class StateMap> void observe
   (
    const StateMap& states,
    const RewardMap& rewards
   )
   {
      //************************************************************************
      // Take the union of the observed states and the last set of actions
      //************************************************************************

      //************************************************************************
      // For each observed reward 
      //************************************************************************

         //*********************************************************************
         // Find the corresponding factored q-value
         //*********************************************************************

         //*********************************************************************
         // If we can't find this factor, go on to the next one
         //*********************************************************************

         //*********************************************************************
         // Update the estimate with the current reward
         //*********************************************************************
         curFactor(allVars) = curReward; // assuming we implement element access via pair collections

   } // observe

}; // class DecQLearner

} // namespace dec_brl

#endif // DEC_BRL_DEC_Q_LEARNER_H

