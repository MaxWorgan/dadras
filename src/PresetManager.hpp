//
//  PresetManager.hpp
//  dadras
//
//  Created by Max Worgan on 01/12/2023.
//

#pragma once

#include <stdio.h>
#include "SimulationParameters.h"
#include "Attractor.h"
#include "BasePresetManager.h"
#include "AllParameters.h"
#include "AbstractAttractorParameters.h"

template <typename T>
class PresetManager : public BasePresetManager {
    AllParameters<T>* targetParams;
    AllParameters<T>* startParams;
    AllParameters<T>  currentParams;
    std::vector<AllParameters<T>> presets;
    // Hack to enable SHIFT + numbers to work less horribly
    const std::map<char32_t,int> keycodes = {{'!', 1},{'"',2},{U'£',3},{'$',4},{'%',5},{'^',6},{'&',7},{'*',8},{'(', 9}, {')',0}};

public:
    PresetManager(shared_ptr<ofParameter<string>> &s) : BasePresetManager(s) {
        presets.resize(10);
        loadPresetsFromJson();
        status->set("Scene 1");
        targetParams = &presets[1];
        startParams  = &presets[1];
        lerp = true;
    }

    ~PresetManager() {
        delete targetParams;
        delete startParams;
    }

    void update(SimulationParameters &sParams, AbstractAttractorParameters &aParams) override {

        if (lerp) {
            float t = ofGetElapsedTimef();
            float amt = 1.0 - (lerpTimeEnd - t) / lerpTime;
            if (amt > 1.0){
                amt = 1.0;
                lerp = false;
                status->set("Scene " + to_string(currentScene));
            }

            updateSimulationParams(sParams, amt);
            updateAttractorParams(aParams, amt);
        }

        currentParams = AllParameters<T>(static_cast<T&>(aParams), sParams);

    }

    void updateSimulationParams(SimulationParameters &sParams, float amt) override{
        sParams.lineMaxSize = ofLerp(startParams->sParams.lineMaxSize, targetParams->sParams.lineMaxSize, amt);
        sParams.lineMinLength = ofLerp(startParams->sParams.lineMinLength, targetParams->sParams.lineMinLength, amt);
        sParams.alpha = ofLerp(startParams->sParams.alpha, targetParams->sParams.alpha, amt);
        sParams.mixFactor = ofLerp(startParams->sParams.mixFactor, targetParams->sParams.mixFactor, amt);
        sParams.numIterations = ofLerp(startParams->sParams.numIterations, targetParams->sParams.numIterations, amt);
        sParams.dt = ofLerp(startParams->sParams.dt, targetParams->sParams.dt, amt);
        sParams.skipFrames = ofLerp(startParams->sParams.skipFrames, targetParams->sParams.skipFrames, amt);
    }

    void updateAttractorParams(AbstractAttractorParameters &aParams, float amt) override;

    void keyPressed(int key) override {
        if (ofGetKeyPressed(OF_KEY_CONTROL) && (key >= '0' && key <= '9')) {
            status->set("Saved current Scene in " + to_string(key - '0'));
            ofLog() << status->get();
            presets[key - '0'] = currentParams;
        }
        else if (key >= '0' && key <= '9') {
            currentScene = key - '0';
            status->set("Lerping to Scene " + to_string(currentScene));
            lerp = true;
            lerpTimeEnd = ofGetElapsedTimef() + lerpTime;
            startParams = &currentParams;
            targetParams = &presets[currentScene];
        } else if (keycodes.find(key) != keycodes.end()) {
            currentScene = keycodes.at(key);
            lerp = true;
            lerpTimeEnd = ofGetElapsedTimef();
            startParams = &currentParams;
            targetParams = &presets[keycodes.at(key)];
        } else if (key == 's') {
            savePresetsToJson();
        } else if (key == 'l') {
            loadPresetsFromJson();
        }
    }

    string getPresetFileName() const {
        return "presets.json";
    }
    
    void setLerpTime(float &time) override {
        lerpTime = time;
    }

    void loadPresetsFromJson() override {
        try{
            ofJson j = ofLoadJson(getPresetFileName());
            presets = j.template get<std::vector<AllParameters<T>>>();
            status->set("Loaded presets from " + getPresetFileName());
            ofLog() << status->get();
        }
        catch (nlohmann::json_abi_v3_11_2::detail::exception &e) {
            status->set("Failed to load presets " + std::string(e.what()));
            ofLog() << status->get();
        }
    }

    void savePresetsToJson() override {
        ofJson j = presets;
        ofSavePrettyJson(getPresetFileName(), j);
        status->set("Saved presets to " + getPresetFileName());
    }
};