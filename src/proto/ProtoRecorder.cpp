/**
 * @file ProtoRecorder.cpp
 * Used to record experimental setup and results to file.
 */
#include "dec_brl/ProtoRecorder.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include "fcntl.h"

/**
 * Constructor opens file for writing.
 * @param outFile the name of the output file.
 * @param append if true append to, rather than truncate file.
 */
dec_brl::ProtoRecorder::ProtoRecorder(std::string outFile, bool append)
: pRawOutput_i(0), pCodedOutput_i(0), nextMsg_i()
{
    //**************************************************************************
    // Set the file write flags based on whether we want to append to file
    // or overwrite it.
    //**************************************************************************
    int fileFlags = O_WRONLY | O_CREAT;
    if(append)
    {
        fileFlags = fileFlags | O_APPEND;
    }
    else
    {
        fileFlags = fileFlags | O_TRUNC;
    }
    
    //**************************************************************************
    // Try to open output file
    //**************************************************************************
    int fileDescriptor = open(outFile.c_str(),fileFlags,S_IWUSR | S_IRUSR);
    if(!fileDescriptor)
    {
        std::stringstream buf;
        buf << "Could not open file : " << outFile;
        throw ProtoException(buf.str());
    }
    
    //**************************************************************************
    // Try to create raw output stream backed by file, and tell it to
    // close the open file when its destructor is called.
    //**************************************************************************
    using namespace google::protobuf;
    pRawOutput_i = new io::FileOutputStream(fileDescriptor);
    pRawOutput_i->SetCloseOnDelete(true);
    
    //**************************************************************************
    // Try to create coded output stream backed by raw output
    //**************************************************************************
    pCodedOutput_i = new io::CodedOutputStream(pRawOutput_i);
    
} // ProtoRecorder constructor

/**
 * Writes the Experimental Setup to file.
 * This applies to all subsequence outcomes recorded in the file, until
 * some other setup is written, or the end of file is reached.
 */
void dec_brl::ProtoRecorder::writeSetup(proto::ExperimentSetup& setup)
{
    //**************************************************************************
    // Ensure that output streams are initialised.
    //**************************************************************************
    if( (0==pCodedOutput_i) || (0==pRawOutput_i) )
    {
        throw ProtoException("Result output stream is not intialised.");
    }
    
    //**************************************************************************
    // Copy content details into wrapper message
    //**************************************************************************
    nextMsg_i.set_type(proto::ResultMsg::SETUP);
    nextMsg_i.clear_setup();
    nextMsg_i.clear_outcome();
    proto::ExperimentSetup* pContent = nextMsg_i.mutable_setup();
    (*pContent) = setup;
    
    //**************************************************************************
    // Write wrapper message to file
    // Note: need to write byte size of message first, so that we know where
    // the end of the message occurs in the stream when we read it back.
    //**************************************************************************
    int msgSize = nextMsg_i.ByteSize();
    pCodedOutput_i->WriteVarint64(msgSize);
    if(!nextMsg_i.SerializeToCodedStream(pCodedOutput_i))
    {
        throw ProtoException("Could not write message to file.");
    }
    
} // ProtoRecorder::writeSetup()

/**
 * Fills message buffer with the result of the current timestep.
 */
void dec_brl::ProtoRecorder::fillOutcome
(
 const dec_brl::FactoredMDP& mdp,
 int epsiode,
 int timestep,
 int actionTime,
 int observationTime
)
{
    //**************************************************************************
    //  Get reference to the outcome message field.
    //**************************************************************************
    proto::Outcome* pOutcome = nextMsg_i.mutable_outcome();
    assert(0!=pOutcome);
    
    //**************************************************************************
    //  Set primitive fields (timestep, episode & operation times)
    //**************************************************************************
    pOutcome->set_episode(epsiode);
    pOutcome->set_timestep(timestep);
    pOutcome->set_acttimeinms(actionTime);
    pOutcome->set_updatetimeinms(observationTime);
    
    //**************************************************************************
    //  Add all actions to the protobuf structure. We do this by iterating
    //  through the list of action IDs and retrieving the corresponding values
    //  from the previous variables map. We do it this way to make sure we
    //  only pick up actions and not states.
    //
    //  Although there are more efficient ways to do this, the difference
    //  in speed would probably be unoticeable, and doing it any other
    //  way would require changing the FactorMDP data structures.
    //**************************************************************************
    typedef FactoredMDP::VarIDList::const_iterator VarIt;
    typedef FactoredMDP::VarMap::const_iterator MapIt;
    for(VarIt it=mdp.getActionIDs().begin(); it!=mdp.getActionIDs().end(); ++it)
    {
        maxsum::VarID id = *it;                     // get action id
        MapIt mapLoc = mdp.getPrevVars().find(*it); // find its value
        assert(mapLoc!=mdp.getPrevVars().end());    // sanity check its there
        maxsum::ValIndex val = mapLoc->second;      // retrieve the value
        
        proto::Outcome_Variable* pVar = pOutcome->add_action(); // add to list
        assert(0!=pVar);
        pVar->set_id(id);
        pVar->set_value(val);
    }
    
    //**************************************************************************
    //  Add all states to the protobuf structure.
    //  We do this in the same way as we did for the actions above.
    //**************************************************************************
    for(VarIt it=mdp.getStateIDs().begin(); it!=mdp.getStateIDs().end(); ++it)
    {
        maxsum::VarID id = *it;                     // get state id
        MapIt mapLoc = mdp.getPrevVars().find(*it); // find its value
        assert(mapLoc!=mdp.getPrevVars().end());    // sanity check its there
        maxsum::ValIndex val = mapLoc->second;      // retrieve the value
        
        proto::Outcome_Variable* pVar = pOutcome->add_state(); // add to list
        assert(0!=pVar);
        pVar->set_id(id);
        pVar->set_value(val);
    }
    
    //**************************************************************************
    //  Add factored rewards (with corresponding factor ids)
    //**************************************************************************
    typedef FactoredMDP::RewardMap::const_iterator RewardIt;
    for(RewardIt it = mdp.getLastRewards().begin();
        it != mdp.getLastRewards().end(); ++it)
    {
        maxsum::FactorID id = it->first;
        maxsum::ValType val =it->second;
        proto::Outcome_Reward* pReward = pOutcome->add_reward();
        pReward->set_id(id);
        pReward->set_value(val);
    }
    
} // ProtoRecorder::fillOutcome

/**
 * Outputs the output of the current timestep to file.
 */
void dec_brl::ProtoRecorder::operator()
(
 const dec_brl::FactoredMDP& mdp,
 int epsiode,
 int timestep,
 int actionTime,
 int observationTime
)
{
    //**************************************************************************
    // Ensure that output streams are initialised.
    //**************************************************************************
    if( (0==pCodedOutput_i) || (0==pRawOutput_i) )
    {
        throw ProtoException("Result output stream is not intialised.");
    }
    
    //**************************************************************************
    // Copy content details into wrapper message
    //**************************************************************************
    nextMsg_i.set_type(proto::ResultMsg::OUTCOME);
    nextMsg_i.clear_setup();
    nextMsg_i.clear_outcome();
    fillOutcome(mdp, epsiode, timestep, actionTime, observationTime);
    
    //**************************************************************************
    // Write wrapper message to file
    // Note: need to write byte size of message first, so that we know where
    // the end of the message occurs in the stream when we read it back.
    //**************************************************************************
    int msgSize = nextMsg_i.ByteSize();
    pCodedOutput_i->WriteVarint64(msgSize);
    if(!nextMsg_i.SerializeToCodedStream(pCodedOutput_i))
    {
        throw ProtoException("Could not write message to file.");
    }
    
} // ProtoRecorder::operator()

/**
 * Closes the file, after which no more data may be written. This
 * is called automatically by destructor.
 */
void dec_brl::ProtoRecorder::close()
{
    //**************************************************************************
    // Something is wrong if either the raw output or coded output is closed
    // while the other is not.
    //**************************************************************************
    if( (0==pCodedOutput_i) != (0==pRawOutput_i) )
    {
        throw ProtoException("Invalid Output stream state|");
    }
    
    //**************************************************************************
    // If the file streams are already closed, we have nothing to do.
    //**************************************************************************
    if( (0==pCodedOutput_i) || (0==pRawOutput_i) )
    {
        return;
    }
    
    //**************************************************************************
    // Before we close a stream, we first write and END_MSG to signal that
    // the file has been terminated properly.
    //**************************************************************************
    nextMsg_i.set_type(proto::ResultMsg::END_MSG);
    nextMsg_i.clear_setup();
    nextMsg_i.clear_outcome();
    int msgSize = nextMsg_i.ByteSize();
    pCodedOutput_i->WriteVarint64(msgSize);
    if(!nextMsg_i.SerializeToCodedStream(pCodedOutput_i))
    {
        throw ProtoException("Could not write message to file.");
    }
    
    //**************************************************************************
    // If necessary, delete output streams and set their pointers to null
    // We only expect these to be null already if close() has been called
    // previously.
    //**************************************************************************
    if(0!=pCodedOutput_i)
    {
        delete pCodedOutput_i;
        pCodedOutput_i=0;
    }
    
    if(0!=pRawOutput_i)
    {
        delete pRawOutput_i;
        pRawOutput_i=0;
    }
    
} // ProtoRecorder::close()

