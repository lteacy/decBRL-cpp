#ifndef DEC_BRL_LEARNING_SOLVER_H
#define DEC_BRL_LEARNING_SOLVER_H

#include "dec_brl/random.h"
#include "MaxSumController.h"
#include <set>
#include <list>
#include <algorithm>

namespace dec_brl {

/**
 * Approximate Factored MDP Solver, which uses Reinforcement Learning to
 * estimate the optimal policy.
 * @tparam type of reinforcement learner used for finding solution.
 */
template<class Learner> class LearningSolver
{
private:

   /**
    * Gamma parameter.
    * The discount factor for future rewards.
    * Must be in open range (0,1).
    */
   double gamma_i;

   /**
    * Specifies the variables that we think are actions.
    * Essentially, all variables, that were not specified as states
    * during the first call to act
    */
   std::list<maxsum::VarID> actionSet_i;

   /**
    * Convenience type def for Factor maps
    */
   typedef std::map<maxsum::FactorID, maxsum::DiscreteFunction> FactorMap;

public:

}; // class LearningSolver

} // namespace dec_brl

#endif // DEC_BRL_LEARNING_SOLVER_H

