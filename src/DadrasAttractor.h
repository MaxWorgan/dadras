//
//  DadrasAttractor.h
//  dadras
//
//  Created by Max Worgan on 01/12/2023.
//

#pragma once
#include "ofJson.h"
#include "ofMain.h"

struct DadrasParameters{
    
    ofParameter<float> a, b, c, d, r;
    
    DadrasParameters() : a(3.0), b(2.7), c(1.7), d(2.0), r(9.0) {
        a.set("a", 3.0,0.1,10.0);
        b.set("b", 2.7,0.1,10.0);
        c.set("c", 1.7,0.1,10.0);
        d.set("d", 2.0,0.1,10.0);
        r.set("r", 9.0,0.1,10.0);

    }
    
    void randomize() {
        a = ofRandom(a.getMin(), a.getMax());
        b = ofRandom(b.getMin(), b.getMax());
        c = ofRandom(c.getMin(), c.getMax());
        d = ofRandom(d.getMin(), d.getMax());
        r = ofRandom(r.getMin(), r.getMax());
    }
    
    void microRandomize(float amt) {
        a = ofClamp(a + ofRandomf() * amt, a.getMin(), a.getMax());
        b = ofClamp(b + ofRandomf() * amt, b.getMin(), b.getMax());
        c = ofClamp(c + ofRandomf() * amt, c.getMin(), c.getMax());
        d = ofClamp(d + ofRandomf() * amt, d.getMin(), d.getMax());
        r = ofClamp(r + ofRandomf() * amt, r.getMin(), r.getMax());
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


struct Dadras {
    
    glm::vec3 state;
    
    Dadras() : state({0.1,0.1,0.1}){}
    
    Dadras(float a, float b, float c, float d, float r) : state({0.1,0.1,0.1}) {}
    
    void calculateDynamics(glm::vec3 const &state, const DadrasParameters &params, glm::vec3 &ddt) const {
        ddt.x = state.y - params.a * state.x + params.b * state.y * state.z;
        ddt.y = params.c * state.y - state.x * state.z + state.z;
        ddt.z = params.d * state.x * state.y - params.r * state.z;
    }
    
    
    void integrate(const double dt, const DadrasParameters &params) {
        glm::vec3 k1, k2, k3, k4;

        calculateDynamics(state, params, k1);
        calculateDynamics(state + dt / 2 * k1, params, k2);
        calculateDynamics(state + dt / 2 * k2, params, k3);
        calculateDynamics(state + dt * k3, params, k4);

        state += dt / 6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    }
    
};
