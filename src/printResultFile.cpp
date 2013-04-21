/**
 * @file printResultFile
 * Utility program that prints human readable content of Experiments result
 * file.
 */
#include "dec_brl/ProtoReader.h"
#include <google/protobuf/text_format.h>
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    using namespace dec_brl;
    using namespace google::protobuf;
    
    //**************************************************************************
    //  Get input file from argument list
    //**************************************************************************
    std::cout << "Hello world!" << std::endl;
    if(2>argc)
    {
        std::cout << "Must specify input file!" << std::endl;
        std::cout << "Usage: " << argv[0] << " inputFile" << std::endl;
        return EXIT_FAILURE;
    }
    std::string inFilename = argv[1];
    
    //**************************************************************************
    //  Try to parse input file.
    //**************************************************************************
    try
    {
        //**********************************************************************
        //  While there are still more experiments on file
        //**********************************************************************
        ProtoReader reader(inFilename);
        while(reader.hasExperiment())
        {
            //******************************************************************
            //  Print Experimental Setup
            //******************************************************************
            const proto::ExperimentSetup& setup = reader.getSetup();
            std::string setupStr;
            TextFormat::PrintToString(setup, &setupStr);
            std::cout << setupStr << std::endl;
            
            //******************************************************************
            //  Print all outcomes
            //******************************************************************
            std::cout << "NUMBER OF OUTCOMES: " << reader.getNumOfOutcomes();
            std::cout << std::endl;
            while(reader.hasOutcome())
            {
                const proto::Outcome& outcome = reader.getNextOutcome();
                std::string outcomeStr;
                TextFormat::PrintToString(outcome, &outcomeStr);
                std::cout << outcomeStr << std::endl;
            }
            
        } // outer while loop
        
    }
    //**************************************************************************
    //  Deal with any errors
    //**************************************************************************
    catch(std::exception& e)
    {
        std::cerr << "Caught Error: " << e.what() << std::endl;
        std::cout << "Failed to read data from file: " << inFilename << '\n';
        return EXIT_FAILURE;
    }
    
} // main function