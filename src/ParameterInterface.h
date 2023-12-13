#pragma once
#include <vector>
#include "ofParameter.h"

using namespace std;

class ParameterInterface {
    public:
        virtual vector<ofParameter<float>> getParameterValues() const = 0;
};