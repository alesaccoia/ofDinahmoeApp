//
//  ofDmTestApp.cpp
//  project_osx
//
//  Created by Alessandro Saccoia on 5/23/15.
//
//

#include "ofDmTestApp.h"

void ofDmTestApp::setup() {
  ofDmafApp::setup();
}

void ofDmTestApp::update() {
  ofDmafApp::update();
}

void ofDmTestApp::draw() {

}

const char* ofDmTestApp::getDmafConfigurationPath() {
  return ofToDataPath("android_tempo/xml/settings.xml").c_str();
}

