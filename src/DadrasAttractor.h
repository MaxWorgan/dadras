//
//  DadrasAttractor.h
//  dadras
//
//  Created by Max Worgan on 01/12/2023.
//

#pragma once
#include "ofJson.h"
#include "AbstractAttractorParameters.h"
#include "Attractor.h"
#include "genann.h"

struct DadrasParameters : public AbstractAttractorParameters{

    ofParameter<float> a, b, c, d, r;
    
    DadrasParameters() : AbstractAttractorParameters(){
        a.set("a", 3.0,0.1,10.0);
        b.set("b", 2.7,0.1,10.0);
        c.set("c", 1.7,0.1,10.0);
        d.set("d", 2.0,0.1,10.0);
        r.set("r", 9.0,0.1,10.0);
    }

    ostream& operator<<(ostream& os) {
        return os << "a: " << a << endl
                  << "b: " << b << endl
                  << "c: " << c << endl
                  << "d: " << d << endl
                  << "r: " << r << endl;
    }
        
    void calculateDynamics(glm::vec3 const &state, glm::vec3 &ddt) const override {
        ddt.x = state.y - a * state.x + b * state.y * state.z;
        ddt.y = c * state.y - state.x * state.z + state.z;
        ddt.z = d * state.x * state.y - r * state.z;
    }

    void randomise() override{
        a = ofClamp(a + ofRandomf() * randomiseAmt, a.getMin(), a.getMax());
        b = ofClamp(b + ofRandomf() * randomiseAmt, b.getMin(), b.getMax());
        c = ofClamp(c + ofRandomf() * randomiseAmt, c.getMin(), c.getMax());
        d = ofClamp(d + ofRandomf() * randomiseAmt, d.getMin(), d.getMax());
        r = ofClamp(r + ofRandomf() * randomiseAmt, r.getMin(), r.getMax());
    }

    vector<ofParameter<float>> getCurrentParams() const override {
        return {a,b,c,d,r};
    } 
    
};

inline void to_json(ofJson& j, const DadrasParameters& d) {
    j = ofJson{{"a", d.a.get()}, {"b", d.b.get()}, {"c", d.c.get()}, {"d", d.d.get()}, {"r", d.r.get()}};
}

inline void from_json(const ofJson& j, DadrasParameters& p) {
    p.a = j.at("a").get<float>();
    p.b = j.at("b").get<float>();
    p.c = j.at("c").get<float>();
    p.d = j.at("d").get<float>();
    p.r = j.at("r").get<float>();
}

inline void to_json(ofJson& j, const AllParameters<DadrasParameters>& s) {
    j = ofJson{
        {"aParams", s.aParams},
        {"sParams", s.sParams}
    };
    
}

inline void from_json(const ofJson& j, AllParameters<DadrasParameters>& p) {
    j.at("aParams").get_to(p.aParams);
    j.at("sParams").get_to(p.sParams);
}

template <>
inline void PresetManager<DadrasParameters>::updateAttractorParams(AbstractAttractorParameters &dParams, float amt) {
    //cast to the correct type
    DadrasParameters &p = static_cast<DadrasParameters&>(dParams);

    currentParams->aParams.a = ofLerp(startParams->aParams.a, targetParams->aParams.a, amt);
    currentParams->aParams.b = ofLerp(startParams->aParams.b, targetParams->aParams.b, amt);
    currentParams->aParams.c = ofLerp(startParams->aParams.c, targetParams->aParams.c, amt);
    currentParams->aParams.d = ofLerp(startParams->aParams.d, targetParams->aParams.d, amt);
    currentParams->aParams.r = ofLerp(startParams->aParams.r, targetParams->aParams.r, amt);
}

template <>
inline void PresetManager<DadrasParameters>::savePresetsToJson() { 
    ofJson j = presets;
    ofSavePrettyJson(PRESETS_FILE, j);
    status->set("Saved presets to " + PRESETS_FILE);
}
template<>
inline void PresetManager<DadrasParameters>::loadPresetsFromJson() {
    ofJson j = ofLoadJson(PRESETS_FILE);
    try {
        presets = j.template get<std::vector<AllParameters<DadrasParameters>>>();
        status->set("Loaded presets from " + PRESETS_FILE);
        ofLog() << status-> get();
    } catch (nlohmann::json_abi_v3_11_2::detail::out_of_range& e){
        status->set("Failed to load presets " + std::string(e.what()));
        ofLog() << status->get();
    }
}

template <>
inline AllParameters<DadrasParameters>::AllParameters(DadrasParameters a, SimulationParameters s) : aParams(a), sParams(s) {}

template <>
inline AllParameters<DadrasParameters>::AllParameters() : aParams(), sParams() {}

template <>
inline PresetManager<DadrasParameters>::PresetManager(shared_ptr<ofParameter<string>> &s) : BasePresetManager(s) {
        targetParams = new AllParameters<DadrasParameters>();
        startParams  = new AllParameters<DadrasParameters>();
        presets.resize(10);
        currentParams = &presets[0];
        status->set("Scene 1");
        currentScene = 1;
        loadPresetsFromJson();
}


class Dadras : public Attractor<DadrasParameters>{

    public:
        Dadras(shared_ptr<ofParameter<string>> status) : Attractor<DadrasParameters>({0.1, 0.1, 0.1}, make_shared<DadrasParameters>()){
            presetManager = new PresetManager<DadrasParameters>(status);
            setupAnn();
        }

        ~Dadras(){
            delete presetManager;
            genann_free(ann);
        }

        int getNumOutputs() const override{
            return 5;
        }

        vector<ofParameter<float>> getParameterValues() const override{
            return params->getCurrentParams();
        }
};