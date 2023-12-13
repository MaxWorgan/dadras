//
//  SimulationParameters.h
//  dadras
//
//  Created by Max Worgan on 02/12/2023.
//

#pragma once
#include "ofJson.h"
#include "ofMain.h"

struct SimulationParameters{
   
    
    ofParameter<float> lineMaxSize;
    ofParameter<float> lineMinLength;
    ofParameter<float> alpha;
    ofParameter<float> mixFactor;
    ofParameter<int>   numIterations;
    ofParameter<float> dt;
    ofParameter<float> lerpTime;
    ofParameter<int>   skipFrames;
    ofParameter<float> rotationSpeedX;
    ofParameter<float> rotationSpeedY;
    ofParameter<float> rotationSpeedZ;
    ofParameter<float> mouseX;
    ofParameter<float> mouseY;
    ofParameter<bool>  autoRotate;
    ofParameter<int>   numTrainingCycles;
    ofParameter<float> learningRate;
    ofParameter<bool>  predict;
    ofParameter<float> audioSignalReduction;
    shared_ptr<ofParameter<string>> status;

    SimulationParameters() {
        lineMaxSize.set("Ribbon Size", 40,5,100);
        lineMinLength.set("Ribbon min length",50,5,100);
        alpha.set("Alpha", 20,1,255);
        mixFactor.set("Mix", 1.0,1.0,0.0);
        numIterations.set("Num", 3,1,50);
        dt.set("dt", 0.003,0.00001,0.01);
        lerpTime.set("Lerp Time", 2,0.1,10);
        skipFrames.set("Skip Frames", 2, 1, 10);
        rotationSpeedX.set("Rotation X", 0.0,0.0,2.0);
        rotationSpeedY.set("Rotation Y", 0.0,0.0,2.0);
        rotationSpeedZ.set("Rotation Z", 0.0,0.0,2.0);
        autoRotate.set("Auto Rotate");
        mouseX.set("X pos", 0.0, 0.0, 1.0);
        mouseY.set("Y pos", 0.0, 0.0, 1.0);
        numTrainingCycles.set("Training cycles", 1, 1,1000);
        learningRate.set("Learning Rate", 0.1, 0.001, 0.9);
        predict.set("predict",false);
        audioSignalReduction.set("Audio squish",0.01,0.0001,0.1);
        status = make_shared<ofParameter<string>>("");
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
        {"skipFrames", s.skipFrames.get()},
        {"learningRate",s.learningRate.get()},
        {"numTrainingCycles", s.numTrainingCycles.get()},
        {"audioSignalReduction", s.audioSignalReduction.get()}
    };
}

inline void from_json(const ofJson& j, SimulationParameters& p) {
    p.lineMaxSize = j.at("lineMaxSize").get<float>();
    p.lineMinLength = j.at("lineMinLength").get<float>();
    p.alpha = j.at("alpha").get<float>();
    p.mixFactor = j.at("mixFactor").get<float>();
    p.numIterations = j.at("numIterations").get<float>();
    p.dt = j.at("dt").get<double>();
    p.skipFrames = j.at("skipFrames").get<float>();
    p.numTrainingCycles = j.at("numTrainingCycles").get<int>();
    p.learningRate = j.at("learningRate").get<float>();
    p.audioSignalReduction = j.at("audioSignalReduction").get<float>();
}
