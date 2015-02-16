#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "DateTime.h"
#include "ofEvents.h"
#include "ofArduino.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		

    void ringButtonPressed();

    void addTimeDuration();
    
    void pinPressed(const int & pinNum);
    
	bool bHide;
    
    int durStart;
    int durEnd;
    int durLength;

    ofxButton ringButton;
   
	ofxPanel gui;

	ofSoundPlayer ring;

    vector< DateTime > notes ;
    
    ofXml xml ;
    
    ofTrueTypeFont		font;



    int currentNote ;
    
    unsigned long long lastTime ;
    unsigned long long duration ;
    
    bool bNoteAdded ;
    bool bStillPressing ;
    
    bool bPressState ;
    
    //arduino
    
    ofArduino	ard;
    bool		bSetupArduino;			// flag variable for setting up arduino once
    
private:
    
    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
    void updateArduino();
    void getDigital();
    
    string buttonState;
    string potValue;
    int buttonVal;
    int oldstate;

    
    
    
    

};

