//
//  ofDmTestApp.h

//
//  Created by Alessandro Saccoia on 5/23/15.
//
//

#ifndef __ofDmTestApp__
#define __ofDmTestApp__

#include "ofDmafApp.h"

class ofDmTestApp : public ofDmafApp {
  void setup();
  void update();
  void draw();
  const char* getDmafConfigurationPath();
};

#endif /* defined(__project_osx__ofDmTestApp__) */
