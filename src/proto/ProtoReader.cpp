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
: pRawInput_i(0), pCodedInput_i(0), setup_i(), outcomes_i(), nextMsg_i()
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
    //**************************************************************************
    
} // constructor

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
 */
void dec_brl::ProtoReader::readExperiment()
{
    //**************************************************************************
    //  
    //**************************************************************************
    
    //**************************************************************************
    //**************************************************************************
    
    //**************************************************************************
    //**************************************************************************
    
} // readNextMessage
