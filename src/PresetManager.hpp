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

class PresetManager {
    
    //parameters to move to
    DadrasParameters* targetParams;
    DadrasParameters* startParams;
    DadrasParameters* currentParams;

    std::vector<DadrasParameters> presets;
    
    float lerpTime;
    float lerpTimeEnd;
    bool lerp;
    
    
public:
    
    PresetManager() : lerpTime(2.0), lerpTimeEnd(0), lerp(false) {
        targetParams = new DadrasParameters();
        startParams  = new DadrasParameters();
        presets.resize(10);
    }
    
    void update(DadrasParameters &attractorParams);
    void keyPressed(int key);
    void setLerpTime(float time);

};

#endif /* PresetManager_hpp */
