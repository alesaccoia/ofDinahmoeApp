#include "ofDmafApp.h"

#include <cassert>

#define SAMPLING_RATE 44100.F
#define INPUT_CHANNELS 0
#define OUTPUT_CHANNELS 2
#define BUFFER_SIZE 512


static ofDmafApp* thisApp;

void dmafCb(const char* trigger_, float time_, DmParametersPOD params_, void* args) {
  thisApp->dmafDirectCallback(trigger_, time_, params_, args);
}


//--------------------------------------------------------------
void ofDmafApp::setup(){
  m_isPlaying = false;

  scale = 1.0F;
  colorScale = 1.0F;

  thisApp = this;
  
  ofSetVerticalSync(true);
	
  ofSetSmoothLighting(true);
  
  setupGraphics();
  
  m_camera.setPosition(ofGetWidth()/2, ofGetHeight()/2, 250);
  
  // setup gui
  m_gui.setup(); // most of the time you don't need a name
  m_gui.add(m_playStop.setup("play/stop", false, ofGetWidth()/2, ofGetHeight()/18));
  m_gui.add(m_intensity.setup("intensity_phone", 0, 0, 10, ofGetWidth()/2, ofGetHeight()/18));
  m_gui.add(m_rotation.setup("rotation3_phone", -90, 0, 90, ofGetWidth()/2, ofGetHeight()/18));
  m_gui.add(m_currentTime.setup("Time", "0.0", ofGetWidth()/2, ofGetHeight()/18));
  
  m_playStop.addListener(this, &ofDmafApp::playStateChanged);
  m_intensity.addListener(this, &ofDmafApp::intensityChanged);
  m_rotation.addListener(this, &ofDmafApp::rotationChanged);
  
  m_logo.load("DinahMoeLogo.png");
  
  
  DmDictionary params;
  params.setStringValue("settingsFile", ofToDataPath("android_tempo/xml/settings.xml").c_str());
  m_dmaf.initialize(params);
  m_dmaf.addListener("drums_1", dmafCb, NULL);
  
  // create temp buffer
  m_tempOutputBuffer = new float*[OUTPUT_CHANNELS];
  for (int ch = 0; ch < OUTPUT_CHANNELS; ++ch) {
    m_tempOutputBuffer[ch] = new float[BUFFER_SIZE];
  }
  
  //
  ofSoundStreamSetup(OUTPUT_CHANNELS, INPUT_CHANNELS, this, SAMPLING_RATE, BUFFER_SIZE, 4);

  m_isPlaying = true;
  
  AlexUtilityBubble* foo(new AlexUtilityBubble("ciao"));
  
  
  #if defined(TARGET_OSX)
  m_usesMultitouch = false;
  m_pad = ofxMultiTouchPad();
  #endif

}


void ofDmafApp::exit() {
  m_isPlaying = false;
  m_audioMutex.lock();
  m_dmaf.deinitialize();
  m_audioMutex.unlock();
}

// callbacks
void ofDmafApp::playStateChanged(bool & playStop_) {
  if (playStop_) {
    m_dmaf.tell("play");
  } else {
    m_dmaf.tell("stop");
  }
}

void ofDmafApp::intensityChanged(float & intensity_){
  DmParametersPOD params = DmParamsCreate(1);
  DmParamsSetFloat(params, 0, intensity_);
  m_dmaf.tell("intensity_phone", params);
}

void ofDmafApp::rotationChanged(float & rotation_){
  DmParametersPOD params = DmParamsCreate(1);
  DmParamsSetFloat(params, 0, rotation_);
  m_dmaf.tell("rotation3_phone", params);
}


void ofDmafApp::dmafDirectCallback(const char* trigger_, float time_, DmParametersPOD params_, void* args) {
  m_mutex.lock();
  m_dmafMessagesShared.push_back(DmafCallbackArgs());
  m_dmafMessagesShared.back().trigger = trigger_;
  m_dmafMessagesShared.back().time = time_;
  m_dmafMessagesShared.back().midinote = DmParamsGetInt(params_, 0);
  m_mutex.unlock();
}

//--------------------------------------------------------------
void ofDmafApp::update(){
  m_currentTime = ofToString(m_dmaf.getCurrentTime());
  
  m_mutex.lock();
  m_newDmafMessages.splice(m_newDmafMessages.end(), m_dmafMessagesShared);
  m_mutex.unlock();
  
  std::list<DmafCallbackArgs>::iterator nmi = m_newDmafMessages.begin();
  
  while (nmi != m_newDmafMessages.end()) {
    std::string trig(nmi->trigger);
    if (nmi->time < m_dmaf.getCurrentTime() + 0.04) {
      if (trig == "drums_1") {
        int midinote = nmi->midinote;
        if (midinote == 36) {
          scale = 2.5F;
        } else {
          colorScale = 3;
        }
      }
      nmi = m_newDmafMessages.erase(nmi);
    } else {
      ++nmi;
    }
  }
}

//--------------------------------------------------------------
void ofDmafApp::draw(){
  ofBackgroundGradient( ofColor( 0 ), ofColor( 128 ) );

  ofEnableDepthTest();
  ofEnableLighting();
  m_camera.begin();
  m_material.begin();
  
  if (false) m_spotLight.enable();
  
  m_directionalLight.enable();
	ofPushMatrix();	ofTranslate( ofGetWidth()/2, ofGetHeight()/2, 0 );

	//Calculate the rotation angle
	float time = ofGetElapsedTimef();	//Get time in seconds
	float angle = time * 10;			//Compute angle. We rotate at speed 10 degrees per second
	ofScale(scale,scale,scale);
  ofRotate( angle, 0, 1, 0 );			//Rotate the coordinate system along y-axe
  scale *= .97;
  if (scale < 1.0F) {
    scale = 1.0F;
  }
  
	//Draw the spheres
	for (int i=0; i<nTri; i++) {
		ofSetColor( colors[i] * colorScale );		//Set color
    ofDrawSphere(vertices[ i*3 ], 10);
	}
  colorScale *= .92;
  if (scale < 1.0F) {
    scale = 1.0F;
  }
	ofPopMatrix();	//Restore the coordinate system
  m_material.end();
  
  if (false) m_spotLight.draw();
  
  m_camera.end();
  
  ofDisableLighting();
  ofDisableDepthTest();
  
  ofSetColor(ofColor::white);
  m_gui.draw();
//  m_logo.draw(10, 0, ofGetWidth()-20, ofGetHeight());
}


// Audio


void ofDmafApp::audioRequested(float * output, int bufferSize, int nChannels) {
  assert(bufferSize == BUFFER_SIZE);
  assert(nChannels == OUTPUT_CHANNELS);
  if (m_audioMutex.tryLock() && m_isPlaying) {
    m_dmaf.process(0, NULL, nChannels, m_tempOutputBuffer, bufferSize);
    float * outputSample = output;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
      for (int ch = 0; ch < OUTPUT_CHANNELS; ++ch) {
        *outputSample = m_tempOutputBuffer[ch][i];
        ++outputSample;
      }
    }
    m_audioMutex.unlock();
  } else {
    float * outputSample = output;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
      for (int ch = 0; ch < OUTPUT_CHANNELS; ++ch) {
        *outputSample = .0F;
        ++outputSample;
      }
    }
  }
}


// graphics

void ofDmafApp::setupGraphics() {
	nTri = 100;			//The number of the triangles
	nVert= nTri * 3;		//The number of the vertices

	float Rad = ofGetWidth() / 5;	//The sphere's radius
	float rad = Rad/5;	//Maximal triangle's ìradiusî
	//(formally, it's the maximal coordinates'
	//deviation from the triangle's center)

	ofEnableDepthTest();
  ofSetSmoothLighting(true);

	//Fill the vertices array
	vertices.resize( nVert );		//Set the array size
	for (int i=0; i<nTri; i++) {	//Scan all the triangles
		//Generate the center of the triangle
		//as a random point on the sphere

		//Take the random point from
		//cube [-1,1]x[-1,1]x[-1,1]	
		ofPoint center( ofRandom( -1, 1 ),
			ofRandom( -1, 1 ),
			ofRandom( -1, 1 ) );
		center.normalize(); //Normalize vector's length to 1
		center *= Rad;	//Now the center vector has
		//length Rad

		//Generate the triangle's vertices
		//as the center plus random point from
		//[-rad, rad]x[-rad, rad]x[-rad, rad]
		for (int j=0; j<3; j++) {
			vertices[ i*3 + j ] =
				center + ofPoint( ofRandom( -rad, rad ),
				ofRandom( -rad, rad ),  
				ofRandom( -rad, rad ) );
		}
	}

	//Fill the array of triangles' colors
	colors.resize( nTri );
	for (int i=0; i<nTri; i++) {
		//Take a random color from black to red
		colors[i] = ofColor( ofRandom(128,255), 45, 17 );
	}

  m_directionalLight.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	m_directionalLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
	m_directionalLight.setDirectional();
    
    // set the direction of the light
    // set it pointing from left to right -> //
	m_directionalLight.setOrientation( ofVec3f(0, 90, 0) );
  
  m_material.setShininess( 120 );
    // the light highlight of the material //
	m_material.setSpecularColor(ofColor(255, 255, 255, 255));
  
  
  
  m_spotLight.setDiffuseColor( ofColor(255.f, 0.f, 0.f));
  m_spotLight.setSpecularColor( ofColor(255.f, 255.f, 255.f));

  // turn the light into spotLight, emit a cone of light //
  m_spotLight.setSpotlight();

  // size of the cone of emitted light, angle between light axis and side of cone //
  // angle range between 0 - 90 in degrees //
  m_spotLight.setSpotlightCutOff( 50 );

  // rate of falloff, illumitation decreases as the angle from the cone axis increases //
  // range 0 - 128, zero is even illumination, 128 is max falloff //
  m_spotLight.setSpotConcentration( 45 );
}


//--------------------------------------------------------------
void ofDmafApp::keyPressed(int key){
#if defined(TARGET_OSX)
  if (key == 'm') {
    usesMultitouch = !usesMultitouch;
    AlexUtilityBubble*foo(new AlexUtilityBubble(usesMultitouch?"Using multitouch":"Using mouse"));
  }
#endif
}

//--------------------------------------------------------------
void ofDmafApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofDmafApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofDmafApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofDmafApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofDmafApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofDmafApp::windowResized(int w, int h){

}


#if defined(TARGET_OSX) || defined(TARGET_IOS)

//--------------------------------------------------------------
void ofDmafApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofDmafApp::dragEvent(ofDragInfo dragInfo){ 

}

#endif

#if defined(TARGET_IOS)

//--------------------------------------------------------------
void ofDmafApp::exit(){

}

//--------------------------------------------------------------
void ofDmafApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDmafApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDmafApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDmafApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDmafApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofDmafApp::lostFocus(){

}

//--------------------------------------------------------------
void ofDmafApp::gotFocus(){

}

//--------------------------------------------------------------
void ofDmafApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofDmafApp::deviceOrientationChanged(int newOrientation){

}

#endif

#if defined(TARGET_ANDROID)

//--------------------------------------------------------------
void ofDmafApp::touchDown(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDmafApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDmafApp::touchUp(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDmafApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDmafApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDmafApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofDmafApp::pause(){

}

//--------------------------------------------------------------
void ofDmafApp::stop(){

}

//--------------------------------------------------------------
void ofDmafApp::resume(){

}

//--------------------------------------------------------------
void ofDmafApp::reloadTextures(){

}

//--------------------------------------------------------------
bool ofDmafApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofDmafApp::okPressed(){

}

//--------------------------------------------------------------
void ofDmafApp::cancelPressed(){

}
#endif


AlexUtilityBubble::AlexUtilityBubble(std::string text_, float duration_)
 : m_text(text_)
 , m_duration(duration_)
 , m_endTime(ofGetElapsedTimef() + m_duration) {
	ofAddListener(ofEvents().update, this, &AlexUtilityBubble::update);
	ofAddListener(ofEvents().draw, this, &AlexUtilityBubble::draw);
}

AlexUtilityBubble::~AlexUtilityBubble() {
  ofRemoveListener(ofEvents().update, this, &AlexUtilityBubble::update);
  ofRemoveListener(ofEvents().draw, this, &AlexUtilityBubble::draw);
}

void AlexUtilityBubble::update(ofEventArgs & args) {
  if (ofGetElapsedTimef() >= m_endTime) delete this;
}

void AlexUtilityBubble::draw(ofEventArgs & args) {
  ofFill();
  float opacity = 255.0F * (m_endTime - ofGetElapsedTimef()) / m_duration;
  ofDrawBitmapStringHighlight(m_text
    , ofGetWidth() / 2
    , ofGetHeight() / 2
    , ofColor(ofColor::darkOliveGreen, opacity)
    , ofColor(ofColor::whiteSmoke, opacity));
}




