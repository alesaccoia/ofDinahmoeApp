#include "ofDinahmoeApp.h"

#define SAMPLING_RATE 44100.F
#define INPUT_CHANNELS 0
#define OUTPUT_CHANNELS 2
#define BUFFER_SIZE 512

//--------------------------------------------------------------
void ofDinahmoeApp::setup(){
  ofSetFrameRate(30);
  ofSetWindowShape(640,480);
  m_logo.load("DinahMoeLogo.png");
  
//  int bufferSize		= 512;
//	sampleRate 			= 44100;
//  soundStream.setup(this, 2, 0, 44100, 512, 4);
  
  DmDictionary params;
  params.setStringValue("settingsFile", ofToDataPath("android_tempo/xml/settings.xml").c_str());
  m_dmaf.initialize(params);
  
  // create temp buffer
  m_tempOutputBuffer = new float*[OUTPUT_CHANNELS];
  for (int ch = 0; ch < OUTPUT_CHANNELS; ++ch) {
    m_tempOutputBuffer[ch] = new float[BUFFER_SIZE];
  }
  
  //
  ofSoundStreamSetup(OUTPUT_CHANNELS, INPUT_CHANNELS, this, SAMPLING_RATE, BUFFER_SIZE, 4);
  
 
}

//--------------------------------------------------------------
void ofDinahmoeApp::update(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::draw(){
  ofSetColor(ofColor::white);
  m_logo.draw(0, 0, ofGetWidth(), ofGetHeight());
}


// Audio


void ofDinahmoeApp::audioRequested(float * output, int bufferSize, int nChannels) {
  assert(bufferSize == BUFFER_SIZE);
  assert(nChannels == OUTPUT_CHANNELS);
  m_dmaf.process(0, NULL, nChannels, m_tempOutputBuffer, bufferSize);
  float * outputSample = output;
  for (int i = 0; i < BUFFER_SIZE; ++i) {
    for (int ch = 0; ch < OUTPUT_CHANNELS; ++ch) {
      *outputSample = m_tempOutputBuffer[ch][i];
      ++outputSample;
    }
  }
}

//--------------------------------------------------------------
void ofDinahmoeApp::keyPressed(int key){
  m_dmaf.tell("play");
}

//--------------------------------------------------------------
void ofDinahmoeApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofDinahmoeApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofDinahmoeApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofDinahmoeApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofDinahmoeApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofDinahmoeApp::windowResized(int w, int h){

}


#if defined(TARGET_OSX) || defined(TARGET_IOS)

//--------------------------------------------------------------
void ofDinahmoeApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofDinahmoeApp::dragEvent(ofDragInfo dragInfo){ 

}

#endif

#if defined(TARGET_IOS)

//--------------------------------------------------------------
void ofDinahmoeApp::exit(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofDinahmoeApp::lostFocus(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::gotFocus(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::deviceOrientationChanged(int newOrientation){

}

#endif

#if defined(TARGET_ANDROID)

//--------------------------------------------------------------
void ofDinahmoeApp::touchDown(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchUp(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDinahmoeApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofDinahmoeApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofDinahmoeApp::pause(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::stop(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::resume(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::reloadTextures(){

}

//--------------------------------------------------------------
bool ofDinahmoeApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofDinahmoeApp::okPressed(){

}

//--------------------------------------------------------------
void ofDinahmoeApp::cancelPressed(){

}
#endif
