
#pragma once
#include "ofParameter.h"
using namespace std;


struct AbstractAttractorParameters{
    public:
        ofParameter<float> randomiseAmt;

        AbstractAttractorParameters(){
            randomiseAmt.set("randomise amount", 0.2,0.0,5.0);
        }
        virtual void calculateDynamics(glm::vec3 const &state, glm::vec3 &ddt) const = 0;
        virtual void randomise() = 0;
        virtual vector<ofParameter<float>> getCurrentParams() const = 0;
};