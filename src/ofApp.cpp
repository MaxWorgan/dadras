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
    
    ann = genann_init(2, 1, 5, 5);

    gui.setup("Dadras Attractor Parameters", 0.0, 0.0, 300, ofGetHeight(), false);

    setupGui(gui, params);

    // Setup ofSound
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate =  SAMPLE_RATE;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = AUDIO_BUFFER_SIZE;
    soundStream.setup(settings);
    
    attractor = new Dadras();

    params.sParams.lerpTime.addListener(&presetManager, &PresetManager::setLerpTime);
    params.sParams.mouseX.addListener(&(*this), &ofApp::predictParameters);
    
    polyLine.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    waveIndex = 0;

    showControls = false;

}

void ofApp::setupGui(ofxControlPanel &gui, AllParameters &params) {
    gui.setWhichPanel(0);
    gui.addSlider(params.dParams.a);
    gui.addSlider(params.dParams.b);
    gui.addSlider(params.dParams.c);
    gui.addSlider(params.dParams.d);
    gui.addSlider(params.dParams.r);
    ofParameter<void> randomizeParam = ofParameter<void>("randomize");
    randomizeParam.addListener(&params, &AllParameters::randomize);
    gui.addButton(randomizeParam);
    ofParameter<void> microRandomize = ofParameter<void>("micro-randomize");
    microRandomize.addListener(&params, &AllParameters::microRandomize);
    gui.addButton(microRandomize);
    gui.addSlider(params.sParams.microRandomizeAmt);
    gui.addSlider(params.sParams.lerpTime);
    gui.addSlider2D(params.sParams.mouseX, params.sParams.mouseY);
    ofParameter<void> capture = ofParameter<void>("capture");
    capture.addListener(&(* this), &ofApp::capture);
    gui.addButton(capture);
    ofParameter<void> clear = ofParameter<void>("clear");
    clear.addListener(&(* this), &ofApp::clearTrainingData);
    gui.addButton(clear);
    ofParameter<void> train = ofParameter<void>("train");
    train.addListener(&(* this), &ofApp::train);
    gui.addButton(train);
    gui.addLabel(loss);
    gui.addSlider(params.sParams.numTrainingCycles);
    gui.addSlider(params.sParams.learningRate);
    gui.addToggle(params.sParams.predict);
    gui.addPanel("Display Parameters",8,false);
    gui.setWhichPanel(1);
    gui.addSlider(params.sParams.lineMaxSize);
    gui.addSlider(params.sParams.lineMinLength);
    gui.addSlider(params.sParams.alpha);
    gui.addSlider(params.sParams.skipFrames);
    gui.addToggle(params.sParams.autoRotate);
    gui.addSlider(params.sParams.rotationSpeedX);
    gui.addSlider(params.sParams.rotationSpeedY);
    gui.addSlider(params.sParams.rotationSpeedZ);
    gui.addPanel("Misc Parameters",8,false);
    gui.setWhichPanel(2);
    gui.addSlider(params.sParams.audioSignalReduction);
    gui.addSlider(params.sParams.mixFactor);
    gui.addSlider(params.sParams.dt);
    gui.addSlider(params.sParams.numIterations);
}

//--------------------------------------------------------------
void ofApp::update(){
    polyLine.clear();
    polyLine.addVertices(points);
    presetManager.update(params);
    if(showControls) gui.update();
}

double ofApp::scaleParameter(ofParameter<float> &p){
    return ofMap(p.get(), p.getMin(), p.getMax(), 0.0,1.0);
}
float ofApp::rescaleToParameter(double value, ofParameter<float> &p){
    return ofMap(value, 0.0, 1.0, p.getMin(), p.getMax());
}

void ofApp::capture(){

    vector<double> scaledOutput = {
        scaleParameter(params.dParams.a),
        scaleParameter(params.dParams.b),
        scaleParameter(params.dParams.c),
        scaleParameter(params.dParams.d),
        scaleParameter(params.dParams.r),
    };

    training_data.push_back(
        tuple(
            ofVec2f(params.sParams.mouseX.get(),params.sParams.mouseY.get()),
            scaledOutput
        )
    );
}

void ofApp::clearTrainingData(){
    training_data.clear();
}

void ofApp::predictParameters(float &value){
    if (params.sParams.predict) {
        const double data[2] = {params.sParams.mouseX.get(), params.sParams.mouseY.get()};
        ofLog() << "predict input:  x - " << data[0] << " y - " << data[1];
        const double *output = genann_run(ann, data);
        ofLog() << "predict output: " << rescaleToParameter(output[0], params.dParams.a) << " "
                << rescaleToParameter(output[1], params.dParams.b) << " "
                << rescaleToParameter(output[1], params.dParams.c) << " "
                << rescaleToParameter(output[1], params.dParams.d) << " "
                << rescaleToParameter(output[1], params.dParams.r);

        params.dParams.a = rescaleToParameter(output[0],params.dParams.a);
        params.dParams.b = rescaleToParameter(output[1],params.dParams.b);
        params.dParams.c = rescaleToParameter(output[2],params.dParams.c);
        params.dParams.d = rescaleToParameter(output[3],params.dParams.d);
        params.dParams.r = rescaleToParameter(output[4],params.dParams.r);
    }
}

void ofApp::train(){

    std::vector<vector<double>> inputs;
    std::vector<vector<double>> outputs;

    // Lambda to split the original vector
    auto splitLambda = [&inputs, &outputs](const auto& tuple) {
         // Insert elements from the vector of doubles into the single vector
        const ofVec2f& p = std::get<0>(tuple);
        inputs.push_back({p.x, p.y});

        const vector<double>& dp = std::get<1>(tuple);
        outputs.push_back(dp);
       
    };

    // Apply the lambda to each element of the original vector
    std::for_each(training_data.begin(), training_data.end(), splitLambda);

    for(int z = 0; z < params.sParams.numTrainingCycles; z++){
        for (size_t i = 0; i < inputs.size(); ++i){
            auto in = inputs[i].data();
            auto out = outputs[i].data();
            genann_train(ann, in, out, params.sParams.learningRate);
        }
    }
    const double* testOutput= genann_run(ann, inputs[0].data());
    double mse = 0;
    for(size_t i = 0; i < ann->outputs; ++i){
        mse += pow((outputs[0][i] - testOutput[i]), 2);
    }
    loss = "loss: " + ofToString(mse);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    cam.begin();
    
    auto verts = polyLine.getVertices();
    mesh.clear();
    
    ofSetColor(0,params.sParams.alpha);
    //do the same thing from the first example...
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
        float thickness = ofMap(distance, 0, params.sParams.lineMinLength, params.sParams.lineMaxSize, 2, true);
        
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
    if(params.sParams.autoRotate) {
        static float rotationX = 0.1;
        static float rotationY = 0.1;
        static float rotationZ = 0.1;
        ofRotateXDeg(rotationX);
        ofRotateYDeg(rotationY);
        ofRotateZDeg(rotationZ);
        rotationX += params.sParams.rotationSpeedX;
        rotationY += params.sParams.rotationSpeedY;
        rotationZ += params.sParams.rotationSpeedZ;
    }
    mesh.draw();
    cam.end();

    if(showControls) gui.draw();
}


void ofApp::audioOut(ofSoundBuffer& output){
    float angle = params.sParams.mixFactor * M_PI_2;
    float crossfadeCoeff1 = cos(angle);
    float crossfadeCoeff2 = sin(angle);
    
    std::size_t outChannels = output.getNumChannels();
    for (int i = 0; i < output.getNumFrames(); ++i){
        for( size_t i = 0; i < params.sParams.numIterations; ++i) {
            attractor->integrate(params.sParams.dt, params.dParams);
        }
        if(isUnstable(*attractor)){
            attractor->state.x = 1.0;
            attractor->state.y = 1.0;
            attractor->state.z = 1.0;
            ofLog() << "Warning: Simulation reset due to instability";
        }
        
        float x = tanh(attractor->state.x * params.sParams.audioSignalReduction);
        float z = tanh(attractor->state.z * params.sParams.audioSignalReduction);
        float y = tanh(attractor->state.y * params.sParams.audioSignalReduction);
        
        output[i * outChannels + 0] = x * crossfadeCoeff1 + y * crossfadeCoeff2;
        output[i * outChannels + 1] = y * crossfadeCoeff1 + z * crossfadeCoeff2;
        
        if(i % params.sParams.skipFrames == 0) {
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
    if(key == '\\'){
        showControls = !showControls;
        if(showControls){
            cam.setControlArea(ofRectangle(gui.getWidth(),0,ofGetWindowWidth() - gui.getWidth(), ofGetWindowHeight()));
        } else {
            cam.clearControlArea();
        }
    }
    presetManager.keyPressed(key);
}
