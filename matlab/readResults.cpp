#include <matrix.h>
#include <mex.h>
#include "dec_brl/ProtoReader.h"
#include <string>

/**
 * Utility function to write EGreedyQParams from protocol buffers to matlab.
 */
mxArray* egreedyQParams2mat
(
 const dec_brl::proto::ExperimentSetup_EGreedyQParams& params
)
{
    return mxCreateDoubleMatrix(0, 0, mxREAL);
    
} // egreedyQParams2mat

/**
 * Utility function to write BayesQParams from protocol buffers to matlab.
 */
mxArray* bayesQParams2mat
(
 const dec_brl::proto::ExperimentSetup_BayesQParams& params
)
{
    return mxCreateDoubleMatrix(0, 0, mxREAL);
    
} // bayesQParams2mat

/**
 * Utility function to write modelBayesParams from protocol buffers to matlab.
 */
mxArray* modelBayesParams2mat
(
 const dec_brl::proto::ExperimentSetup_ModelBayesParams& params
)
{
    return mxCreateDoubleMatrix(0, 0, mxREAL);
    
} // modelBayesParams2mat

/**
 * Utility function to write FactoredMDP from protocol buffers to matlab.
 */
mxArray* mdp2mat
(
 const dec_brl::proto::FactoredMDP& mdp
)
{
    using namespace dec_brl::proto;
    
    //**************************************************************************
    //  Setup the size of the structure array (1,1) - i.e. just one structure.
    //**************************************************************************
    const mwSize DIMS[] = {1,1};
    mwSize N_DIMS = 1;
    
    //**************************************************************************
    //  Setup the field names for the structure
    //**************************************************************************
    int N_FIELDS = 7;
    const char* FIELDNAMES[] = {"name", "description", "gamma", "states",
        "actions", "rewards", "transitions"};
    
    //**************************************************************************
    //  Allocate structure array
    //**************************************************************************
    mxArray* pStruct = mxCreateStructArray(N_DIMS,DIMS,N_FIELDS,FIELDNAMES);
    
    //**************************************************************************
    //  Fill in scalar fields
    //**************************************************************************
    mxArray* pName = mxCreateString(mdp.name().c_str());
    mxSetField(pStruct, 0, "name", pName);
    
    mxArray* pDescription = mxCreateString(mdp.description().c_str());
    mxSetField(pStruct,0,"description",pDescription);
    
    mxArray* pGamma = mxCreateDoubleScalar(mdp.gamma());
    mxSetField(pStruct,0,"gamma",pGamma);
    
    //**************************************************************************
    //  Fill in state variable domains
    //**************************************************************************
    const char* VAR_FIELDS[] = {"id","size"};
    int nStates = mdp.states_size();
    mxArray* pStates=mxCreateStructMatrix(1,nStates,2, VAR_FIELDS);
    for(int k=0; k<nStates; ++k)
    {
        double id = mdp.states(k).id();
        double size = mdp.states(k).size();
        mxArray* pId = mxCreateDoubleScalar(id);
        mxArray* pSize = mxCreateDoubleScalar(size);
        mxSetField(pStates, k, "id", pId);
        mxSetField(pStates, k, "size", pSize);
    }
    mxSetField(pStruct,0,"states",pStates);
    
    //**************************************************************************
    //  Fill in action variable domains
    //**************************************************************************
    int nActions = mdp.actions_size();
    mxArray* pActions=mxCreateStructMatrix(1,nActions,2, VAR_FIELDS);
    for(int k=0; k<nStates; ++k)
    {
        double id = mdp.actions(k).id();
        double size = mdp.actions(k).size();
        mxArray* pId = mxCreateDoubleScalar(id);
        mxArray* pSize = mxCreateDoubleScalar(size);
        mxSetField(pActions, k, "id", pId);
        mxSetField(pActions, k, "size", pSize);
    }
    mxSetField(pStruct,0,"actions",pActions);
    
    //**************************************************************************
    //  Fill in rewards
    //**************************************************************************
    // TODO will need separate function for this
    
    //**************************************************************************
    //  Fill in transitions
    //**************************************************************************
    // TODO will need separate function for this
    
    //**************************************************************************
    //  Return the finished structure
    //**************************************************************************
    return pStruct;
    
} // mdp2mat

/**
 * Utility function to convert protocol buffer experimental setup into
 * matlab structure.
 */
mxArray* setup2mat(const dec_brl::proto::ExperimentSetup& setup)
{
    using namespace dec_brl::proto;
    
    //**************************************************************************
    //  Setup the size of the structure array (1,1) - i.e. just one structure.
    //**************************************************************************
    const mwSize DIMS[] = {1,1};
    mwSize N_DIMS = 1;
    
    //**************************************************************************
    //  Setup the field names for the structure
    //**************************************************************************
    int N_FIELDS = 10;
    const char* FIELDNAMES[] =
        {"name", "description", "timesteps", "episodes", "seed", "learner",
         "params_EGreedyQ","params_BayesQ","params_ModelBayes","problem"};
    
    //**************************************************************************
    //  Allocate structure array
    //**************************************************************************
    mxArray* pStruct = mxCreateStructArray(N_DIMS,DIMS,N_FIELDS,FIELDNAMES);
    
    //**************************************************************************
    //  Fill in scalar fields
    //**************************************************************************
    mxArray* pName = mxCreateString(setup.name().c_str());
    mxSetField(pStruct, 0, "name", pName);
    
    mxArray* pDescription = mxCreateString(setup.description().c_str());
    mxSetField(pStruct,0,"description",pDescription);
    
    mxArray* pTimesteps = mxCreateDoubleScalar(setup.timesteps());
    mxSetField(pStruct,0,"timesteps",pTimesteps);
    
    mxArray* pEpisodes = mxCreateDoubleScalar(setup.episodes());
    mxSetField(pStruct,0,"episodes",pEpisodes);
    
    std::string algorithm = ExperimentSetup_Algorithm_Name(setup.learner());
    mxArray* pAlgorithm = mxCreateString(algorithm.c_str());
    mxSetField(pStruct,0,"learner",pAlgorithm);
    
    //**************************************************************************
    //  Fill in random seeds
    //**************************************************************************
    mxArray* pSeeds = mxCreateNumericMatrix(1, setup.seed_size(),
                                            mxUINT32_CLASS, mxREAL);
    
    uint32_t* pSeedData = static_cast<uint32_t*>(mxGetData(pSeeds));
    
    for(int k=0; k<setup.seed_size(); ++k)
    {
        pSeedData[k] = setup.seed(k);
    }
    
    //**************************************************************************
    //  Fill any algorithm parameters that are present
    //**************************************************************************
    if(setup.has_params_egreedyq())
    {
        mxArray* pEGreedyQParams = egreedyQParams2mat(setup.params_egreedyq());
        mxSetField(pStruct,0,"params_EGreedyQ",pEGreedyQParams);
    }
    
    if(setup.has_params_bayesq())
    {
        mxArray* pBayesQ = bayesQParams2mat(setup.params_bayesq());
        mxSetField(pStruct,0,"params_BayesQ",pBayesQ);
    }
    
    if(setup.has_params_modelbayes())
    {
        mxArray* pModelBayes = modelBayesParams2mat(setup.params_modelbayes());
        mxSetField(pStruct,0,"params_ModelBayes",pModelBayes);
    }
    
    //**************************************************************************
    //  Fill in Factored MDP
    //**************************************************************************
    if(setup.has_problem())
    {
        mxArray* pMdp = mdp2mat(setup.problem());
        mxSetField(pStruct,0,"problem",pMdp);
    }
    
    //**************************************************************************
    //  Return the finished structure
    //**************************************************************************
    return pStruct;
    
} // function setup2mat


/**
 * Matlab interface function.
 * @param[in] nlhs number of (left-handside) output arguments from Matlab.
 * @param[out] plhs Matlab output arguments.
 * @param[in] nrhs number of (right-handside) input arguments form Matlab.
 * @param[in] prhs Matlab input arguments.
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    //**************************************************************************
    //  Try to get filename from input arguments
    //**************************************************************************
    if(1>nrhs)
    {
        mexErrMsgIdAndTxt("READRESULT:ARGS", "Filename must be specified.");
    }
    
    const mxArray* matFilename = prhs[0];
    if(!mxIsChar(matFilename))
    {
        mexErrMsgIdAndTxt("READRESULT:ARGTYPE",
                          "Filename argument must be char array.");
    }
    
    const char* const psFilename = mxArrayToString(matFilename);
    
    if(NULL==psFilename)
    {
        mexErrMsgIdAndTxt("READRESULT:ARGTYPE:INTERNAL",
                          "Internal error reading filename.");
    }
    
    mexPrintf("Requested File: %s\n",psFilename);
    
    //**************************************************************************
    //  Try to Parse file
    //**************************************************************************
    using namespace dec_brl;
    try
    {
        ProtoReader reader(psFilename);
        plhs[0] = setup2mat(reader.getSetup());
        //while(reader.hasExperiment())
        //{
        //  const proto::ExperimentSetup& setup = reader.getSetup();
        //  mexPrintf("NUMBER OF OUTCOMES: %d",reader.getNumOfOutcomes());
            
        //} // outer while loop
        
    }
    catch (std::exception& e)
    {
        mexErrMsgIdAndTxt("READRESULT:CATCH", e.what());
    }
    
} // mexFunction
