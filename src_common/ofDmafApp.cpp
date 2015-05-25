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

  thisApp = this;
  
  ofSetVerticalSync(true);
	
  ofSetSmoothLighting(true);

  
  DmDictionary params;
  params.setStringValue("settingsFile", this->getDmafConfigurationPath());
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
  
  #if defined(TARGET_OSX)
  m_usesMultitouch = false;
  m_pad = ofxMultiTouchPad();
  #endif
}


void ofDmafApp::subscribeToDmafTrigger(const char* eventName) {
  m_dmaf.addListener(eventName, dmafCb, NULL);
}


void ofDmafApp::exit() {
  m_isPlaying = false;
  m_audioMutex.lock();
  m_dmaf.deinitialize();
  m_audioMutex.unlock();
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
  m_mutex.lock();
  m_newDmafMessages.splice(m_newDmafMessages.end(), m_dmafMessagesShared);
  m_mutex.unlock();
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


//--------------------------------------------------------------
void ofDmafApp::keyPressed(int key){
#if defined(TARGET_OSX)
  if (key == 'm') {
    m_usesMultitouch = !m_usesMultitouch;
    AlexUtilityBubble*foo(new AlexUtilityBubble(m_usesMultitouch?"Using multitouch":"Using mouse"));
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




