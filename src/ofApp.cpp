#include "ofApp.h"
#include "guiTypeButton.h"


//--------------------------------------------------------------
void ofApp::setup(){


    points.resize(DRAWING_BUFFER_SIZE);

    ofSetWindowTitle("Dadras Visualiser");
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(255);

    srand(time(0));
    
    gui = new ofxControlPanel();
    gui->setup("Choose", 0.0, 0.0, 300, ofGetHeight(), false);
    
    dadras = new Dadras(params.status);
    thomas = new Thomas(params.status);
    coupledLorenz = new CoupledLorenz(params.status);
    attractor = dadras;


    // Setup ofSound
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate =  SAMPLE_RATE;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = AUDIO_BUFFER_SIZE;
    soundStream.setup(settings);

    params.lerpTime.addListener(&(*attractor->presetManager), &BasePresetManager::setLerpTime);
    params.mouseX.addListener(this, &ofApp::predict);
    params.mouseY.addListener(this, &ofApp::predict);

    polyLine.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    waveIndex = 0;
    showControls = false;

    attractorNames = {"Dadras", "Thomas", "Coupled Lorenz"};
    whichAttractor.set("Select Attractor", 0, 0, attractorNames.size() - 1);
    whichAttractor.addListener(this, &ofApp::setupAttractor);

    gui->setWhichPanel("Choose");
    gui->addTextDropDown(whichAttractor, attractorNames);
    setupGui(*gui);
    setupAttractorGui(*gui);

}

void ofApp::setupAttractor(int &which){
    switch(which){
        case 0:
            attractor = dadras;
            break;
        case 1:
            attractor = thomas;
            break;
        case 2:
            attractor = coupledLorenz;
            break;
    }
    string attractorName = attractorNames[which];
    gui->removePanel("Attractor");
    setupAttractorGui(*gui);
}

void ofApp::predict(float &value) {
    if(params.predict) {
        dynamic_cast<MLPAttractor*>(attractor)->predict(params.mouseX.get(), params.mouseY.get());
    }
}

void ofApp::capture(){
    dynamic_cast<MLPAttractor*>(attractor)->capture(params.mouseX.get(), params.mouseY.get());
}

void ofApp::train(){
    float lossValue = dynamic_cast<MLPAttractor*>(attractor)->train(params.numTrainingCycles, params.learningRate);
    loss = "Loss: " + to_string(lossValue);
}   

void ofApp::setupAttractorGui(ofxControlPanel &gui){
    gui.addPanel("Attractor");
    gui.setWhichPanel("Attractor");
    gui.addLabel(*params.status);
    attractor->setupGui(gui);
    gui.addSlider(params.lerpTime);
    gui.addSlider2D(params.mouseX, params.mouseY);

    ofParameter<void> capture = ofParameter<void>("capture");
    capture.addListener(this, &ofApp::capture);
    gui.addButton(capture);
    ofParameter<void> clear = ofParameter<void>("clear training data");
    clear.addListener(dynamic_cast<MLPAttractor*>(attractor), &MLPAttractor::clearTrainingData);
    gui.addButton(clear);
    ofParameter<void> reset = ofParameter<void>("reset ann");
    reset.addListener(dynamic_cast<MLPAttractor*>(attractor), &MLPAttractor::resetAnn);
    gui.addButton(reset);

    ofParameter<void> train = ofParameter<void>("train");
    train.addListener(this, &ofApp::train);        
    gui.addButton(train);
    gui.addLabel(loss);
    gui.addSlider(params.numTrainingCycles);
    gui.addSlider(params.learningRate);
    gui.addToggle(params.predict);
}

void ofApp::setupGui(ofxControlPanel &gui) {
    gui.addPanel("Display");
    gui.setWhichPanel("Display");
    gui.addSlider(params.lineMaxSize);
    gui.addSlider(params.lineMinLength);
    gui.addSlider(params.alpha);
    gui.addSlider(params.skipFrames);
    gui.addToggle(params.autoRotate);
    gui.addSlider(params.rotationSpeedX);
    gui.addSlider(params.rotationSpeedY);
    gui.addSlider(params.rotationSpeedZ);
    gui.addPanel("Misc");
    gui.setWhichPanel("Misc");
    gui.addSlider(params.audioSignalReduction);
    gui.addSlider(params.mixFactor);
    gui.addSlider(params.dt);
    gui.addSlider(params.numIterations);
}

//--------------------------------------------------------------
void ofApp::update(){
    polyLine.clear();
    polyLine.addVertices(points);
    attractor->presetManager->update(params, *attractor->params.get());
    if(showControls) gui->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    
    auto verts = polyLine.getVertices();
    mesh.clear();
    
    ofSetColor(0,params.alpha);
    for(size_t i = 1; i < verts.size(); i++){

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
        glm::vec3 unitDirection = glm::normalize(direction);

        //find both directions to the left and to the right
        glm::vec3 toTheLeft  = glm::rotate(unitDirection, -90.f, glm::vec3(0,0,1));
        glm::vec3 toTheRight = glm::rotate(unitDirection,  90.f, glm::vec3(0,0,1));

        //use the map function to determine the distance.
        //the longer the distance, the narrower the line.
        //this makes it look a bit like brush strokes
        float thickness = ofMap(distance, 0, params.lineMinLength, params.lineMaxSize, 2, true);
        
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
    if(params.autoRotate) {
        static float rotationX = 0.1;
        static float rotationY = 0.1;
        static float rotationZ = 0.1;
        ofRotateXDeg(rotationX);
        ofRotateYDeg(rotationY);
        ofRotateZDeg(rotationZ);
        rotationX += params.rotationSpeedX;
        rotationY += params.rotationSpeedY;
        rotationZ += params.rotationSpeedZ;
    }
    mesh.draw();
    cam.end();

    if(showControls) gui->draw();
}


void ofApp::audioOut(ofSoundBuffer& output){
    float angle = params.mixFactor * M_PI_2;
    float crossfadeCoeff1 = cos(angle);
    float crossfadeCoeff2 = sin(angle);
    
    std::size_t outChannels = output.getNumChannels();
    for (int i = 0; i < output.getNumFrames(); ++i){
        for( size_t i = 0; i < params.numIterations; ++i) {
            attractor->integrate(params.dt);
        }
        if(isUnstable(*dadras)){
            attractor->state.x = 1.0;
            attractor->state.y = 1.0;
            attractor->state.z = 1.0;
            ofLog() << "Warning: Simulation reset due to instability";
        }
        
        float x = tanh(attractor->state.x * params.audioSignalReduction);
        float z = tanh(attractor->state.z * params.audioSignalReduction);
        float y = tanh(attractor->state.y * params.audioSignalReduction);
        
        output[i * outChannels + 0] = x * crossfadeCoeff1 + y * crossfadeCoeff2;
        output[i * outChannels + 1] = y * crossfadeCoeff1 + z * crossfadeCoeff2;
        
        if(i % params.skipFrames == 0) {
            points[waveIndex] = attractor->state * 30.0f;
            
            if (waveIndex < (DRAWING_BUFFER_SIZE - 1)) {
                ++waveIndex;
            } else {
                waveIndex = 0;
            }
        }
    }
}
bool ofApp::isUnstable(const BaseAttractor& attractor) {
    return std::isnan(attractor.state.x) || std::isnan(attractor.state.y) || std::isnan(attractor.state.z) ||
           std::isinf(attractor.state.x) || std::isinf(attractor.state.y) || std::isinf(attractor.state.z);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == '\\'){
        showControls = !showControls;
        if(showControls){
            cam.setControlArea(ofRectangle(gui->getWidth(),0,ofGetWindowWidth() - gui->getWidth(), ofGetWindowHeight()));
        } else {
            cam.clearControlArea();
        }
    }

    attractor->presetManager->keyPressed(key);
}
