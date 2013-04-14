/**
 * @file FactoredMDP.h
 * Defines a FactoredMDP that simulate state and rewards given actions, and
 * can be read from a protocol buffer specification.
 */
#ifndef DEC_BRL_FACTORED_MDP_H
#define DEC_BRL_FACTORED_MDP_H

#include <vector>
#include <boost/container/flat_map.hpp>
#include "common.h"
#include "register.h"
#include "DiscreteFunction.h"
#include "TransProb.h"
#include "ProtoMDP.pb.h"
#include "exceptions.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace dec_brl
{
    
    /**
     * Defines a FactoredMDP that simulate state and rewards given actions, and
     * can be read from a protocol buffer specification.
     */
    class FactoredMDP
    {
    private:
        
        /**
         * Type used to store a Factored reward function together with any
         * associated standard deviation.
         * We assume rewards are constant (standard deviation 0) or normally
         * distributed with some non-zero standard deviation.
         */
        struct RewardDist
        {
            maxsum::DiscreteFunction reward;
            maxsum::DiscreteFunction std_dev;
        };
                
        /**
         * Type used to store joint states.
         * (boost flat map is apparently more efficient of all operators,
         *  apart from [worst-case] insertion). Although we modify values
         * frequently, we don't insert new variables after initial construction,
         * so this should do fine.
         */
        typedef boost::container::flat_map<maxsum::VarID,
                                           maxsum::ValIndex> StateMap;
        
        /**
         * Type used to store observed factored rewards.
         */
        typedef boost::container::flat_map<maxsum::FactorID,
                                           maxsum::ValType>  RewardMap;
        
        /**
         * Type of map used to stored factors.
         */
        typedef boost::container::flat_map<maxsum::FactorID,
                                           RewardDist>       FactorMap;
        
        /**
         * Type used to store transition probabilties.
         */
        typedef std::vector<TransProb> FactoredCPT;
        
        /**
         * Protocol buffer representation.
         * We keep this, so that write in back to file along with results later.
         */
        proto::FactoredMDP protoSpec_i;
        
        /**
         * The factored expected reward functions for this MDP.
         */
        FactorMap rewardFactors_i;
        
        /**
         * The factored transition probability CPTs for this MDP.
         */
        FactoredCPT transProbs_i;
        
        /**
         * Discount factor for future rewards.
         */
        double gamma_i;
        
        /**
         * The previous joint state and actions observed for this MDP.
         * Should include prevState_i as a subset.
         */
        StateMap prevVars_i;
        
        /**
         * The previous joint state of this MDP.
         */
        StateMap prevState_i;
        
        /**
         * The current joint state of this MDP.
         */
        StateMap curState_i;
        
        /**
         * The last rewards obtained from this MDP.
         */
        RewardMap lastRewards_i;
        
        /**
         * Adds a new factored reward function based on a protocol buffer
         * specification.
         * @see FactoredMDP::parseFromIStream
         */
        void addReward(const proto::FactoredMDP_Reward& reward)
        {
            //******************************************************************
            //  Get this factor's id
            //******************************************************************
            maxsum::FactorID id = reward.id();
            
            //******************************************************************
            //  Set this factor's domain
            //******************************************************************
            RewardDist& factor = rewardFactors_i[id];
            factor.reward = 0;  // clear any previous values.
            factor.reward.expand(reward.domain().begin(),reward.domain().end());
            
            //******************************************************************
            //  Get this factor's values
            //******************************************************************
            const int nValues = reward.values_size();
            if(nValues != factor.reward.domainSize())
            {
                std::stringstream buf;
                buf << "Reward Factor " << id << " needs ";
                buf << factor.reward.domainSize() << " values ";
                buf << " but " << nValues << " are specified.";
                throw new ProtoException(buf.str());
            }
            
            for(int k=0; k<nValues; ++k)
            {
                factor.reward(k) = reward.values(k);
            }
            
            //******************************************************************
            //  If this factor is constant, set its standard deviation to 0
            //******************************************************************
            const int nStdDev = reward.std_dev_size();
            if(0==nStdDev)
            {
                factor.std_dev = 0.0;
            }
            //******************************************************************
            //  Otherwise we need to make sure we have the right amount
            //******************************************************************
            else if(nValues!=nStdDev)
            {
                std::stringstream buf;
                buf << "Reward Factor has " << nValues << " but ";
                buf << nStdDev << " standard deviations.";
                throw new ProtoException(buf.str());
            }
            //******************************************************************
            //  Otherwise, set its standard deviation
            //******************************************************************
            else
            {
                factor.std_dev = 0.0; // clear any previous value
                factor.std_dev.expand(factor.reward); // match domain
                for(int k=0; k<nStdDev; ++k)
                {
                    factor.std_dev(k) = reward.std_dev(k);
                }
            }
            
        } // function addReward
        
        /**
         * Adds a new factored CPT based on a protocol buffer specification.
         * @see FactoredMDP::parseFromIStream
         */
        void addTransition(const proto::FactoredMDP_TransProb& trans)
        {
            using namespace std;
            
            //******************************************************************
            //  Get the condition and domain variables for this matrix
            //******************************************************************
            vector<int> domain(trans.domain().begin(),trans.domain().end());
            
            vector<int> cond(trans.conditions().begin(),
                             trans.conditions().end());
            
            //******************************************************************
            //  Calculate Correct dimensions for CPT, based on the combined
            //  condition and domain sizes.
            //******************************************************************
            int condSize = 1;
            for(int k=0; k<cond.size(); ++k)
            {
                condSize *= maxsum::getDomainSize(cond[k]);
            }
            
            int domainSize = 1;
            for(int k=0; k<domain.size(); ++k)
            {
                domainSize *= maxsum::getDomainSize(domain[k]);
            }
            
            //******************************************************************
            //  Validate the number of values
            //******************************************************************
            const int nValues = trans.values_size();
            if(nValues!=(condSize*domainSize))
            {
                stringstream buf;
                buf << "TransProb requires " << domainSize << "x" << condSize;
                buf << "=" << domainSize*condSize << " values but " << nValues;
                buf << " are specified.";
                throw new ProtoException(buf.str());
            }
            
            //******************************************************************
            //  Copy values into transition matrix
            //******************************************************************
            Eigen::ArrayXXd cpt(domainSize,condSize);
            for(int k=0; k<nValues; ++k)
            {
                cpt(k) = trans.values(k);
            }
            
            TransProb result(cond, domain);
            result.setCPT(cpt);
            transProbs_i.insert(transProbs_i.end(), result);
            
        } // function addTransition
        
        /**
         * Called to initialise state and reward observation maps.
         * Basically just allocates space and sets everything to zero.
         */
        void initState()
        {
            //******************************************************************
            //  Initialise previous and current states
            //******************************************************************
            prevState_i.clear();
            prevState_i.reserve(protoSpec_i.states_size());
            for(int k=0; k<protoSpec_i.states_size(); ++k)
            {
                maxsum::VarID var = protoSpec_i.states(k).id();
                prevState_i[var] = 0;
            }
            curState_i = prevState_i;
            
            //******************************************************************
            //  Initialise previous variables (includes states and actions)
            //******************************************************************
            int noVars = curState_i.size() + protoSpec_i.actions_size();
            prevVars_i = prevState_i;
            prevVars_i.reserve(noVars);
            for(int k=0; k<protoSpec_i.actions_size(); ++k)
            {
                maxsum::VarID var = protoSpec_i.actions(k).id();
                prevVars_i[var] = 0;
            }
            
            //******************************************************************
            //  Initialise observe rewards
            //******************************************************************
            lastRewards_i.clear();
            lastRewards_i.reserve(protoSpec_i.rewards_size());
            for(int k=0; k<protoSpec_i.rewards_size(); ++k)
            {
                maxsum::FactorID fac = protoSpec_i.rewards(k).id();
                lastRewards_i[fac] = 0.0;
            }
            
            //******************************************************************
            //  Re don't plan to do this operation too often, so might as well
            //  save on space.
            //******************************************************************
            prevVars_i.shrink_to_fit();
            prevState_i.shrink_to_fit();
            curState_i.shrink_to_fit();
            lastRewards_i.shrink_to_fit();
            
        } // initState
        
    public:
        
        /**
         * Construct an Empty FactoredMDP.
         * @param gamma discount factor for MDP.
         */
        FactoredMDP(double gamma=0.0) : gamma_i(gamma) {}
        
        /**
         * Accessor for MDP discount factor.
         */
        double getGamma() { return gamma_i; }
        
        /**
         * Accessor method for MDP discount factor.
         */
        void setGamma(double gamma)
        {
            //******************************************************************
            //  Update protocol buffer specification as well as local copy,
            //  in case we ever need to write back out to file.
            //******************************************************************
            gamma_i = gamma;
            protoSpec_i.set_gamma(gamma);
        }
        
        /**
         * Reads a FactoredMDP from a file containing a single protocol buffer
         * FactoredMDP.proto instance.
         */
        void parseFromFile(std::string filename)
        {
            using namespace std;
            fstream input(filename.c_str(), ios::in | ios::binary);
            parseFromIStream(input);
        }
        
        /**
         * Reads FactoredMDP from protocol buffer specification.
         * @returns 0 on success, -1 otherwise
         */
        void parseFromIStream(std::istream& input)
        {
            //******************************************************************
            //  Parse definition from input stream using protocol buffers
            //******************************************************************
            if(!protoSpec_i.ParseFromIstream(&input))
            {
                throw ProtoException("Failed to parse specification from "
                                     "stream");
            }
            
            //******************************************************************
            //  Set MDP discount factor
            //******************************************************************
            if(!protoSpec_i.has_gamma())
            {
                throw ProtoException("Gamma not specified");
            }
            gamma_i = protoSpec_i.gamma();
            
            //******************************************************************
            //  Register state variables with maxsum library
            //******************************************************************
            const int nStates = protoSpec_i.states_size();
            if(0>=nStates)
            {
                throw ProtoException("At least one state must be specified");
            }
            
            for(int k=0; k<nStates; ++k)
            {
                maxsum::VarID var = protoSpec_i.states(k).id();
                maxsum::ValIndex siz = protoSpec_i.states(k).size();
                maxsum::registerVariable(var, siz);
            }
            
            //******************************************************************
            //  Register action variables with maxsum library
            //******************************************************************
            const int nActions = protoSpec_i.actions_size();
            if(0>=nActions)
            {
                throw ProtoException("At least one action must be specified");
            }
            
            for(int k=0; k<nActions; ++k)
            {
                maxsum::VarID var = protoSpec_i.actions(k).id();
                maxsum::ValIndex siz = protoSpec_i.actions(k).size();
                maxsum::registerVariable(var, siz);
            }
            
            //******************************************************************
            //  Initialise Rewards
            //******************************************************************
            const int nRewards = protoSpec_i.rewards_size();
            if(0>=nRewards)
            {
                throw ProtoException("At least one reward function must be"
                                     " specified");
            }
            
            rewardFactors_i.clear();
            rewardFactors_i.reserve(nRewards);
            for(int k=0; k<nRewards; ++k)
            {
                addReward(protoSpec_i.rewards(k));
            }
            
            //******************************************************************
            //  Initialise Transition Probabilties
            //******************************************************************
            const int nCPT = protoSpec_i.transitions_size();
            if(0>=nCPT)
            {
                throw ProtoException("At least one transition probability "
                                     "matrix must be specified");
            }
            
            transProbs_i.clear();
            transProbs_i.reserve(nCPT);
            for(int k=0; k<nCPT; ++k)
            {
                addTransition(protoSpec_i.transitions(k));
            }
            
            //******************************************************************
            //  Finally, initialise current MDP state
            //******************************************************************
            initState();
            
        } // parseFromIStream
        
    }; // class FactoredMDP
    
    
} // dec_brl


#endif // DEC_BRL_FACTORED_MDP_H
