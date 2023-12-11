//
//  PresetManager.cpp
//  dadras
//
//  Created by Max Worgan on 01/12/2023.
//

#include "PresetManager.hpp"

#define PRESETS_FILE "presets.json"

void PresetManager::update(AllParameters &params){
    
    if(lerp) {
        float t = ofGetElapsedTimef();
        float amt  = 1.0 - (lerpTimeEnd - t) / lerpTime;
        if(amt > 1.0){
            amt = 1.0;
            lerp = false;
            status->set("Scene " + to_string(currentScene));
        }
        params.dParams.a = ofLerp(startParams->dParams.a, targetParams->dParams.a,amt);
        params.dParams.b = ofLerp(startParams->dParams.b, targetParams->dParams.b,amt);
        params.dParams.c = ofLerp(startParams->dParams.c, targetParams->dParams.c,amt);
        params.dParams.d = ofLerp(startParams->dParams.d, targetParams->dParams.d,amt);
        params.dParams.r = ofLerp(startParams->dParams.r, targetParams->dParams.r,amt);
        
        params.sParams.lineMaxSize = ofLerp(startParams->sParams.lineMaxSize, targetParams->sParams.lineMaxSize, amt);
        params.sParams.lineMinLength = ofLerp(startParams->sParams.lineMinLength, targetParams->sParams.lineMinLength, amt);
        params.sParams.alpha = ofLerp(startParams->sParams.alpha, targetParams->sParams.alpha, amt);
        params.sParams.mixFactor = ofLerp(startParams->sParams.mixFactor, targetParams->sParams.mixFactor,amt);
        params.sParams.numIterations = ofLerp(startParams->sParams.numIterations, targetParams->sParams.numIterations,amt);
        params.sParams.dt = ofLerp(startParams->sParams.dt, targetParams->sParams.dt,amt);
        params.sParams.lerpTime = ofLerp(startParams->sParams.lerpTime, targetParams->sParams.lerpTime,amt);
        params.sParams.skipFrames = ofLerp(startParams->sParams.skipFrames, targetParams->sParams.skipFrames,amt);
        
        
    }
    
    currentParams = &params;
}

// Hack to enable SHIFT + numbers to work less horribly
static const std::map<char32_t,int> keycodes = {{'!', 1},{'"',2},{U'£',3},{'$',4},{'%',5},{'^',6},{'&',7},{'*',8},{'(', 9}, {')',0}};

void PresetManager::keyPressed(int key) {
    if (ofGetKeyPressed(OF_KEY_CONTROL) && (key >= '0' && key <= '9')){
        status->set("Saved current Scene in " + to_string(key - '0'));
        ofLog() << status->get();
        presets[key - '0'] = *currentParams;
    } else if (key >= '0' && key <= '9'){
        status->set("Lerping to Scene "  + to_string(key - '0'));
        currentScene = key - '0';
        lerp = true;
        lerpTimeEnd = ofGetElapsedTimef() + lerpTime;
        *startParams = *currentParams;
        *targetParams = presets[key - '0'];
    } else if (keycodes.find(key) != keycodes.end()){
        currentScene = keycodes.at(key);
        lerp = true;
        lerpTimeEnd = ofGetElapsedTimef();
        *startParams = *currentParams;
        *targetParams = presets[keycodes.at(key)];
    } else if (key == 's') {
        savePresetsToJson();
    } else if (key == 'l'){
        loadPresetsFromJson();
    }
    

}

void PresetManager::setLerpTime(float &time){
    lerpTime = time;
}

void PresetManager::savePresetsToJson() { 
    ofJson j = presets;
    ofSavePrettyJson(PRESETS_FILE, j);
    status->set("Saved presets to " PRESETS_FILE);
}

void PresetManager::loadPresetsFromJson() {
    ofJson j = ofLoadJson(PRESETS_FILE);
    try {
        presets = j.template get<std::vector<AllParameters>>();
        status->set("Loaded presets from " PRESETS_FILE);
        ofLog() << status-> get();
    } catch (nlohmann::json_abi_v3_11_2::detail::out_of_range& e){
        status->set("Failed to load presets " + std::string(e.what()));
        ofLog() << status->get();
    }
}

