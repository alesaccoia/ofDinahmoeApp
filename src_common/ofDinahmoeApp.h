#pragma once

#include "ofMain.h"
#include "ofxDmaf.h"

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
  
    ofThread m_initializationThread;
    float** m_tempOutputBuffer;
};
