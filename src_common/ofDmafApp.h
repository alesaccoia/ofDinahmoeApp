
/**
  @date 5/23/2015
  @author  Alessandro Saccoia <alessandro@dinahmoe.com>
  @copyright 2015 Dinahmoe

  This file encapsulates the following things:
    - init/deinit across the various platforms
    - multi touch vs single mouse clicks
    - dmaf initialization
    - dmaf messages demultiplexing

  It doesn't implement any draw method that remains pure virtual, so this
  class is here to be subclassed.
*/
#pragma once

#include "ofMain.h"
#include "ofxDmaf.h"
#include "ofxGui.h"

#include <list>

#if defined(TARGET_ANDROID)
#include "ofxAndroid.h"
#endif

#if defined(TARGET_OSX)
#include "ofxMultiTouchPad.h"
#endif

#if defined(TARGET_OSX) || defined(TARGET_IOS)
class ofDmafApp : public ofBaseApp{
#elif defined(TARGET_ANDROID)
class ofDmafApp : public ofxAndroidApp{
#endif
	public:
    // subclasses methods' need to call the relative ofDmafApp methods
		virtual void setup();
		virtual void update();
    virtual void exit();
  
    // pure virtual: we don't draw anything
    virtual void draw() = 0;
    virtual const char* getDmafConfigurationPath() = 0;
  
    // calls dmaf process
    virtual void audioRequested(float * output, int bufferSize, int nChannels );

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
  
  
    #if defined(TARGET_OSX) || defined(TARGET_IOS)
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    #endif
  
    #if defined(TARGET_IOS)
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);

    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
    #endif // defined(TARGET_IOS)    
		
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
    #endif // defined(TARGET_ANDROID)
  
  
   protected:
    DmafCore m_dmaf;
  
  
  
    // callback related stuff: the internal dmaf dispatcher runs on another
    // thread: we can't call openGL methods from other threads so what happens
    // is:
    // - user code subscribes to events with subscribeToDmafTrigger
    // - the callback pushes messages in a queue: m_dmafMessagesShared
    // - at each update() m_dmafMessagesShared is checked: if it contains
    //   something, the content is spliced to the m_newDmafMessages that can
    //   be safely accessed.
  
    // this is the favorite way to subscribe to triggers
    void subscribeToDmafTrigger(const char* eventName);
  
    typedef struct DmafCallbackArgs {
      DmafCallbackArgs() {
      
      }
      std::string trigger;
      float time;
      int midinote;
    } DmafCallbackArgs;
  
    ofMutex m_mutex;
    std::list<DmafCallbackArgs> m_dmafMessagesShared; // protected by m_mutex
    std::list<DmafCallbackArgs> m_newDmafMessages; // protected by m_mutex
   public:
    void dmafDirectCallback(const char* trigger_, float time_, DmParametersPOD params_, void* args);
   private:
    float** m_tempOutputBuffer;
    volatile bool m_isPlaying;
    ofMutex m_audioMutex;
  
    #if defined(TARGET_OSX)
    bool m_usesMultitouch;
    ofxMultiTouchPad m_pad;
    #endif
};

class AlexUtilityBubble {
 public:
  AlexUtilityBubble(std::string text_, float duration_ = 3.0F);
  ~AlexUtilityBubble();
  void update(ofEventArgs & args);
  void draw(ofEventArgs & args);
 private:
  std::string m_text;
  float m_duration;
  float m_endTime;
};
