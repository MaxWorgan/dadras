#pragma once

#include "ofxOsc.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxImGui.h"
#include "ofJson.h"
#include "ImHelpers.h"
#include "DadrasAttractor.h"
#include "PresetManager.hpp"

#define SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 512
#define DRAWING_BUFFER_SIZE 10000
#define SAMPLE_RATE 44100


class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;

		void keyPressed(int key) override;
        void audioOut(ofSoundBuffer& output) override;
        bool isUnstable(const Dadras& attractor);
    
        Dadras* attractor;
        PresetManager presetManager;
       
        ofEasyCam cam;
        vector<glm::vec3> points;
        ofPolyline polyLine;
        int waveIndex;
        ofSoundStream soundStream;
        
        ofParameter<float>  maxSize;
        ofParameter<float>  sizeLength;
        ofParameter<float>  alpha;
        ofParameter<float>  mixFactor;
        ofParameter<int>    numIterations;
        ofParameter<float>  dt;
        ofParameter<float>  lerpTime;
        ofParameter<int>    skipFrames;
        
        ofxImGui::Gui gui;

};
