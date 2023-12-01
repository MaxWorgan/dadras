#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    points.resize(DRAWING_BUFFER_SIZE);
    ofSetWindowTitle("Dadras Visualiser");
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(255);

    
    //required call
    gui.setup(nullptr, true, ImGuiConfigFlags_ViewportsEnable );

    waveIndex = 0;
    
    // Maximilian audio stuff
    int sampleRate = 44100; /* Sampling Rate */
    int bufferSize= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
//    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    
    // Setup ofSound
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    
    attractor = new Dadras();
    targetParams = new DadrasParameters();
    startParams = new DadrasParameters();
    presets.resize(10);
    
    
    params.add(dadras_a.set("a",3.0,0.01,10.0));
    params.add(dadras_b.set("b",2.7,0.01,10.0));
    params.add(dadras_c.set("c",1.7,0.01,10.0));
    params.add(dadras_d.set("d",2.0,0.01,10.0));
    params.add(dadras_r.set("r",9.0,0.01,10.0));
    params.add(maxSize.set("Ribbon Size", 40,5,100));
    params.add(sizeLength.set("Ribbon min length",50,5,100));
    params.add(alpha.set("Alpha", 20,1,255));
    params.add(mixFactor.set("Mix", 1.0,1.0,0.0));
    params.add(numIterations.set("Num", 3,1,50));
    params.add(dt.set("dt", 0.003,0.00001,0.01));
    
    
    dadras_a.addListener(this, &ofApp::dadras_aChanged);
    dadras_b.addListener(this, &ofApp::dadras_bChanged);
    dadras_c.addListener(this, &ofApp::dadras_cChanged);
    dadras_d.addListener(this, &ofApp::dadras_dChanged);
    dadras_r.addListener(this, &ofApp::dadras_rChanged);
    
    polyLine.clear();
    
    lerpTime = 2;
    lerpTimeEnd = 0;
    lerp = false;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    polyLine.clear();
    polyLine.addVertices(points);
    
    if(lerp && (*targetParams != attractor->params)) {
        float t = ofGetElapsedTimef();
        float amt  = 1.0 - (lerpTimeEnd - t) / lerpTime;
        if(amt > 1.0){
            amt = 1.0;
            lerp = false;
        }
        dadras_a = ofLerp(startParams->a, targetParams->a,amt);
        dadras_b = ofLerp(startParams->b, targetParams->b,amt);
        dadras_c = ofLerp(startParams->c, targetParams->c,amt);
        dadras_d = ofLerp(startParams->d, targetParams->d,amt);
        dadras_r = ofLerp(startParams->r, targetParams->r,amt);
    }
//    if(*targetParams == attractor->params) {
//        lerp = false;
//    }
    
//    if(t < lerpTimeEnd){
//        static float startA = dadras_a;
//        float amt  = 1.0 - (lerpTimeEnd - t) / lerpTime;
//        ofLog() << "Amt: " << amt;
//        float value = ofLerp(startA, dadras_b, amt);
//        ofLog() << "value: " << value;
//        dadras_a = value;
//    }

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
        glm::vec3 toTheLeft =  glm::rotate(unitDirection, -90.f, glm::vec3(0,0,1));
        glm::vec3 toTheRight = glm::rotate(unitDirection, 90.f, glm::vec3(0,0,1));

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
    
    
//    
//    ofBackground(20);
//    
//    cam.begin();
//        ofSetColor(248, 255,246);
//        ofFill();
//        polyLine.draw();
//    cam.end();

//    gui.draw();
    
    
    //required to call this at beginning
    auto mainSettings = ofxImGui::Settings();
    gui.begin();
    static bool bCollapse = false;
    if (ofxImGui::BeginWindow("Dadras", mainSettings, ImGuiWindowFlags_NoCollapse, &bCollapse))
    {
        ImGui::SetWindowSize(ImVec2(600,600));
        ImGui::Text("%.1f FPS (%.3f ms/frame)", ofGetFrameRate(), 1000.0f / ImGui::GetIO().Framerate);
        ofxImGui::AddParameter(dadras_a);
        ofxImGui::AddParameter(dadras_b);
        ofxImGui::AddParameter(dadras_c);
        ofxImGui::AddParameter(dadras_d);
        ofxImGui::AddParameter(dadras_r);
//        ImGui::Separator();
//        ofxImGui::AddParameter(maxSize);
//        ofxImGui::AddParameter(sizeLength);
//        ofxImGui::AddParameter(alpha);
//        ImGui::Separator();
//        ofxImGui::AddParameter(mixFactor);
//        ofxImGui::AddParameter(numIterations);
//        ofxImGui::AddParameter(dt);
                                                                                                    
        
        
    }
    
    ofxImGui::EndWindow(mainSettings);

    
    

    //required to call this at end
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
        
        if(i % 2 == 0) {
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

// Hack to enable SHIFT + numbers to work less horribly
static const std::map<char32_t,int> keycodes = {{'!', 1},{'"',2},{U'£',3},{'$',4},{'%',5},{'^',6},{'&',7},{'*',8},{'(', 9}, {')',0}};

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (ofGetKeyPressed(OF_KEY_CONTROL) && (key >= '0' && key <= '9')){
        ofLog() << "Saved current state in " << key - '0';
        presets[key - '0'] = attractor->params;
    } else if (key >= '0' && key <= '9'){
        ofLog() << "Lerping to state " << key - '0';
        lerp = true;
        lerpTimeEnd = ofGetElapsedTimef() + lerpTime;
        *startParams = attractor->params;
        *targetParams = presets[key - '0'];
    } else if (keycodes.find(key) != keycodes.end()){
        ofLog() << "Moving to state " << keycodes.at(key);
        lerp = true;
        lerpTimeEnd = ofGetElapsedTimef();
        *startParams = attractor->params;
        *targetParams = presets[keycodes.at(key)];
    }

}

void ofApp::dadras_aChanged(float & a){
    attractor->params.a = a;
}
void ofApp::dadras_bChanged(float & b){
    attractor->params.b = b;
}
void ofApp::dadras_cChanged(float & c){
    attractor->params.c = c;
}
void ofApp::dadras_dChanged(float & d){
    attractor->params.d = d;
}
void ofApp::dadras_rChanged(float & r){
    attractor->params.r = r;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
 
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
