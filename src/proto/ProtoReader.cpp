/**
 * @file ProtoRecorder.cpp
 * Class used to record simulator results to protocol buffer file.
 */
#include "dec_brl/ProtoReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include "fcntl.h"

/**
 * Constructor opens file for reading.
 * @param inFile the name of the input file.
 */
dec_brl::ProtoReader::ProtoReader(std::string inFile)
: pRawInput_i(0), pCodedInput_i(0), setup_i(), outcomes_i(), nextMsg_i(),
  curOutcome_i()
{
    //**************************************************************************
    //  Try to open file for reading
    //**************************************************************************
    int fd = open(inFile.c_str(),O_RDONLY);
    if(!fd)
    {
        std::stringstream buf;
        buf << "Failed to open file: " << inFile;
        throw ProtoException(buf.str());
    }
    pRawInput_i = new google::protobuf::io::FileInputStream(fd);
    pRawInput_i->SetCloseOnDelete(true);
    pCodedInput_i = new google::protobuf::io::CodedInputStream(pRawInput_i);
    
    //**************************************************************************
    //  Read first message into buffer to get started.
    //**************************************************************************
    readNextMessage();
    
    //**************************************************************************
    //  Read results of the first experiment into buffers
    //**************************************************************************
    readExperiment();
    
} // constructor

/**
 * Returns the next experimental outcome stored on file.
 */
const dec_brl::proto::Outcome& dec_brl::ProtoReader::getNextOutcome()
{
    using namespace dec_brl::proto;
    
    //**************************************************************************
    //  Ensure there is another outcome to give
    //**************************************************************************
    if(outcomes_i.empty())
    {
        throw ProtoException("Requested Outcome from empty list");
    }
    curOutcome_i.CopyFrom(outcomes_i.front());
    outcomes_i.pop();
    
    //**************************************************************************
    //  If we've reached the end of this experiment, and there is another
    //  experiment to come, then read the next experiment into buffers.
    //**************************************************************************
    if(outcomes_i.empty() && nextMsg_i.type()==ResultMsg_Type_SETUP)
    {
        readExperiment();
    }
    
    //**************************************************************************
    //  Finally, return the current outcome
    //**************************************************************************
    return curOutcome_i;
}

/**
 * Closes the file, after which no more data may be written. This
 * is called automatically by destructor.
 */
void dec_brl::ProtoReader::close()
{
    //**************************************************************************
    // Unless already closed, close the coded input stream.
    //**************************************************************************
    if(0!=pCodedInput_i)
    {
        delete pCodedInput_i;
        pCodedInput_i=0;
    }
    
    //**************************************************************************
    // Unless already closed, close the raw input stream.
    //**************************************************************************
    if(0!=pRawInput_i)
    {
        delete pRawInput_i;
        pRawInput_i=0;
    }
    
} // function close

/**
 * Read the next message.
 * Returns true iff the next message is not the end of file marker.
 */
bool dec_brl::ProtoReader::readNextMessage()
{
    //**************************************************************************
    // Ensure that the input file has been initialised.
    //**************************************************************************
    if( (0==pCodedInput_i) || (0==pRawInput_i) )
    {
        throw ProtoException("Invalid input stream state!");
    }
    
    //**************************************************************************
    //  Clear any previous messages from the buffer
    //**************************************************************************
    nextMsg_i.clear_outcome();
    nextMsg_i.clear_setup();
    nextMsg_i.set_type(proto::ResultMsg_Type_END_MSG);
    
    //**************************************************************************
    // Get the number of bytes for the next message, and limit the
    // next read to that number of bytes.
    // This is required because protocol buffers are not self delimiting.
    //**************************************************************************
    google::protobuf::uint64 msgSize = 0;
    if(!pCodedInput_i->ReadVarint64(&msgSize))
    {
        throw ProtoException("Size of next message could not be read!");
    }
    int lastLimit = pCodedInput_i->PushLimit(msgSize);
    
    //**************************************************************************
    // Read in the next message, and release the read limit.
    //**************************************************************************
    if(!nextMsg_i.ParseFromCodedStream(pCodedInput_i))
    {
        throw ProtoException("Message could not be read!");
    }
    
    pCodedInput_i->PopLimit(lastLimit);
    
    //**************************************************************************
    // If END_MSG was encountered, return null to signal EOF. Otherwise
    // return the buffered message.
    //**************************************************************************
    if(proto::ResultMsg::END_MSG==nextMsg_i.type())
    {
        return false;
    }
    return true;
    
} // function readNextMessage

/**
 * Reads the next message.
 * @pre the ExperimentSetup message at the beginning of this experiment's set
 * of results should already have been read into the nextMsg_i buffer.
 */
void dec_brl::ProtoReader::readExperiment()
{
    //**************************************************************************
    //  Check that the experimental setup is currently in the message buffer
    //**************************************************************************
    if(nextMsg_i.type()!=proto::ResultMsg_Type_SETUP || !nextMsg_i.has_setup())
    {
        throw ProtoException("Did not encounter SETUP at beginning of"
                             " result stream.");
    }
    
    //**************************************************************************
    //  Copy the experimental setup into the setup buffer, so that it can
    //  be returned to the caller.
    //**************************************************************************
    setup_i.CopyFrom(nextMsg_i.setup());
    
    //**************************************************************************
    //  Read all subsequent outcomes into queue
    //**************************************************************************
    readNextMessage();
    while(nextMsg_i.type()==proto::ResultMsg_Type_OUTCOME
          && nextMsg_i.has_outcome())
    {
        outcomes_i.push(proto::Outcome());
        outcomes_i.back().CopyFrom(nextMsg_i.outcome());
        readNextMessage();
        
    } // while loop
    
    //**************************************************************************
    //  If we didn't read any outcomes, assume something is wrong
    //**************************************************************************
    if(outcomes_i.empty())
    {
        throw ProtoException("Could not read any outcomes for current"
                             " experiment!");
    }
    
} // readNextMessage
