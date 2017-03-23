#include <stdlib.h>
#include "common.h"
#include "matlab.h"

static string matlabprog = "matlab -nojvw -nosplash -nodesktop -r ";

int matlabCall(const std::string& cmd)
{
    return system((matlabprog+cmd+", exit").c_str());
}
