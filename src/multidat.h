// ****************************************
// * TMultiData - Multiple resource array *
// ****************************************

#pragma once

#include "revenant.h"

#define MAXMULTIOFFSETS 256

_CLASSDEF(TMultiData)
class TMultiData
{
  public:
    int32_t numoffsets;                     // Number of resource offsets
    OFFSET names[MAXMULTIOFFSETS];      // Array of offsets to resource names
    OFFSET offsets[MAXMULTIOFFSETS];    // Array of offsets to resources
};


