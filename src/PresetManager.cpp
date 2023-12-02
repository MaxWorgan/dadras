//
//  PresetManager.cpp
//  dadras
//
//  Created by Max Worgan on 01/12/2023.
//

#include "PresetManager.hpp"


void PresetManager::update(DadrasParameters &attractorParams) {
    
    if(lerp && (*targetParams != attractorParams)) {
        float t = ofGetElapsedTimef();
        float amt  = 1.0 - (lerpTimeEnd - t) / lerpTime;
        if(amt > 1.0){
            amt = 1.0;
            lerp = false;
        }
        attractorParams.a = ofLerp(startParams->a, targetParams->a,amt);
        attractorParams.b = ofLerp(startParams->b, targetParams->b,amt);
        attractorParams.c = ofLerp(startParams->c, targetParams->c,amt);
        attractorParams.d = ofLerp(startParams->d, targetParams->d,amt);
        attractorParams.r = ofLerp(startParams->r, targetParams->r,amt);
    }
    
    currentParams = &attractorParams;
}

// Hack to enable SHIFT + numbers to work less horribly
static const std::map<char32_t,int> keycodes = {{'!', 1},{'"',2},{U'£',3},{'$',4},{'%',5},{'^',6},{'&',7},{'*',8},{'(', 9}, {')',0}};

void PresetManager::keyPressed(int key) {
    if (ofGetKeyPressed(OF_KEY_CONTROL) && (key >= '0' && key <= '9')){
        ofLog() << "Saved current state in " << key - '0';
        presets[key - '0'] = *currentParams;
    } else if (key >= '0' && key <= '9'){
        ofLog() << "Lerping to state " << key - '0';
        lerp = true;
        lerpTimeEnd = ofGetElapsedTimef() + lerpTime;
        *startParams = *currentParams;
        *targetParams = presets[key - '0'];
    } else if (keycodes.find(key) != keycodes.end()){
        ofLog() << "Moving to state " << keycodes.at(key);
        lerp = true;
        lerpTimeEnd = ofGetElapsedTimef();
        *startParams = *currentParams;
        *targetParams = presets[keycodes.at(key)];
    }

}
