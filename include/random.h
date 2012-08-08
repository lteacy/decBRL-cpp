#ifndef MDP_SIM_RANDOM_H
#define MDP_SIM_RANDOM_H

/**
 * A set of wrapper functions for random number generation.
 * These allow us to keep coupling to a minimum. For example
 * we may want to use Matlab generators when compiling mex functions,
 * but boost/random for other purposes.
 */
namespace mdp_sim
{
   namespace random
   {

      /**
       * Initialise random generator using time dependent seed.
       */
      void initRandomEngineByTime();

      /**
       * Generate integer from uniform distribution
       * over closed interval [min, max]
       * @param min minimum generated value
       * @param max maximum generated value
       * @return random integer x, s.t. min <= x <= max
       */
      int unidrnd(int min, int max);  

   } // namespace random

} // namespace mdp_sim

#endif // MDP_SIM_RANDOM_H
