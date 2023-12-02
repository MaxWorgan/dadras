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


class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
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
        ofParameter<double> dt;
        
        ofxImGui::Gui gui;

    
};
