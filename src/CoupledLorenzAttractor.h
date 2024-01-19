#pragma once
#include "PresetManager.hpp"
#include "Attractor.h"
#include "MLPAttractor.h"

struct CoupledLorenzParameters : public AbstractAttractorParameters{
    ofParameter<float> b;
    ofParameter<float> o;
    ofParameter<float> r1;
    ofParameter<float> r2;
    ofParameter<float> e;


    CoupledLorenzParameters() : AbstractAttractorParameters(){
        b.set("b", 8.0/3.0,0.1,10.0);
        o.set("o", 10,0.01,20.0);
        r1.set("r1", 35,0.1,50);
        r2.set("r2", 1.15,0.001,25.0);
        e.set("e", 2.85,0.01,10.0);
    }

    void calculateDynamics(glm::vec3 const &state, glm::vec3 &dtt) const override {
        dtt.x = o  * (state.y - state.x);
        dtt.y = r1 *  state.x - state.y - state.x * state.z;     
        dtt.z = -b  *  state.z + state.x * state.y;
    }

    void calculateDynamics2(glm::vec3 const &state, glm::vec3 &dtt, glm::vec3 const &hidden_state) const {
        dtt.x = o  * (state.y - state.x) + e * (hidden_state.x - state.x);
        dtt.y = r2 *  state.x - state.y - state.x * state.z;     
        dtt.z = -b  *  state.z + state.x * state.y;
    }

    void randomise() override {
        b  = ofClamp(b  + ofRandomf() * randomiseAmt, b.getMin(),  b.getMax());
        o  = ofClamp(o  + ofRandomf() * randomiseAmt, o.getMin(),  o.getMax());
        r1 = ofClamp(r1 + ofRandomf() * randomiseAmt, r1.getMin(), r1.getMax());
        r2 = ofClamp(r2 + ofRandomf() * randomiseAmt, r2.getMin(), r2.getMax());
        e  = ofClamp(e  + ofRandomf() * randomiseAmt, e.getMin(),  e.getMax());
    }

    vector<ofParameter<float>> getCurrentParams() const override {
        return {b, o, r1, r2, e};
    }

};



inline void to_json(ofJson& j, const CoupledLorenzParameters& d) {
    j = ofJson{
        {"b", d.b.get()},
        {"o", d.o.get()},
        {"r1", d.r1.get()},
        {"r2", d.r2.get()},
        {"e", d.e.get()},
    };
}

inline void from_json(const ofJson& j, CoupledLorenzParameters& p) {
    p.b  = j.at("b").get<float>();
    p.o  = j.at("o").get<float>();
    p.r1 = j.at("r1").get<float>();
    p.r2 = j.at("r2").get<float>();
    p.e  = j.at("e").get<float>();
}


inline void to_json(ofJson& j, const AllParameters<CoupledLorenzParameters>& s) {
    j = ofJson{
            {"aParams", s.aParams},
            {"sParams", s.sParams}
    };
    
}

inline void from_json(const ofJson& j, AllParameters<CoupledLorenzParameters>& p) {
    j.at("aParams").get_to(p.aParams);
    j.at("sParams").get_to(p.sParams);
}

template <>
inline void PresetManager<CoupledLorenzParameters>::updateAttractorParams(AbstractAttractorParameters &params, float amt) {
    //cast params to CoupledLorenzParameters
    CoupledLorenzParameters *p = static_cast<CoupledLorenzParameters*>(&params);

    p->b  = ofLerp(startParams->aParams.b,  targetParams->aParams.b, amt);
    p->o  = ofLerp(startParams->aParams.o,  targetParams->aParams.o, amt);
    p->r1 = ofLerp(startParams->aParams.r1, targetParams->aParams.r1, amt);
    p->r2 = ofLerp(startParams->aParams.r2, targetParams->aParams.r2, amt);
    p->e  = ofLerp(startParams->aParams.e,  targetParams->aParams.e, amt);
}


template <>
inline string PresetManager<CoupledLorenzParameters>::getPresetFileName() const {
    return "coupled_lorenz_presets.json";
}

class CoupledLorenz : public Attractor<CoupledLorenzParameters> {
    public:

        glm::vec3 hidden_state;

        CoupledLorenz(shared_ptr<ofParameter<string>> &status) : Attractor<CoupledLorenzParameters>({0.4, 0.3, 0.1}, make_shared<CoupledLorenzParameters>()) {
            presetManager = new PresetManager<CoupledLorenzParameters>(status);
            hidden_state = state;
            setupAnn();
        }

        int getNumOutputs() const override {
            return 5;
        }

        vector<ofParameter<float>> getParameterValues() const override {
            return params->getCurrentParams();
        }

        void integrate(const double dt) override {
            glm::vec3 k1, k2, k3, k4;
            CoupledLorenzParameters *p = static_cast<CoupledLorenzParameters*>(params.get());
            p->calculateDynamics(hidden_state, k1);
            p->calculateDynamics(hidden_state + dt / 2 * k1, k2);
            p->calculateDynamics(hidden_state + dt / 2 * k2, k3);
            p->calculateDynamics(hidden_state + dt * k3, k4);

            hidden_state += dt / 6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);

            p->calculateDynamics2(state, k1, hidden_state);
            p->calculateDynamics2(state + dt / 2 * k1, k2, hidden_state);
            p->calculateDynamics2(state + dt / 2 * k2, k3, hidden_state);
            p->calculateDynamics2(state + dt * k3, k4, hidden_state);

            state += dt / 6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        }

};