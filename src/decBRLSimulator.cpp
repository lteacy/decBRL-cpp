/**
 * @file decBRLSimulator.cpp
 * Defines main simulator executable for running experiments.
 * This program takes a experimental setup data specified using
 * protocol buffers, and outputs experimental results along with the original
 * experiment specification.
 */
#include <iostream>
#include <string>
#include <boost/random/mersenne_twister.hpp>
#include "dec_brl/FactoredMDP.h"

/**
 * Defines main simulator executable for running experiments.
 * This program takes a experimental setup data specified using
 * protocol buffers, and outputs experimental results along with the original
 * experiment specification.
 */
int main(int argc, char* argv[]) 
{
    using namespace dec_brl;
    
    std::cout << "Hello world!" << std::endl;
    
    //**************************************************************************
    // Verify that the version of protocol buffers that we linked against is
    // compatible with the version of the headers we compiled against.
    //**************************************************************************
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    //**************************************************************************
    //  Get input file name from arguments, or look in default location
    //**************************************************************************
    std::string inputFile = "test.mdp";
    if(argc>=2)
    {
        inputFile = argv[1];
    }
    
    //**************************************************************************
    //  Try to Read a Factored MDP from file
    //**************************************************************************
    FactoredMDP mdp;
    try
    {
        mdp.parseFromFile(inputFile);
    }
    catch(std::exception& e)
    {
        std::cout << "Caught Error: " << e.what() << std::endl;
        std::cout << "Failed to read data from file: " << inputFile << '\n';
        std::cout << "Usage: " << argv[0] << " inputFilename " << std::endl;
        return EXIT_FAILURE;
    }
    
    //**************************************************************************
    //  Perform some actions
    //**************************************************************************
    boost::mt19937 randGenerator; // generates uniform random numbers
    boost::container::flat_map<unsigned int, int> actions;
    std::cout << "MDP=" << mdp << std::endl;
    actions[3]=1;
    actions[4]=1;
    std::cout << "performing actions: " << actions << std::endl;
    mdp.act(randGenerator, actions);
    std::cout << "MDP=" << mdp << std::endl;
    actions[3]=0;
    actions[4]=2;
    std::cout << "performing actions: " << actions << std::endl;
    mdp.act(randGenerator, actions);
    std::cout << "MDP=" << mdp << std::endl;
    actions[3]=1;
    actions[4]=0;
    std::cout << "performing actions: " << actions << std::endl;
    mdp.act(randGenerator, actions);
    std::cout << "MDP=" << mdp << std::endl;
    
    
    //**************************************************************************
    // Delete all global objects allocated by libprotobuf.
    // Not strictly necessary - but perhaps best to tidy up own mess explicitly.
    //**************************************************************************
    google::protobuf::ShutdownProtobufLibrary();
    std::cout << "Exiting normally." << std::endl;
    
    return EXIT_SUCCESS;
}
