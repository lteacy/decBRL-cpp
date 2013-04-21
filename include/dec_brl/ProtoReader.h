/**
 * @file ProtoReader.h
 * Class used to read result files recorded by simulator using protocol buffers.
 */
#ifndef DEC_BRL_PROTO_READER
#define DEC_BRL_PROTO_READER

#include "Experiment.pb.h"
#include "Results.pb.h"
#include "exceptions.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>
#include <queue>

namespace dec_brl
{
    /**
     * Class used to read result files recorded by simulator using protocol
     * buffers.
     */
    class ProtoReader
    {
    private:
        
        /**
         * Raw file output stream
         */
        google::protobuf::io::FileInputStream* pRawInput_i;
        
        /**
         * Coded output stream backed by rawOutput_i
         */
        google::protobuf::io::CodedInputStream* pCodedInput_i;
        
        /**
         * Buffer in which to hold current experimental setup read from file.
         */
        proto::ExperimentSetup setup_i;
        
        /**
         * Buffer in which to read outcomes of the current experiment.
         */
        std::queue<proto::Outcome> outcomes_i;
        
        /**
         * Buffer in which to store the next message.
         */
        proto::ResultMsg nextMsg_i;
        
        /**
         * Read next message in file.
         */
        bool readNextMessage();
        
        /**
         * Read all outcomes for the current experiment.
         */
        void readExperiment();
        
    public:
        
        /**
         * Constructor opens file for reading.
         * @param inFile the name of the input file.
         */
        ProtoReader(std::string inFile);
        
        /**
         * Returns the number of recorded outcomes for the current experiment.
         */
        int getNumOfOutcomes()
        {
            return 0;
        }
        
        /**
         * Returns the current experimental setup.
         */
        const proto::ExperimentSetup& getSetup()
        {
            return setup_i;
        }
        
        /**
         * Returns the next experimental outcome stored on file.
         */
        const proto::Outcome& getNextOutcome()
        {
            return nextMsg_i.outcome();
        }
        
        /**
         * Returns true if there is a next outcome.
         */
        bool hasOutcome()
        {
            return false;
        }
        
        /**
         * Returns true iff we've ran out of outcomes for this experiment,
         * but there is another experiment to follow.
         */
        bool hasExperiment()
        {
            return false;
        }
        
        /**
         * Closes the file, after which no more data may be written. This
         * is called automatically by destructor.
         */
        void close();
        
        /**
         * Destructor makes sure the file is closed.
         */
        virtual ~ProtoReader()
        {
            close();
        }
        
    }; // class ProtoReader
    
} // namespace dec_brl


#endif // DEC_BRL_PROTO_READER
