//
//  PresetManager.hpp
//  dadras
//
//  Created by Max Worgan on 01/12/2023.
//

#ifndef PresetManager_hpp
#define PresetManager_hpp

#include <stdio.h>
#include "DadrasAttractor.h"
#include "ofJson.h"
#include "SimulationParameters.h"

class PresetManager {
    
    //parameters to move to
    AllParameters* targetParams;
    AllParameters* startParams;
    AllParameters* currentParams;

    std::vector<AllParameters> presets;
    
    float lerpTime;
    float lerpTimeEnd;
    bool lerp;
    
    
public:
    
    PresetManager() : lerpTime(2.0), lerpTimeEnd(0), lerp(false), currentScene(1) {
        targetParams = new AllParameters();
        startParams  = new AllParameters();
        presets.resize(10);
        loadPresetsFromJson();
        currentParams = &presets[1];
    }
    
    int currentScene;
    void update(AllParameters &params);
    void keyPressed(int key);
    void setLerpTime(float &time);
    void savePresetsToJson();
    void loadPresetsFromJson();

};

#endif /* PresetManager_hpp */
