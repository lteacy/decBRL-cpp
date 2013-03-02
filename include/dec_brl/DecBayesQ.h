#ifndef DEC_BRL_DEC_BAYES_Q_H
#define DEC_BRL_DEC_BAYES_Q_H

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
class DecBayesQ
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
    * Convenience type def for Factor maps
    */
   typedef std::map<maxsum::FactorID, maxsum::DiscreteFunction> FactorMap;

   /**
    * Estimated Q-values stored as DiscreteFunctions.
    */
   FactorMap qValues_i;

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
    * Default Constructor.
    */
   DecBayesQ
   (
    double alpha=DEFAULT_ALPHA,
    double gamma=DEFAULT_GAMMA,
    int maxIterations=maxsum::MaxSumController::DEFAULT_MAX_ITERATIONS,
    maxsum::ValType maxnorm=maxsum::MaxSumController::DEFAULT_MAXNORM_THRESHOLD
   )
   : alpha_i(alpha), gamma_i(gamma), 
     maxsum_i(maxIterations,maxnorm), actionSet_i(), isInitialised_i(false),
     qValues_i()
   {}

   /**
    * (Deep) Copy constructor.
    */
   DecBayesQ(const DecBayesQ& rhs)
   : alpha_i(rhs.alpha_i), gamma_i(rhs.gamma_i), 
     maxsum_i(rhs.maxsum_i), actionSet_i(rhs.actionSet_i), 
     isInitialised_i(rhs.isInitialised_i), qValues_i(rhs.qValues_i)
   {}

   /**
    * (Deep) Copy assignment.
    */
   DecBayesQ& operator=(const DecBayesQ& rhs)
   {
      alpha_i = rhs.alpha_i;
      gamma_i = rhs.gamma_i;
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
    * Tells this learner which variables to treat as states. State variables
    * are not max marginalised, and must have assigned values passed into
    * the act member function. This function is called 'Just In Time' by
    * act function, but may be called before hand to reduce computational
    * overhead when choosing the first action. This function should
    * only be called at most once: after construction, but before the first
    * call to act.
    * @param[in] stateBegin iterator to the start of the set of states.
    * @param[in] stateEnd iterator to the end of the set of states.
    * @pre the set of states specified by begin and end must be sorted in
    * ascending order. 
    */
   template<class Iterator> void setStates
   (
    Iterator stateBegin,
    Iterator stateEnd
   )
   {
      //************************************************************************
      // Only proceed if this Learner has not already been initialised.
      //************************************************************************
      if(isInitialised_i)
      {
         return;
      }

      //************************************************************************
      // Construct set of all variables
      //************************************************************************
      std::set<maxsum::VarID> allVars;
      for(FactorMap::const_iterator it=qValues_i.begin();
            it!=qValues_i.end(); ++it)
      {
         const maxsum::DiscreteFunction& fun = it->second;
         allVars.insert(fun.varBegin(),fun.varEnd());
      }

      //************************************************************************
      // The action set is then the set difference.
      // Apparently, the set_difference ensures that actionSet_i will be
      // sorted (assuming all lists are initially sorted).
      //************************************************************************
      std::set_difference(allVars.begin(), allVars.end(), stateBegin, stateEnd,
            std::inserter(actionSet_i, actionSet_i.begin()));

      //************************************************************************
      // Make sure we only do this once
      //************************************************************************
      isInitialised_i = true;

   } // setStates function

   /**
    * Return the next actions selected by the Q-Learner.
    * This is equivalent to the act member function, except that
    * actions are always selected greedily w.r.t to the current Q-value
    * estimate, and so exploration is never performed.
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
    * @returns the number of max-sum iterations performs (0 means this was an
    * exploratory move).
    */
   template<class ActionMap, class StateMap> int actGreedy
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
         // Construct set of all states
         //*********************************************************************
         std::set<maxsum::VarID> stateSet;
         for(typename StateMap::const_iterator it=states.begin();
               it!=states.end(); ++it)
         {
            stateSet.insert(it->first);
         }

         //*********************************************************************
         // Call setStates function to do the hard work.
         //*********************************************************************
         setStates(stateSet.begin(),stateSet.end());

      } // if statement

      //************************************************************************
      // Condition the MaxSumController on the current states.
      //************************************************************************
      maxsum::DiscreteFunction curFactor;
      for(FactorMap::const_iterator it=qValues_i.begin();
            it!=qValues_i.end(); ++it)
      {
         maxsum::condition(it->second,curFactor,states);
         maxsum_i.setFactor(it->first,curFactor);
      }

      //************************************************************************
      // Run max-sum to optimise the set of actions
      //************************************************************************
      int msIterationCount = maxsum_i.optimise();

      //************************************************************************
      // Populate the action map with the optimised actions.
      //************************************************************************
      actions.clear();
      actions.insert(maxsum_i.valBegin(),maxsum_i.valEnd());

      //************************************************************************
      // For diagnostic purposes, also return number of max-sum iterations.
      //************************************************************************
      return msIterationCount;

   } // actGreedy function

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
    * @returns the number of max-sum iterations performs (0 means this was an
    * exploratory move).
    */
   template<class ActionMap, class StateMap> int act
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
         // Construct set of all states
         //*********************************************************************
         std::set<maxsum::VarID> stateSet;
         for(typename StateMap::const_iterator it=states.begin();
               it!=states.end(); ++it)
         {
            stateSet.insert(it->first);
         }

         //*********************************************************************
         // Call setStates function to do the hard work.
         //*********************************************************************
         setStates(stateSet.begin(),stateSet.end());

      } // if statement

      //************************************************************************
      // Flip a coin to decide whether to explore (with probablity epsilon)
      // or to exploit by acting greedily w.r.t. to current estimate.
      //************************************************************************
      bool doExplore = true;

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

         //*********************************************************************
         // Return zero for exploratory moves, because no max-sum iterations
         // have been performed.
         //*********************************************************************
         return 0;
      }

      //************************************************************************
      // Otherwise act greedily
      //************************************************************************
      return actGreedy(states,actions);

   } // act

   /**
    * Update Q-Value estimates best on estimates.
    * Updates the factored Q-Values for given observed factored rewards, and
    * given successor states, assuming that the last states and actions where
    * as defined immediately after the last call to the act() function.
    * @tparam RewardMap maps maxsum::FactorID to rewards (double)
    * @tparam VarMap maps maxsum::VarID to maxsum::ValIndex
    * @param priorStates map of all state values immediately before performing
    * specified actions.
    * @param actions map of all performed action values.
    * @param postStates map of all state values immediately after performing
    * specified actions.
    * @param rewards map of all observed rewards to their corresponding
    * Q-value factors.
    * @post Q-value estimates will be updated according to observed factored
    * rewards.
    */
   template<class RewardMap, class VarMap> void observe
   (
    const VarMap& priorStates,
    const VarMap& actions,
    const VarMap& postStates,
    const RewardMap& rewards
   )
   {
      //************************************************************************
      // Take the union of the previous states and the last set of actions.
      // This specifies which Q-values need to be updated.
      //************************************************************************
      std::map<maxsum::VarID,maxsum::ValIndex> priorVars;
      priorVars.insert(priorStates.begin(),priorStates.end());
      priorVars.insert(actions.begin(),actions.end());

      //************************************************************************
      // Choose greedy actions w.r.t. to current states. These are used to
      // perform the maximisation step in the update.
      //************************************************************************
      std::map<maxsum::VarID,maxsum::ValIndex> postVars;
      actGreedy(postStates,postVars);

      //************************************************************************
      // Bundle the next states in with the greedy next actions. Again, this
      // is for the maximisation step - specifying the s' and a' together for
      // finding the value of Q(s',a').
      //************************************************************************
      postVars.insert(postStates.begin(),postStates.end());

      //************************************************************************
      // For each observed reward 
      //************************************************************************
      typedef typename RewardMap::const_iterator Iterator;
      for(Iterator it=rewards.begin(); it!=rewards.end(); ++it)
      {
         //*********************************************************************
         // Find the corresponding factored q-value
         //*********************************************************************
         FactorMap::iterator qPos = qValues_i.find(it->first);

         //*********************************************************************
         // If we can't find this factor, go on to the next one
         //*********************************************************************
         if(qValues_i.end()==qPos)
         {
            continue;
         }

         //*********************************************************************
         // Update the estimate with the current reward:
         // Q(s,a) = (1-alpha)*Q(s,a) + alpha*(r + gamma*Q(s',a') )
         //*********************************************************************
         maxsum::ValType& priorQ = qPos->second(priorVars);
         const maxsum::ValType postQ = qPos->second(postVars);
         const maxsum::ValType curReward = it->second;
         const maxsum::ValType update = curReward + gamma_i*postQ;
         priorQ = (1.0-alpha_i)*priorQ + alpha_i*update;

      } // for loop

   } // observe

}; // class DecBayesQ

/**
 * Default weight to place in new reward estimates.
 */
const double DecBayesQ::DEFAULT_ALPHA = 0.1;

/**
 * Default MDP discount factor for future rewards.
 */
const double DecBayesQ::DEFAULT_GAMMA = 0.95;

} // namespace dec_brl

#endif // DEC_BRL_DEC_BAYES_Q_H

