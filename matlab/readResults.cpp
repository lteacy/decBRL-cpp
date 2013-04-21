#include <matrix.h>
#include <mex.h>
#include "dec_brl/ProtoReader.h"
#include <string>

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

} // mexFunction
