#ifndef DEC_BRL_DEC_Q_LEARNER_H
#define DEC_BRL_DEC_Q_LEARNER_H

#include "dec_brl/random.h"
#include "MaxSumController.h"
#include <set>
#include <list>
#include <algorithm>

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
    * Specifies the variables that we think are actions.
    * Essentially, all variables, that were not specified as states
    * during the first call to act
    */
   std::list<maxsum::VarID> actionSet_i;

   /**
    * True iff this object is fully initialised.
    * At the moment, that means that act as been called at least once.
    */
   bool isInitialised_i;

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
    * Default Constructor.
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
   : alpha_i(alpha), gamma_i(gamma), epsilon_i(epsilon),
     maxsum_i(maxIterations,maxnorm), actionSet_i(), isInitialised_i(false),
     qValues_i()
   {}

   /**
    * (Deep) Copy constructor.
    */
   DecQLearner(const DecQLearner& rhs)
   : alpha_i(rhs.alpha_i), gamma_i(rhs.gamma_i), epsilon_i(rhs.epsilon_i),
     maxsum_i(rhs.maxsum_i), actionSet_i(rhs.actionSet_i), 
     isInitialised_i(rhs.isInitialised_i), qValues_i(rhs.qValues_i)
   {}

   /**
    * (Deep) Copy assignment.
    */
   DecQLearner& operator=(const DecQLearner& rhs)
   {
      alpha_i = rhs.alpha_i;
      gamma_i = rhs.gamma_i;
      epsilon_i = rhs.epsilon_i;
      maxsum_i = rhs.maxsum_i;
      actionSet_i = rhs.actionSet_i;
      isInitialised_i = rhs.isInitialised_i;
      qValues_i = rhs.qValues_i;
      return *this;
   }

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
    VarIt varEnd
   )
   {
      //************************************************************************
      // Map the specified Q-value factor to a function that depends only on
      // the specified list of variables. All values are initially zero.
      //************************************************************************
      qValues_i[factor] = maxsum::DiscreteFunction(varBegin,varEnd,0.0);

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
      // If this is the first call to act, construct the action set, from the
      // combined domain of all factors minus the specified states.
      //************************************************************************
      if(!isInitialised_i)
      {
         //*********************************************************************
         // Construct set of all variables
         //*********************************************************************
         std::set<maxsum::VarID> allVars;
         for(int k=0; k<qValues_i.size(); ++k)
         {
            const maxsum::DiscreteFunction& fun = qValues_i[k];
            allVars.insert(fun.varBegin(),fun.varEnd());
         }

         //*********************************************************************
         // Construct set of all states
         //*********************************************************************
         std::set<maxsum::VarID> stateSet;
         for(typename StateMap::const_iterator it=states.begin();
               it!=states.end(); ++it)
         {
            stateSet.insert(it->first);
         }

         //*********************************************************************
         // The action set is then the set difference.
         // Apparently, the set_difference ensures that actionSet_i will be
         // sorted (assuming all lists are initially sorted).
         //*********************************************************************
         std::set_difference(allVars.begin(),allVars.end(),
               stateSet.begin(),stateSet.end(),actionSet_i.begin());

         //*********************************************************************
         // Make sure we only do this once
         //*********************************************************************
         isInitialised_i = true;

      } // if statement

      //************************************************************************
      // Flip a coin to decide whether to explore (with probablity epsilon)
      // or to exploit by acting greedily w.r.t. to current estimate.
      //************************************************************************
      bool doExplore = random::unirnd()<=epsilon_i;

      //************************************************************************
      // If this is an exploratory move, just choose random actions
      //************************************************************************
      if(doExplore)
      {
         for(std::list<maxsum::VarID>::const_iterator it=actionSet_i.begin();
               it!=actionSet_i.end(); ++it)
         {
            maxsum::VarID curAction = *it;
            int domainSize = maxsum::getDomainSize(curAction);
            actions[curAction] = random::unidrnd(0,domainSize-1);
         }
         return;
      }

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
         //curFactor(allVars) = curReward; // assuming we implement element access via pair collections

   } // observe

}; // class DecQLearner

/**
 * Default weight to place in new reward estimates.
 */
const double DecQLearner::DEFAULT_ALPHA = 0.1;

/**
 * Default MDP discount factor for future rewards.
 */
const double DecQLearner::DEFAULT_GAMMA = 0.95;

/**
 * Default Probability of choosing explortory (random) actions.
 */
const double DecQLearner::DEFAULT_EPSILON = 0.1;

} // namespace dec_brl

#endif // DEC_BRL_DEC_Q_LEARNER_H

