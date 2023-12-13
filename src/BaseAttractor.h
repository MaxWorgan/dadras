#pragma once
#include "BasePresetManager.h"
#include "AbstractAttractorParameters.h"
#include "ofxControlPanel.h"
#include "MLPAttractor.h"

using namespace std;

class BaseAttractor : public MLPAttractor{
    public:
        
        BasePresetManager *presetManager;

        glm::vec3 state;
        shared_ptr<AbstractAttractorParameters> params;

        BaseAttractor(glm::vec3 initState, shared_ptr<AbstractAttractorParameters> p) : state(initState), params(p), presetManager(nullptr), MLPAttractor() {}
        virtual void integrate(const double dt) = 0;
        virtual void setupGui(ofxControlPanel &gui) = 0;
        virtual ~BaseAttractor() = default;

    
};

