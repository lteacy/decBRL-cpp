/**
 * A set of wrapper functions for random number generation.
 * These allow us to keep coupling to a minimum. For example
 * we may want to use Matlab generators when compiling mex functions,
 * but boost/random for other purposes.
 */

#include <ctime>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "random.h"

/**
 * Module namespace defines objects used for random
 * number generation.
 */
namespace
{
   boost::random::mt19937 gen_m;

} // module namespace

/**
 * Initialise random generator using time dependent seed.
 */
void mdp_sim::random::initRandomEngineByTime()
{
   gen_m.seed(std::time(0));
}

/**
 * Generate integer from uniform distribution
 * over closed interval [min, max]
 * @param min minimum generated value
 * @param max maximum generated value
 * @return random integer x, s.t. min <= x <= max
 */
int mdp_sim::random::unidrnd(int min, int max)
{
   boost::random::uniform_int_distribution<> dist(min, max);
   return dist(gen_m);
}


