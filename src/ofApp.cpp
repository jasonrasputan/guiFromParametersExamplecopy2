#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    
    //ringButton.addListener(this,&ofApp::ringButtonPressed);
    buttonState = "digital pin:";
    potValue = "analog pin:";
    
    //gui.setup("panel"); // most of the time you don't need a name but don't forget to call setup
    //gui.add(ringButton.setup("ring"));
    font.loadFont("franklinGothic.otf", 20);
    //arduino
    ard.connect("/dev/cu.usbmodemfa131", 57600);
    
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    //endarduino
    
    bHide = true;
    
    durLength = 0;
    
    ring.loadSound("zenbell.mp3");
    ofSetFrameRate( 15 ) ;
    bNoteAdded = false ;
    bStillPressing = false ;
    xml.load( "data.xml" )  ;
     xml.setTo("notes") ;
    currentNote = 0;
    
    bPressState = 0;
    
    int numNotes = xml.getNumChildren() ;
    if ( numNotes > 0 )
    {
        
        //for ( int i  = numNotes.size()-1 ; i > 0  ; i-- )
        for ( int i  = 0; i < numNotes  ; i++ )
        {
            DateTime dt ;
            string branch = "note[" + ofToString( i ) + "]" ;
             dt.hour =  xml.getValue<int>(branch+"hours") ;
             dt.minute =  xml.getValue<int>(branch+"minutes") ;
             dt.second =  xml.getValue<int>(branch+"seconds") ;
             dt.value = xml.getValue<int>(branch+"value") ;
            
            
            int totalSeconds = 0 ;
            totalSeconds = dt.hour * 60 * 60 ;
            totalSeconds += dt.minute * 60 ;
            totalSeconds += dt.second ;
            
            
            
            notes.push_back( dt ) ;
            //int selectedIndex = ofMap( totalSeconds , 0 , secondsInDay , 0 , numFrames, true ) ;
            //frames[ selectedIndex ] = true ;
            
            ofLogNotice() << " note [ " << i << " ] is : " << dt.toString() << " selected Index is :" << 0 ;  ;
            
        }
        
        
    }
    else
    {
        
    }
    
    
    lastTime == -1 ;
    
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    ringButton.removeListener(this,&ofApp::ringButtonPressed);
}

//--------------------------------------------------------------


//--------------------------------------------------------------
void ofApp::ringButtonPressed(){
    
    
}

//-------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::update(){
    
    updateArduino();
    //VVV new VVV
    if (ard.getDigital(2)){
    pinPressed(2);
    }
    if (ard.getDigital(3)){
        pinPressed(3);
    }
    if (ard.getDigital(4)){
        pinPressed(4);
    }
    if (ard.getDigital(5)){
        pinPressed(5);
    }
    if (ard.getDigital(6)){
        pinPressed(6);
    }
    /*
     if ( ofGetMousePressed() && frames[currentFrame]== true )
     {
     
     ring.play();
     }*/
    
}
//--------------------------------------------------------------

void ofApp::setupArduino(const int & version) {
    
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // Note: pins A0 - A5 can be used as digital input and output.
    // Refer to them as pins 14 - 19 if using StandardFirmata from Arduino 1.0.
    // If using Arduino 0022 or older, then use 16 - 21.
    // Firmata pin numbering changed in version 2.3 (which is included in Arduino 1.0)
    
    // set pins D2 and A5 to digital input
    ard.sendDigitalPinMode(2, ARD_INPUT);
    ard.sendDigitalPinMode(3, ARD_INPUT);
    ard.sendDigitalPinMode(4, ARD_INPUT);
    ard.sendDigitalPinMode(5, ARD_INPUT);
    ard.sendDigitalPinMode(6, ARD_INPUT);
    
    ard.sendDigitalPinMode(19, ARD_INPUT);  // pin 21 if using StandardFirmata from Arduino 0022 or older
    
    // set pin A0 to analog input
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    
    // set pin D13 as digital output
    ard.sendDigitalPinMode(13, ARD_OUTPUT);
    // set pin A4 as digital output
    ard.sendDigitalPinMode(18, ARD_OUTPUT);  // pin 20 if using StandardFirmata from Arduino 0022 or older
    
    // set pin D11 as PWM (analog output)
    ard.sendDigitalPinMode(11, ARD_PWM);
    
    // attach a servo to pin D9
    // servo motors can only be attached to pin D3, D5, D6, D9, D10, or D11
    ard.sendServoAttach(9);
    
    // Listen for changes on the digital and analog pins
    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}

//--------------------------------------------------------------
void ofApp::updateArduino(){
    
    // update the arduino, get any data or messages.
    // the call to ard.update() is required
    ard.update();
    
    // do not send anything until the arduino has been set up
    if (bSetupArduino) {
        // fade the led connected to pin D11
        ard.sendPwm(11, (int)(128 + 128 * sin(ofGetElapsedTimef())));   // pwm...
    }
    
}

// digital pin event handler, called whenever a digital pin value has changed
// note: if an analog pin has been set as a digital pin, it will be handled
// by the digitalPinChanged function rather than the analogPinChanged function.
//--------------------------------------------------------------

//VVVV new VVVV
void ofApp::pinPressed(const int & pinNum) {
    int note = -1 ;
    
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
    
    if(ard.getDigital(pinNum))
    {
        ofLogNotice() << "digital pin " << ard.getDigital(pinNum);
        
        bNoteAdded = false ;
        
        
        bStillPressing = true ;
        
       
        if ( (lastTime + duration) < ofGetSystemTime() )
        {
            ofLogNotice() << "current time" << ofGetSystemTime();
            ofLogNotice() << "last time" << lastTime;
            ofLogNotice() << "current note " << currentNote;
            
            DateTime dt = notes[ currentNote ] ;
            switch( dt.value )
            {
                case 0 :
                    ring.setSpeed( 0.75f );
                    break ;
                    
                case 1 :
                    ring.setSpeed( 0.92 ) ;
                    break ;
                    
                case 2 :
                    ring.setSpeed( 1.0f ) ;
                    break ;
                    
                case 3 :
                    ring.setSpeed( 1.12 ) ;
                    break ;
                    
                case 4:
                    ring.setSpeed( 1.25 ) ;
                    break ;
            }
            ring.play() ;
            currentNote-- ;
            if ( currentNote < 0 )
            {
                currentNote = 0 ;
            }
            duration = 1000 ;
            lastTime = ofGetSystemTime() ;
        }
        
        
    }
}
//--------------------------------------------------------------
void ofApp::digitalPinChanged(const int & pinNum) {
    int note = -1 ;
    
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
    
    if(ard.getDigital(pinNum))
    {
        
        ofLogNotice() << "digital pin " << ard.getDigital(pinNum);
        durStart = ofGetSystemTime();
        durLength = 0;
        bNoteAdded = false ;
        
      
            bStillPressing = true ;
            
        
    } else {
        durEnd = ofGetSystemTime();
        durLength = (durEnd - durStart);
        bStillPressing = false ;
        
        if (durLength > 0 && durLength < 2000){
        note = 0 ;
        }
        if (durLength > 2000 && durLength < 3000){
            note = 1 ;
        }
        if (durLength > 3000 && durLength < 4000){
            note = 2 ;
        }
        if (durLength > 4000 && durLength < 5000){
            note = 3 ;
        }
        if (durLength > 5000 && durLength < 6000){
            note = 4 ;
        }


        if ( note != -1 )
        {
            if ( bNoteAdded == false )
            {
                bNoteAdded = true ;
                ofXml noteXml ;
                noteXml.addChild("note" ) ;
                noteXml.setTo("note") ;
                //noteXml.setTo("note[" + ofToString( tagId ) + "]" ) ;
                noteXml.addValue( "hours" , ofGetHours() ) ;
                noteXml.addValue( "minutes" , ofGetMinutes() ) ;
                noteXml.addValue( "seconds" , ofGetSeconds() ) ;
                noteXml.addValue( "value" , note ) ;
                //xml.setTo("notes") ;
                
                xml.addXml( noteXml ) ;
                
                xml.save( "data.xml" ) ;
                
                DateTime dt ;
                dt.hour = ofGetHours() ;
                dt.minute = ofGetMinutes() ;
                dt.second = ofGetSeconds() ;
                dt.value = note ;
                notes.push_back( dt ) ;
                currentNote = notes.size() - 1 ;
                
                
                
                
                
                
                switch( dt.value )
                {
                    case 0 :
                        ring.setSpeed( 0.75f );
                        break ;
                        
                    case 1 :
                        ring.setSpeed( 0.92 ) ;
                        break ;
                        
                    case 2 :
                        ring.setSpeed( 1.0f ) ;
                        break ;
                        
                    case 3 :
                        ring.setSpeed( 1.12 ) ;
                        break ;
                        
                    case 4:
                        ring.setSpeed( 1.25 ) ;
                        break ;
                }
                ring.play() ;
                currentNote-- ;
                if ( currentNote < 0 )
                {
                    currentNote = 0 ;
                }
                duration = 5000 ;
                //lastTime = ofGetSystemTime() ;
                
            }

        }
    }
}

// analog pin event handler, called whenever an analog pin value has changed


//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
}


//--------------------------------------------------------------


void ofApp::draw(){
    
    
    ofBackgroundGradient(ofColor::white, ofColor::gray);
    
    // DateTime dt = notes[ currentNote ] ;
    font.drawString("press duration: " + ofToString((int) (durLength)), 515, 70);
    
    if (!bSetupArduino){
        font.drawString("arduino not ready...\n", 515, 40);
    } else {
        font.drawString(buttonState, 515, 40);
        
        
        
    }
    
    
    
    /* if ( frames[currentFrame]== true )
     {
     
     ring.play();
     }*/
    
    float maxWidth = 40 ;
    
    if ( notes.size() > 0 )
    {
        float w = ofGetWidth() / notes.size() ;
        if ( w > maxWidth )
            w = maxWidth ;
        
        
        for (int i = 0; i < notes.size() ; i++)
        {
            if ( i == (currentNote + 1) )
                ofSetColor( 0 , 255 , 0 ) ;
            else
                ofSetColor( 255 ) ;
            int valueY = ofMap( notes[i].value , 0.0f , 5.0f , 0 , ofGetHeight() ) ;
            ofRect( i * w ,valueY ,  w , 15 ) ;
            
        }
        
        
    }
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  }

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    
}

//--------------------------------------------------------------
//void addTimeDuration() {

//}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    //ring.play();
    
    // frames[currentFrame] = true;
    
    
    //int tagId = xml.addChild( "note" ) ;
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
