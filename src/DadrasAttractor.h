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
    
    float a, b, c, d, r;
    
    DadrasParameters() : a(3.0), b(2.7), c(1.7), d(2.0), r(9.0) {}
    DadrasParameters(float a, float b, float c, float d, float r): a(a), b(b), c(c), d(d), r(r) {}
    
    static constexpr float TOL = 1e-6;
    
    bool operator==(const DadrasParameters& other) {
        return fabs(other.a - a) < TOL &&
               fabs(other.b - b) < TOL &&
               fabs(other.c - c) < TOL &&
               fabs(other.d - d) < TOL &&
               fabs(other.r - r) < TOL;
    }
    bool operator!=(const DadrasParameters& other) {
        return !(*this == other);
    }

 
};

inline void to_json(ofJson& j, const DadrasParameters& d) {
    j = ofJson{{"a", d.a}, {"b", d.b}, {"c", d.c}, {"d", d.d}, {"r", d.r}};
}

inline void from_json(const ofJson& j, DadrasParameters& p) {
    j.at("a").get_to(p.a);
    j.at("b").get_to(p.b);
    j.at("c").get_to(p.c);
    j.at("d").get_to(p.d);
    j.at("r").get_to(p.r);
}


struct Dadras {
    
    glm::vec3 state;
    
    DadrasParameters params;
    
    Dadras() : state({0.1,0.1,0.1}){}
    
    Dadras(const ofJson &j) : state({0.1,0.1,0.1}) {
        params = j.template get<DadrasParameters>();
    }
    
    Dadras(float a, float b, float c, float d, float r) : params(a, b, c, d, r), state({0.1,0.1,0.1}) {}
    
    void calculateDynamics(glm::vec3 const &state, glm::vec3 &ddt) const {
        ddt.x = state.y - params.a * state.x + params.b * state.y * state.z;
        ddt.y = params.c * state.y - state.x * state.z + state.z;
        ddt.z = params.d * state.x * state.y - params.r * state.z;
    }
    
    
    void integrate(const double dt) {
        glm::vec3 k1, k2, k3, k4;

        calculateDynamics(state,k1);
        calculateDynamics(state + dt / 2 * k1, k2);
        calculateDynamics(state + dt / 2 * k2, k3);
        calculateDynamics(state + dt * k3, k4);

        state += dt / 6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    }
    
};
