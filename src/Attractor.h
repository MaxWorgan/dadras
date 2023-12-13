#pragma once

#include "ofParameter.h"
#include "BaseAttractor.h"
#include "AbstractAttractorParameters.h"
#include "ofxControlPanel.h"

template <typename T>
class Attractor : public BaseAttractor{

    public:

        Attractor(glm::vec3 initState, shared_ptr<AbstractAttractorParameters> p) : BaseAttractor(initState, p){}


        void integrate(const double dt) override {
            glm::vec3 k1, k2, k3, k4;

            params->calculateDynamics(state, k1);
            params->calculateDynamics(state + dt / 2 * k1, k2);
            params->calculateDynamics(state + dt / 2 * k2, k3);
            params->calculateDynamics(state + dt * k3, k4);

            state += dt / 6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        }

        void setupGui(ofxControlPanel &gui) override {
            for(auto &p : params->getCurrentParams()){
                gui.add(p);
            }
            ofParameter<void> randomise = ofParameter<void>("randomise");
            //this must be some kind of crime
            randomise.addListener(static_cast<T* >(params.get()), &T::randomise);
            gui.addButton(randomise);
            gui.add(params->randomiseAmt);
        }

};
