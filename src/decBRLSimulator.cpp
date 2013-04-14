/**
 * @file decBRLSimulator.cpp
 * Defines main simulator executable for running experiments.
 * This program takes a experimental setup data specified using
 * protocol buffers, and outputs experimental results along with the original
 * experiment specification.
 */
#include <iostream>
#include <string>
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
    // Delete all global objects allocated by libprotobuf.
    // Not strictly necessary - but perhaps best to tidy up own mess explicitly.
    //**************************************************************************
    google::protobuf::ShutdownProtobufLibrary();
    std::cout << "Exiting normally." << std::endl;
    
    return EXIT_SUCCESS;
}
