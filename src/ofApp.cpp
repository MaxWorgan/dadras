#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    points.resize(DRAWING_BUFFER_SIZE);
    ofSetWindowTitle("Dadras Visualiser");
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(255);
    
    gui.setup(nullptr, true, ImGuiConfigFlags_ViewportsEnable );

    // Setup ofSound
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate =  SAMPLE_RATE;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = AUDIO_BUFFER_SIZE;
    soundStream.setup(settings);
    
    attractor = new Dadras();
    
    maxSize.set("Ribbon Size", 40,5,100);
    sizeLength.set("Ribbon min length",50,5,100);
    alpha.set("Alpha", 20,1,255);
    mixFactor.set("Mix", 1.0,1.0,0.0);
    numIterations.set("Num", 3,1,50);
    dt.set("dt", 0.003,0.00001,0.01);
    lerpTime.set("Lerp Time", 2,0.1,10);
    skipFrames.set("Skip Frames", 2, 1, 10);
    
    lerpTime.addListener(&presetManager, &PresetManager::setLerpTime);
    
    polyLine.clear();
    waveIndex = 0;
    
}

static float rotation = 0.0;

//--------------------------------------------------------------
void ofApp::update(){
    polyLine.clear();
    polyLine.addVertices(points);
    presetManager.update(attractor->params);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    
    auto verts = polyLine.getVertices();
    
    ofSetColor(0,alpha);
    //do the same thing from the first example...
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(unsigned int i = 1; i < (int)verts.size(); i++){

        //find this point and the next point
        glm::vec3 thisPoint = verts[i-1];
        glm::vec3 nextPoint = verts[i];
        if( i < verts.size()-1 ) {
            nextPoint = verts[i+1];
        }

        //get the direction from one to the next.
        //the ribbon should fan out from this direction
        glm::vec3 direction = (nextPoint - thisPoint);

        //get the distance from one point to the next
        float distance = glm::length(direction);

        //get the normalized direction. normalized vectors always have a length of one
        //and are really useful for representing directions as opposed to something with length
        glm::vec3 unitDirection = glm::normalize(direction);

        //find both directions to the left and to the right
        glm::vec3 toTheLeft  = glm::rotate(unitDirection, -90.f, glm::vec3(0,0,1));
        glm::vec3 toTheRight = glm::rotate(unitDirection,  90.f, glm::vec3(0,0,1));

        //use the map function to determine the distance.
        //the longer the distance, the narrower the line.
        //this makes it look a bit like brush strokes
        float thickness = ofMap(distance, 0, sizeLength, maxSize, 2, true);
        
        // calculate a taper based on the index
        float indexPct = 1.0f;
        if( i < 25 ) {
            indexPct = ofMap( i, 0, 25, 0.0, 1.0, true );
        }
        thickness *= indexPct;

        //calculate the points to the left and to the right
        //by extending the current point in the direction of left/right by the length
        glm::vec3 leftPoint = thisPoint+toTheLeft*thickness;
        glm::vec3 rightPoint = thisPoint+toTheRight*thickness;

        //add these points to the triangle strip
        mesh.addVertex(glm::vec3(leftPoint.x, leftPoint.y, leftPoint.z));
        mesh.addVertex(glm::vec3(rightPoint.x, rightPoint.y, rightPoint.z));
    }

    //end the shape
    mesh.draw();
    cam.end();
    
    //required to call this at beginning
    auto mainSettings = ofxImGui::Settings();
    gui.begin();
    static bool bCollapse = false;
    if (ofxImGui::BeginWindow("Dadras", mainSettings, ImGuiWindowFlags_NoCollapse, &bCollapse))
    {
        ImGui::SetWindowSize(ImVec2(500,300));
        ImGui::Text("%.1f FPS (%.3f ms/frame)", ofGetFrameRate(), 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Scene %i", presetManager.currentScene);
        ImGui::SliderFloat("a", &attractor->params.a,0.1 ,10.0);
        ImGui::SliderFloat("b", &attractor->params.b,0.1 ,10.0);
        ImGui::SliderFloat("c", &attractor->params.c,0.1 ,10.0);
        ImGui::SliderFloat("d", &attractor->params.d,0.1 ,10.0);
        ImGui::SliderFloat("r", &attractor->params.r,0.1 ,10.0);
        ImGui::Separator();
        ofxImGui::AddParameter(maxSize);
        ofxImGui::AddParameter(sizeLength);
        ofxImGui::AddParameter(alpha);
        ImGui::Separator();
        ofxImGui::AddParameter(mixFactor);
        ofxImGui::AddParameter(numIterations);
        ofxImGui::AddParameter(dt);
        ofxImGui::AddParameter(lerpTime);
        ofxImGui::AddParameter(skipFrames);
    }
    
    ofxImGui::EndWindow(mainSettings);

    gui.end();
}


void ofApp::audioOut(ofSoundBuffer& output){
    float angle = mixFactor * M_PI_2;
    float crossfadeCoeff1 = cos(angle);
    float crossfadeCoeff2 = sin(angle);
    
    std::size_t outChannels = output.getNumChannels();
    for (int i = 0; i < output.getNumFrames(); ++i){
        for( size_t i = 0; i < numIterations; ++i) {
            attractor->integrate(dt);
        }
        if(isUnstable(*attractor)){
            attractor->state.x = 1.0;
            attractor->state.y = 1.0;
            attractor->state.z = 1.0;
            ofLog() << "Warning: Simulation reset due to instability";
        }
        
        float x = tanh(attractor->state.x * 0.01);
        float y = tanh(attractor->state.y * 0.01);
        float z = tanh(attractor->state.z * 0.01);
        
        output[i * outChannels + 0] = x * crossfadeCoeff1 + y * crossfadeCoeff2;
        output[i * outChannels + 1] = y * crossfadeCoeff1 + z * crossfadeCoeff2;
        
        if(i % skipFrames == 0) {
            points[waveIndex] = attractor->state * 30.0f;
            
            if (waveIndex < (DRAWING_BUFFER_SIZE - 1)) {
                ++waveIndex;
            } else {
                waveIndex = 0;
            }
        }
    }
}
bool ofApp::isUnstable(const Dadras& attractor) {
    return std::isnan(attractor.state.x) || std::isnan(attractor.state.y) || std::isnan(attractor.state.z) ||
           std::isinf(attractor.state.x) || std::isinf(attractor.state.y) || std::isinf(attractor.state.z);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    presetManager.keyPressed(key);
}
