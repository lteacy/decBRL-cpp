/**
 * @file ProtoRecorder.h
 * Class used to record simulator results to protocol buffer file.
 */
#ifndef DEC_BRL_PROTO_RECORDER_H
#define DEC_BRL_PROTO_RECORDER_H

#include "Experiment.pb.h"
#include "dec_brl/FactoredMDP.h"
#include "Results.pb.h"
#include "exceptions.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>

namespace dec_brl
{
    /**
     * Class used to record simulator results to protocol buffer file.
     * Experimental setup is written first, preceded by its size in bytes,
     * and then each timestep result is written sequentally - again proceeded
     * by record size in bytes.
     */
    class ProtoRecorder
    {
    private:
        
        /**
         * Raw file output stream
         */
        google::protobuf::io::FileOutputStream* pRawOutput_i;
        
        /**
         * Coded output stream backed by rawOutput_i
         */
        google::protobuf::io::CodedOutputStream* pCodedOutput_i;
        
        /**
         * Buffer in which to store the next message.
         */
        proto::ResultMsg nextMsg_i;
        
        /**
         * Fills message buffer with the result of the current timestep
         */
        void fillOutcome
        (
         const dec_brl::FactoredMDP& mdp,
         int epsiode,
         int timestep,
         int actionTime=-1,
         int observationTime=-1
        );
        
    public:
        
        /**
         * Constructor opens file for writing.
         * @param outFile the name of the output file.
         * @param append if true append to, rather than truncate file.
         */
        ProtoRecorder(std::string outFile, bool append=false);
        
        /**
         * Writes the Experimental Setup to file.
         * This applies to all subsequence outcomes recorded in the file, until
         * some other setup is written, or the end of file is reached.
         */
        void writeSetup(proto::ExperimentSetup& setup);
        
        /**
         * Outputs the output of the current timestep to file.
         */
        void operator()
        (
         const dec_brl::FactoredMDP& mdp,
         int epsiode,
         int timestep,
         int actionTime=-1,
         int observationTime=-1
        );
        
        /**
         * Closes the file, after which no more data may be written. This
         * is called automatically by destructor.
         */
        void close();
        
        /**
         * Destructor makes sure the file is closed.
         */
        virtual ~ProtoRecorder()
        {
            close();
        }
        
    }; // class ProtoRecorder
    
    
} // namespace dec_brl

#endif // DEC_BRL_PROTO_RECORDER_H
