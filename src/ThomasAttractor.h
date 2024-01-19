#pragma once
#include "PresetManager.hpp"
#include "Attractor.h"
#include "MLPAttractor.h"

struct ThomasParameters : public AbstractAttractorParameters{
    ofParameter<float> b;

    ThomasParameters() : AbstractAttractorParameters(){
        b.set("b", 0.18,0.001,1.0);
    }

    void calculateDynamics(glm::vec3 const &state, glm::vec3 &dtt) const override {
        dtt.x = sin(state.y) - b * state.x;
        dtt.y = sin(state.z) - b * state.y;
        dtt.z = sin(state.x) - b * state.z;
    }

    void randomise() override {
        b = ofClamp(b + ofRandomf() * randomiseAmt, b.getMin(), b.getMax());
    }

    vector<ofParameter<float>> getCurrentParams() const override {
        return {b};
    }

};



inline void to_json(ofJson& j, const ThomasParameters& d) {
    j = ofJson{{"b", d.b.get()}};
}

inline void from_json(const ofJson& j, ThomasParameters& p) {
    p.b = j.at("b").get<float>();
}


inline void to_json(ofJson& j, const AllParameters<ThomasParameters>& s) {
    j = ofJson{
        {"aParams", s.aParams},
        {"sParams", s.sParams}
    };
    
}

inline void from_json(const ofJson& j, AllParameters<ThomasParameters>& p) {
    j.at("aParams").get_to(p.aParams);
    j.at("sParams").get_to(p.sParams);
}

template <>
inline void PresetManager<ThomasParameters>::updateAttractorParams(AbstractAttractorParameters &params, float amt) {
    //cast to ThomasParameters
    ThomasParameters* p = static_cast<ThomasParameters*>(&params);
    p->b = ofLerp(startParams->aParams.b, targetParams->aParams.b, amt);
}

template <>
inline string PresetManager<ThomasParameters>::getPresetFileName() const {
    return "thomas_presets.json";
}

class Thomas : public Attractor<ThomasParameters> {
    public:
        Thomas(shared_ptr<ofParameter<string>> &status) : Attractor<ThomasParameters>({0.4, 0.3, 0.1}, make_shared<ThomasParameters>()) {
            presetManager = new PresetManager<ThomasParameters>(status);
            setupAnn();
        }

        int getNumOutputs() const override {
            return 1;
        }

        vector<ofParameter<float>> getParameterValues() const override {
            return params->getCurrentParams();
        }
};