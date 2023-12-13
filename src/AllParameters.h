#pragma once
#include "SimulationParameters.h"

struct BaseAllParameters{};

template <typename T>
struct AllParameters : public BaseAllParameters{
        
    T aParams;
    SimulationParameters sParams;
    AllParameters(T a, SimulationParameters s) : aParams(a), sParams(s){}
    AllParameters(){}
    
};