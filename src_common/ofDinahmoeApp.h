#pragma once

#include "ofMain.h"
#include "ofxDmaf.h"
#include "ofxGui.h"

#include <list>


#if defined(TARGET_ANDROID)
#include "ofxAndroid.h"
#endif

#if defined(TARGET_OSX) || defined(TARGET_IOS)
class ofDinahmoeApp : public ofBaseApp{
#elif defined(TARGET_ANDROID)
class ofDinahmoeApp : public ofxAndroidApp{
#endif
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
  
    void audioRequested(float * output, int bufferSize, int nChannels );
  
    #if defined(TARGET_OSX) || defined(TARGET_IOS)
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    #endif
  
    #if defined(TARGET_IOS)    
    void exit();
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);

    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
    #endif
		
    #if defined(TARGET_ANDROID)
    void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();
    #endif
  
    ofImage m_logo;
    DmafCore m_dmaf;
  
    typedef struct DmafCallbackArgs {
      DmafCallbackArgs() {
      
      }
      std::string trigger;
      float time;
      int midinote;
    } DmafCallbackArgs;
  
    ofMutex m_mutex;
    std::list<DmafCallbackArgs> m_dmafMessagesShared; // protected
    std::list<DmafCallbackArgs> m_newDmafMessages; // protected
  
    void dmafDirectCallback(const char* trigger_, float time_, DmParametersPOD params_, void* args);
  
    ofThread m_initializationThread;
    float** m_tempOutputBuffer;
  
    ofxPanel m_gui;
  
    ofxToggle m_playStop;
    ofxFloatSlider m_intensity;
    ofxFloatSlider m_rotation;
    ofxLabel m_currentTime;
    // cbs
    void playStateChanged(bool & playStop_);
    void intensityChanged(float & intensity_);
    void rotationChanged(float & rotation_);
  
  
    // graphics
  
    vector<ofPoint> vertices;
    vector<ofColor> colors;
    int nTri;       //The number of triangles
    int nVert;      //The number of the vertices equals nTri * 3
    ofLight m_directionalLight;
    ofLight m_spotLight;
    ofMaterial m_material;
  
    ofCamera m_camera;
  
    void setupGraphics();
  
    float scale;
    float colorScale;
};
