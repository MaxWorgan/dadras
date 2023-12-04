//
//  SimulationParameters.h
//  dadras
//
//  Created by Max Worgan on 02/12/2023.
//

#ifndef SimulationParameters_h
#define SimulationParameters_h

#include "ofJson.h"
#include "ofMain.h"

struct SimulationParameters{
   
    
    ofParameter<float>  lineMaxSize;
    ofParameter<float>  lineMinLength;
    ofParameter<float>  alpha;
    ofParameter<float>  mixFactor;
    ofParameter<int>    numIterations;
    ofParameter<float>  dt;
    ofParameter<float>  lerpTime;
    ofParameter<int>    skipFrames;
    ofParameter<float>  microRandomizeAmt;
    
    SimulationParameters() {
        lineMaxSize.set("Ribbon Size", 40,5,100);
        lineMinLength.set("Ribbon min length",50,5,100);
        alpha.set("Alpha", 20,1,255);
        mixFactor.set("Mix", 1.0,1.0,0.0);
        numIterations.set("Num", 3,1,50);
        dt.set("dt", 0.003,0.00001,0.01);
        lerpTime.set("Lerp Time", 2,0.1,10);
        skipFrames.set("Skip Frames", 2, 1, 10);
        microRandomizeAmt.set("Randomize Amt", 0.5, 0.001,1.0);
    }
};

inline void to_json(ofJson& j, const SimulationParameters& s) {
    j = ofJson{
        {"lineMaxSize", s.lineMaxSize.get()},
        {"lineMinLength", s.lineMinLength.get()},
        {"alpha", s.alpha.get()},
        {"mixFactor", s.mixFactor.get()},
        {"numIterations", s.numIterations.get()},
        {"dt", s.dt.get()},
        {"lerpTime", s.lerpTime.get()},
        {"skipFrames", s.skipFrames.get()},
        {"microRandomizeAmt", s.microRandomizeAmt.get()},
    };
}

inline void from_json(const ofJson& j, SimulationParameters& p) {
    p.lineMaxSize = j.at("lineMaxSize").get<float>();
    p.lineMinLength = j.at("lineMinLength").get<float>();
    p.alpha = j.at("alpha").get<float>();
    p.mixFactor = j.at("mixFactor").get<float>();
    p.numIterations = j.at("numIterations").get<float>();
    p.dt = j.at("dt").get<double>();
    p.lerpTime = j.at("lerpTime").get<float>();
    p.skipFrames = j.at("skipFrames").get<float>();
    p.microRandomizeAmt = j.at("microRandomizeAmt").get<float>();
}

struct AllParameters{
        
    DadrasParameters dParams;
    SimulationParameters sParams;
    
    AllParameters(){}
    
    
    
};

inline void to_json(ofJson& j, const AllParameters& s) {
    j = ofJson{
        {"dParams", s.dParams},
        {"sParams", s.sParams}
    };
    
}

inline void from_json(const ofJson& j, AllParameters& p) {
    j.at("dParams").get_to(p.dParams);
    j.at("sParams").get_to(p.sParams);
    
}


#endif /* SimulationParameters_h */
