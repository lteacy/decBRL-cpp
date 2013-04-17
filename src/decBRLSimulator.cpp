/**
 * @file decBRLSimulator.cpp
 * Defines main simulator executable for running experiments.
 * This program takes a experimental setup data specified using
 * protocol buffers, and outputs experimental results along with the original
 * experiment specification.
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include "dec_brl/exceptions.h"
#include <string>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/container/flat_map.hpp>
#include "dec_brl/FactoredMDP.h"
#include "Experiment.pb.h"
#include <ctime>
#include "dec_brl/DecQLearner.h"
#include "dec_brl/DecBayesQ.h"
#include "dec_brl/DecBayesModelLearner.h"
#include "dec_brl/LearningSolver.h"

namespace  {
    
    /**
     * A Simple class for outputing MDP results to standard output.
     */
    class SimpleRecorder
    {
    public:
        
        /**
         * Outputs the current MDP to standard output, along with duration
         * in milliseconds for learner to observe and act on current MDP state.
         * Missing times are recorded as -1
         */
        void operator()
        (
         const dec_brl::FactoredMDP& mdp,
         int epsiode,
         int timestep,
         int actionTime=-1,
         int observationTime=-1
        )
        {
            std::cout << "EPISODE: " << epsiode << "TIMESTEP: " << timestep;
            std::cout << std::endl;
            std::cout << mdp << std::endl;
            std::cout << " obsTime: " << observationTime << " actTime: ";
            std::cout << actionTime << std::endl;
            
        } // operator()
        
    };
    
    /**
     * Run single episode using a given policy and MDP.
     * @param[in] mdp a Factored MDP - the problem to solve.
     * @param[in] learner the reinforcement learner for choosing actions.
     * @param[in] recorder object used for recording results
     * @param[in] random random number generator for generating events
     * @param[in] episode the sequence number of this episode
     * @param[in] nTimesteps the number of timesteps in this episode.
     */
    template<class Learner, class DataRecorder, class Rand> void runEpisode
    (
     dec_brl::FactoredMDP& mdp,
     Learner& learner,
     DataRecorder& recorder,
     Rand& random,
     uint episode,
     uint nTimesteps
    )
    {
        using namespace dec_brl;
        using namespace boost::container;
        
        //**********************************************************************
        //  Initialise action map
        //**********************************************************************
        flat_map<maxsum::VarID, maxsum::ValIndex> actions;
        int nActions = mdp.getNumOfActions();
        actions.reserve(nActions);
        for(int k=0; k<nActions; ++k)
        {
            actions[k] = 0;
        }

        //**********************************************************************
        //  For each timestep
        //**********************************************************************
        for(int timestep=0; timestep<nTimesteps; ++timestep)
        {
            //******************************************************************
            //  Ask the learner to choose its next actions and time how long
            //  it takes to decide.
            //******************************************************************
            clock_t start = std::clock() / (CLOCKS_PER_SEC / 1000);
            learner.act(mdp.getCurState(),actions);
            clock_t end = std::clock() / (CLOCKS_PER_SEC / 1000);
            int time2act = end-start;
            
            //******************************************************************
            //  Perform the chosen actions on the MDP
            //******************************************************************
            mdp.act(random,actions);
            
            //******************************************************************
            //  Let the learner observe the result and time how long it takes
            //  to update the model
            //******************************************************************
            start = std::clock() / (CLOCKS_PER_SEC / 1000);
            learner.observe(mdp.getPrevVars(),actions,mdp.getCurState(),
                            mdp.getLastRewards());
            end = std::clock() / (CLOCKS_PER_SEC / 1000);
            int time2obs = end-start;
            
            //******************************************************************
            //  Record result
            //******************************************************************
            recorder(mdp,episode,timestep,time2act,time2obs);
            
        } // timestep loop

    } // function runEpisode

    /**
     * Run an experiment using a given policy and MDP.
     * @param[in] mdp a Factored MDP - the problem to solve.
     * @param[in] learner the reinforcement learner for choosing actions.
     * @param[in] recorder object used for recording results
     * @param[in] randSeeds vector containing either a single random seed,
     * or one per episode.
     * @param[in] nEpisodes the number of independent episodes to run
     * @param[in] nTimesteps the number of timesteps per episode.
     */
    template<class Learner, class DataRecorder> void runExperiment
    (
     dec_brl::FactoredMDP& mdp,
     Learner& learner,
     DataRecorder& recorder,
     const std::vector<uint> randSeeds,
     uint nEpisodes,
     uint nTimesteps
    )
    {
        using namespace dec_brl;
        
        //**********************************************************************
        //  Setup the random number generator. If we have no seeds, we use the
        //  current time.
        //**********************************************************************
        std::cout << "Setting up random number generator ";
        boost::random::mt19937 random;
        if(0==randSeeds.size())
        {
            std::cout << "using current time." << std::endl;
            random.seed(std::time(0));
        }
        else
        {
            std::cout << "using specified seed." << std::endl;
            random.seed(randSeeds[0]);
        }
        
        //**********************************************************************
        //  Inform the learner about the structure of the MDP
        //**********************************************************************
        std::cout << "Telling learner about problem structure..";
        // TODO
        std::cout << "OK." << std::endl;
        
        //**********************************************************************
        //  Run Episodes
        //**********************************************************************
        for(int k=0; k<nEpisodes; ++k)
        {
            //******************************************************************
            //  Make a new fresh copy of the learner that has no member of
            //  previous episodes.
            //******************************************************************
            std::cout << "Initialising Learner...";
            Learner freshLearner = learner;
            std::cout << "OK." << std::endl;
            
            //******************************************************************
            //  Reinitialise the MDP state
            //******************************************************************
            std::cout << "Initialising MDP...";
            mdp.initState(random);
            std::cout << "OK." << std::endl;
            
            //******************************************************************
            //  Run this episode and record results
            //******************************************************************
            std::cout << "Starting episode: " << k << std::endl;
            runEpisode(mdp, freshLearner, recorder, random, k, nTimesteps);
            
            //******************************************************************
            //  If we have one random seed for each episode, then reset the
            //  seed now for the next state.
            //******************************************************************
            if(randSeeds.size()>k+1)
            {
                std::cout << "setting new random seed from parameters";
                std::cout << std::endl;
                random.seed(randSeeds[k+1]);
            }

        } // episode loop
            
    } // function runExperiment
    
} // private (anonymous) module namespace

/**
 * Defines main simulator executable for running experiments.
 * This program takes a experimental setup data specified using
 * protocol buffers, and outputs experimental results along with the original
 * experiment specification.
 */
int main(int argc, char* argv[]) 
{
    using namespace dec_brl;
    
    //**************************************************************************
    //  String used to tell user how to use this executable
    //**************************************************************************
    std::stringstream buf;
    buf << "Usage: " << argv[0] << " [inFile] [outFile]";
    const std::string USAGE_STR = buf.str();
    
    //**************************************************************************
    // Verify that the version of protocol buffers that we linked against is
    // compatible with the version of the headers we compiled against.
    //**************************************************************************
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    //**************************************************************************
    //  Get input file name from arguments, or look in default location
    //**************************************************************************
    std::string inFilename = "test.exp";
    if(argc>=2)
    {
        inFilename = argv[1];
    }
    
    //**************************************************************************
    //  Get the output file name from arguments, or use default location
    //**************************************************************************
    std::string outFilename = "test.results";
    if(argc>=3)
    {
        inFilename = argv[2];
    }
    
    //**************************************************************************
    // Try to Read Experiment setup from file
    //**************************************************************************
    std::fstream inFile(inFilename.c_str(), std::ios::in | std::ios::binary);
    proto::ExperimentSetup setup;
    if(!setup.ParseFromIstream(&inFile))
    {
        std::cerr << "Failed to read setup from file: " << inFilename;
        std::cerr << std::endl;
        std::cout << "Failed to read data from file: " << inFilename << '\n';
        std::cout << USAGE_STR << std::endl;
        return EXIT_FAILURE;
    }

    //**************************************************************************
    //  Read a Factored MDP (the problem to solve) from the specification.
    //**************************************************************************
    if(!setup.has_problem())
    {
        std::cerr << "No problem to solve found on file: " << inFilename;
        std::cerr << std::endl;
        std::cout << "Failed to read data from file: " << inFilename << '\n';
        std::cout << USAGE_STR << std::endl;
        return EXIT_FAILURE;
    }
    
    FactoredMDP mdp; // the Factored MDP
    
    try
    {
        mdp.copyFromProto(setup.problem());
    }
    catch(std::exception& e)
    {
        std::cerr << "Caught Error: " << e.what() << std::endl;
        std::cout << "Failed to read data from file: " << inFilename << '\n';
        std::cout << USAGE_STR << std::endl;
        return EXIT_FAILURE;
    }
    
    //**************************************************************************
    //  Retrieve random seeds (if any) from setup
    //**************************************************************************
    std::vector<uint32_t> randSeeds(setup.seed().begin(),setup.seed().end());
    
    //**************************************************************************
    //  Set up recorder for recording experimental results.
    //**************************************************************************
    SimpleRecorder recorder;
    
    //**************************************************************************
    //  Get number of timesteps and number of episodes from specification
    //**************************************************************************
    uint nTimesteps = setup.timesteps();
    uint nEpisodes = setup.episodes();
    
    //**************************************************************************
    // If selected, setup and run experiment on RANDOM policy
    //**************************************************************************
    proto::ExperimentSetup_Algorithm learnerType = setup.learner();
    if(learnerType==proto::ExperimentSetup_Algorithm_RANDOM)
    {
        std::cout << "Running Experiment on RANDOM policy" << std::endl;
    }
    //**************************************************************************
    //  EGREEDY Q Learner experiment
    //**************************************************************************
    else if(learnerType==proto::ExperimentSetup_Algorithm_EGREEDY_Q)
    {
        std::cout << "Running Experiment on EGREEDY_Q policy" << std::endl;
        double alpha_q = setup.params_egreedyq().alphaq();
        double epsilon = setup.params_egreedyq().epsilonq();
        DecQLearner qLearner(alpha_q,mdp.getGamma(),epsilon);
        runExperiment(mdp, qLearner, recorder, randSeeds, nEpisodes,
                      nTimesteps);
    }
    //**************************************************************************
    // Bayes Q Learner experiment
    //**************************************************************************
    else if(learnerType==proto::ExperimentSetup_Algorithm_BAYES_Q)
    {
        std::cout << "Running Experiment on BAYES_Q policy" << std::endl;
        DecBayesQ bayesQ(mdp.getGamma());
        runExperiment(mdp, bayesQ, recorder, randSeeds, nEpisodes, nTimesteps);
    }
    //**************************************************************************
    //  Model-based Bayes RL experiment
    //**************************************************************************
    else if(learnerType==proto::ExperimentSetup_Algorithm_MODEL_BAYES)
    {
        std::cout << "Running Experiment on MODEL_BAYES policy";
        LearningSolver<DecQLearner> solver;
        
        DecBayesModelLearner<LearningSolver<DecQLearner> >
            modelBayes(mdp.getGamma());
        
        runExperiment(mdp, modelBayes, recorder, randSeeds, nEpisodes,
                      nTimesteps);
    }
    //**************************************************************************
    //  If we get this far, somehow we've got an unknown type of experiment
    //  Probably the code is out of date.
    //**************************************************************************
    else
    {
        std::cerr << "Internal Error: Unknown Algorithm type" << std::endl;
        std::cout << "Exiting due to unknown algorithm type" << std::endl;
        return EXIT_FAILURE;
    }
    
    //**************************************************************************
    // Delete all global objects allocated by libprotobuf.
    // Not strictly necessary - but perhaps best to tidy up own mess explicitly.
    //**************************************************************************
    google::protobuf::ShutdownProtobufLibrary();
    std::cout << "Exiting normally." << std::endl;
    
    return EXIT_SUCCESS;
}
