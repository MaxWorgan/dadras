
#pragma once

#include <stdio.h>
#include "ofParameter.h"
#include "SimulationParameters.h"
#include "AbstractAttractorParameters.h"

class BasePresetManager {
public:
    float lerpTime;
    float lerpTimeEnd;
    bool lerp;
    shared_ptr<ofParameter<string>> status;
    ofParameter<int> currentScene;

    BasePresetManager(shared_ptr<ofParameter<string>> &s) : lerpTime(2.0), lerpTimeEnd(0), lerp(false), status(s), currentScene(1) {}

    virtual ~BasePresetManager() = default;

    virtual void update(SimulationParameters &sParams, AbstractAttractorParameters &aParams) = 0;
    virtual void updateSimulationParams(SimulationParameters &sParams, float amt) = 0;
    virtual void updateAttractorParams(AbstractAttractorParameters &aParams, float amt) = 0;
    virtual void keyPressed(int key) = 0;
    virtual void setLerpTime(float &time) = 0;
    virtual void savePresetsToJson() = 0;
    virtual void loadPresetsFromJson() = 0;
};