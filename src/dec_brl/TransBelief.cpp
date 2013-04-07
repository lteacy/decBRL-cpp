/**
 * @file TransBelief.cpp
 * Defines class for representing Bayesian beliefs about factored transition
 * probabilities using Diriclet conjugate priors.
 */

#include <dec_brl/TransBelief.h>

namespace dec_brl {
    
    
    /**
     * Produce text representation of beliefs for diagnostics.
     */
    std::ostream& operator<<(std::ostream& out, const TransBelief& beliefs)
    {
        Eigen::MatrixX2i condVars(beliefs.condVars_i.size(),2);
        Eigen::MatrixX2i domainVars(beliefs.domainVars_i.size(),2);
        condVars.col(0) = beliefs.condVars_i;
        condVars.col(1) = beliefs.condSize_i;
        domainVars.col(0) = beliefs.domainVars_i;
        domainVars.col(1) = beliefs.domainSize_i;
        out << "Condition Variables, Size\n" << condVars << std::endl;
        out << "Domain Variables, Size\n" << domainVars << std::endl;
        out << "Hyperparams:\n" << beliefs.alpha_i << std::endl;
        return out;
    }
    
} // namespace dec_brl