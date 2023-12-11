#pragma once

#include "ofxOsc.h"
#include "ofMain.h"
// #include "ofxGui.h"
// #include "ofxImGui.h"
#include "ofJson.h"
// #include "ImHelpers.h"
#include "DadrasAttractor.h"
#include "PresetManager.hpp"
#include "SimulationParameters.h"
#include "genann.h"
#include "ofxControlPanel.h"
#include <tuple>


#define SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 512
#define DRAWING_BUFFER_SIZE 10000
#define SAMPLE_RATE 44100
#define LEARNING_RATE 0.001
class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;

		void keyPressed(int key) override;
        void audioOut(ofSoundBuffer& output) override;
        bool isUnstable(const Dadras& attractor);
        void setupGui(ofxControlPanel &gui, AllParameters &params);
        void capture();
        void train();
        void clearTrainingData();
        void predictParameters(float &value);
        
        double scaleParameter(ofParameter<float> &p);
        float rescaleToParameter(double value, ofParameter<float> &p);
    
        Dadras* attractor;
        AllParameters params;
        genann *ann;
        PresetManager *presetManager;

        ofParameter<bool> showControls;
        ofParameter<string> loss;
       
        ofEasyCam cam;
        vector<glm::vec3> points;
        ofPolyline polyLine;
        ofMesh mesh;
        int waveIndex;
        ofSoundStream soundStream;

        vector<tuple<ofVec2f, vector<double>>> training_data;

        ofxControlPanel gui;

};
