#pragma once

#include "ofxOsc.h"
#include "ofMain.h"
#include "ofJson.h"
#include "SimulationParameters.h"
#include "genann.h"
#include "ofxControlPanel.h"
#include <tuple>
#include "ThomasAttractor.h"
#include "DadrasAttractor.h"
#include "PresetManager.hpp"
#include "CoupledLorenzAttractor.h"


#define AUDIO_BUFFER_SIZE 512
#define DRAWING_BUFFER_SIZE 10000
#define SAMPLE_RATE 96000
class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;

		void keyPressed(int key) override;
        void audioOut(ofSoundBuffer& output) override;
        bool isUnstable(const BaseAttractor& attractor);
        void setupGui(ofxControlPanel &gui);
        void setupAttractor(int &which);
        void setupAttractorGui(ofxControlPanel &gui);
        void capture();
        void train();
        void predict(float &value);
        
        double scaleParameter(ofParameter<float> &p);
        float rescaleToParameter(double value, ofParameter<float> &p);
    
        Dadras* dadras;
        Thomas* thomas;
        CoupledLorenz* coupledLorenz;

        BaseAttractor* attractor;

        SimulationParameters params;

        ofParameter<bool> showControls;
        ofParameter<string> loss;

        ofParameter<int> whichAttractor;
        vector<string> attractorNames;
       
        ofEasyCam cam;
        vector<glm::vec3> points;
        ofPolyline polyLine;
        ofMesh mesh;
        int waveIndex;
        ofSoundStream soundStream;

        vector<tuple<ofVec2f, vector<double>>> training_data;

        ofxControlPanel* gui;

};
