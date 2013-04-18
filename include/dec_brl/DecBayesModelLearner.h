#ifndef DEC_BRL_DEC_BAYES_MODEL_LEARNER_H
#define DEC_BRL_DEC_BAYES_MODEL_LEARNER_H

#include "dec_brl/LearnerBase.h"
#include "dec_brl/random.h"
#include "dec_brl/NormalGamma.h"
#include "dec_brl/vpi.h"
#include "dec_brl/util.h"
#include "MaxSumController.h"
#include <set>
#include <list>
#include <algorithm>

namespace dec_brl {

/**
 * Implements a factored a decentralised model based Bayesian Reinforcement
 * learner.
 * @tparam MDPSolver type used to solve factored MDPs in subroutines.
 */
template<class MDPSolver> class DecBayesModelLearner : public LearnerBase<>
{
private:

   /**
    * Object used to solve factored MDPs.
    */
   MDPSolver solver_i;

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
    * Convenience type def for a reward belief distribution.
    * This is a normal gamma distribution defined for each element of a
    * DiscreteFunction.
    */
   typedef dist::NormalGamma_Tmpl<maxsum::DiscreteFunction> RewardDist;

   /**
    * Convenience type def for reward belief maps
    */
   typedef std::map<maxsum::FactorID, RewardDist> RewardBeliefMap;

   /**
    * Estimated rewards stored as DiscreteFunctions.
    */
   RewardBeliefMap rewardBeliefs_i;

public:

   /**
    * Default Constructor.
    */
   DecBayesModelLearner
   (
    double gamma=DEFAULT_GAMMA,
    MDPSolver solver=MDPSolver(),
    int maxIterations=maxsum::MaxSumController::DEFAULT_MAX_ITERATIONS,
    maxsum::ValType maxnorm=maxsum::MaxSumController::DEFAULT_MAXNORM_THRESHOLD
   )
   : solver_i(solver), gamma_i(gamma), 
     maxsum_i(maxIterations,maxnorm), actionSet_i(), isInitialised_i(false),
     rewardBeliefs_i()
   {}

   /**
    * (Deep) Copy constructor.
    */
   DecBayesModelLearner(const DecBayesModelLearner& rhs)
   : solver_i(rhs.solver_i), gamma_i(rhs.gamma_i), 
     maxsum_i(rhs.maxsum_i), actionSet_i(rhs.actionSet_i), 
     isInitialised_i(rhs.isInitialised_i), rewardBeliefs_i(rhs.rewardBeliefs_i)
   {}

   /**
    * (Deep) Copy assignment.
    */
   DecBayesModelLearner& operator=(const DecBayesModelLearner& rhs)
   {
      solver_i = rhs.solver_i;
      gamma_i = rhs.gamma_i;
      maxsum_i = rhs.maxsum_i;
      actionSet_i = rhs.actionSet_i;
      isInitialised_i = rhs.isInitialised_i;
      rewardBeliefs_i = rhs.rewardBeliefs_i;
      return *this;
   }

   /**
    * Adds a reward factor to the factor graph.
    * Adds a factored reward to the factor graph, given a specified unique
    * factor id, and a list of variable ids on which this factored depends.
    * At this point, we do not distinguish between state and action variables.
    * The Learner assumes that states variables are those passed to the act
    * and observe functions, while action variables are any other variables on
    * which the reward factored depend.
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
      // Initialise a distribution using default hyperparameters.
      // This is done by std::map implicitly calling the NormalGamma default
      // constructor the new factor. 
      //************************************************************************
      RewardDist& dist = rewardBeliefs_i[factor];
      
      //************************************************************************
      // Sanity check to make sure the factor does not already exist,
      // otherwise we might get unexpected behaviour.
      //************************************************************************
      assert(0==dist.alpha.noVars());
      assert(0==dist.beta.noVars());
      assert(0==dist.lambda.noVars());
      assert(0==dist.m.noVars());

      //************************************************************************
      // Expand the distribution domain to the required variables.
      // This will copy the default hyperparameter values for each joint
      // state-action in the Q factor's domain.
      //************************************************************************
      dist.alpha.expand(varBegin,varEnd);
      dist.beta.expand(varBegin,varEnd);
      dist.lambda.expand(varBegin,varEnd);
      dist.m.expand(varBegin,varEnd);
      
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
      for(RewardBeliefMap::const_iterator it=rewardBeliefs_i.begin();
            it!=rewardBeliefs_i.end(); ++it)
      {
         const RewardDist& fun = it->second;
         allVars.insert(fun.alpha.varBegin(),fun.alpha.varEnd());
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
    * Return the next actions selected by the Learner.
    * This is equivalent to the act member function, except that
    * actions are always selected greedily w.r.t to the current reward
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
      // Condition the MaxSumController on the current states and expected
      // rewards. Note that the expected rewards are equal to the 'm'
      // hyperparameter of the NormalGamma distribution.
      //************************************************************************
      maxsum::DiscreteFunction curFactor;
      for(RewardBeliefMap::const_iterator it=rewardBeliefs_i.begin();
            it!=rewardBeliefs_i.end(); ++it)
      {
         maxsum::condition(it->second.m,curFactor,states);
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
    * Return the next actions selected by the Learner
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
      // Also initialise the maxsum controller
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

         //*********************************************************************
         // Condition the MaxSumController on the current states and expected
         // rewards. Note that the expected rewards are equal to the 'm'
         // hyperparameter of the NormalGamma distribution.
         //*********************************************************************
         maxsum::DiscreteFunction curFactor;
         for(RewardBeliefMap::const_iterator it=rewardBeliefs_i.begin();
               it!=rewardBeliefs_i.end(); ++it)
         {
            maxsum::condition(it->second.m,curFactor,states);
            maxsum_i.setFactor(it->first,curFactor);
         }

      } 
      //************************************************************************
      // If we've already initialised, then we modify the maxsum's factor
      // the fast way.
      //************************************************************************
      else
      {
         //*********************************************************************
         // Condition the MaxSumController on the current states and expected
         // rewards. Note that the expected rewards are equal to the 'm'
         // hyperparameter of the NormalGamma distribution.
         //*********************************************************************
         for(RewardBeliefMap::const_iterator it=rewardBeliefs_i.begin();
               it!=rewardBeliefs_i.end(); ++it)
         {
            maxsum::DiscreteFunction& curFactor = 
               maxsum_i.getUnSafeWritableFactorHandle(it->first);

            maxsum::condition(it->second.m,curFactor,states);
            maxsum_i.notifyFactor(it->first); // notify maxsum of change 

         } // for
      } // else

      //************************************************************************
      // Run max-sum to calculate each factor's total local value
      // (sum of factor plus its received messages).
      //************************************************************************
      int msIterationCount = maxsum_i.optimise();

      //************************************************************************
      // For each factor 
      //************************************************************************
      for(RewardBeliefMap::const_iterator it=rewardBeliefs_i.begin();
            it!=rewardBeliefs_i.end(); ++it)
      {
         const maxsum::FactorID factor = it->first;
         const RewardDist& valDist = it->second;

         //*********************************************************************
         // Construct the belief distribution over the local combined value
         // This is the same as the local belief distribution, except it is
         // conditioned on the current state, and the mean is shifted to include
         // the messages past from all neighbouring nodes.
         //*********************************************************************
         RewardDist totValDist;
         totValDist.m = maxsum_i.getTotalValue(factor);
         maxsum::condition(valDist.alpha,totValDist.alpha,states);
         maxsum::condition(valDist.beta,totValDist.beta,states);
         maxsum::condition(valDist.lambda,totValDist.lambda,states);

         //*********************************************************************
         // Calculate local vpi for current state
         //*********************************************************************
         maxsum::DiscreteFunction localVPI;
         exactVPI(totValDist, localVPI);

         //*********************************************************************
         // Add VPI to expected local Q - which is already stored in 
         // maxsum controller
         //*********************************************************************
         maxsum_i.getUnSafeWritableFactorHandle(factor) += localVPI;
         maxsum_i.notifyFactor(factor); // notify maxsum of change to factor

      } // for loop

      //************************************************************************
      // Run maxsum again to optimise w.r.t. to combined value.
      //************************************************************************
      msIterationCount += maxsum_i.optimise();

      //************************************************************************
      // Populate the action map with the optimised actions.
      //************************************************************************
      actions.clear();
      actions.insert(maxsum_i.valBegin(),maxsum_i.valEnd());

      //************************************************************************
      // For diagnostic purposes, also return number of max-sum iterations.
      //************************************************************************
      return msIterationCount;

   } // act

   /**
    * Update model based on observed rewards.
    * Updates the factored beliefs for given observed factored rewards, and
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
    * reward factors.
    * @post reward distributions will be updated according to observed
    * factored rewards.
    */
   template<class RewardMap, class VarMap> void observe
   (
    const VarMap& priorStates,
    const VarMap& actions,
    const VarMap& postStates,
    const RewardMap& rewards
   )
   {
      using namespace maxsum;
      
      //************************************************************************
      // Take the union of the previous states and the last set of actions.
      // This specifies which rewards need to be updated.
      //************************************************************************
      std::map<VarID,ValIndex> priorVars;
      priorVars.insert(priorStates.begin(),priorStates.end());
      priorVars.insert(actions.begin(),actions.end());

      //************************************************************************
      // Choose greedy actions w.r.t. to current states. These are used to
      // perform the maximisation step in the update.
      //************************************************************************
      std::map<VarID,ValIndex> postVars;
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
         // Find the corresponding factored q-value belief distribution
         //*********************************************************************
         RewardBeliefMap::iterator qPos = rewardBeliefs_i.find(it->first);

         //*********************************************************************
         // If we can't find this factor, go on to the next one
         //*********************************************************************
         if(rewardBeliefs_i.end()==qPos)
         {
            continue;
         }
         
         //*********************************************************************
         // Retrieve the hyperparameters for the next local reward
         //*********************************************************************
         RewardDist& dist = qPos->second;
         const ValType nxtAlpha = dist.alpha(postVars);
         const ValType nxtBeta = dist.beta(postVars);
         const ValType nxtLambda = dist.lambda(postVars);
         const ValType nxtM = dist.m(postVars);
         
         //*********************************************************************
         // Calculate the required moments
         //*********************************************************************
         const ValType r = it->second;
         const ValType expSigma2 = nxtBeta/(nxtAlpha-1);
         const ValType expR2 = nxtM*nxtM + (1+1/nxtLambda)*expSigma2;
         const ValType expQ  = r + gamma_i*nxtM;
         const ValType expQ2 = r*r + 2*gamma_i*r*nxtM + gamma_i*gamma_i*expR2;

         //*********************************************************************
         // Find the corresponding linear index for the current reward
         // distribution, and use the calculated moments to update it.
         //*********************************************************************
         dist::observe<std::map<VarID,ValIndex>&>(dist,priorVars,expQ,expQ2,1);

      } // for loop

   } // observe

}; // class DecBayesModelLearner

} // namespace dec_brl

#endif // DEC_BRL_DEC_BAYES_MODEL_LEARNER_H

