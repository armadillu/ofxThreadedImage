#include "testApp.h"

void testApp::setup(){

	ofEnableAlphaBlending();
	ofBackground(22);
}

void testApp::update(){
 
    img.update();
}

void testApp::draw(){

	img.draw(0,0, ofGetWidth(), ofGetHeight(), true);

	//draw rotating thingie to see blocking calls

	ofPushMatrix();

	ofTranslate(ofGetWidth() - 60,60, 0);
	ofRotate( ofGetFrameNum() * 3, 0,0,1);
    ofSetColor(255,255,255);
	float h = 5;
	ofRect(-h/2,h/2, h,50);

	ofPopMatrix();

	//instructions
	ofDrawBitmapStringHighlight("press '1' to load image from local disk on main thread\n"
								"press '2' to load image from local disk on background thread\n"
								"press '3' to load image from http on main thread\n"
								"press '4' to load image from http on background thread\n"
								"press '5' to save current image to disk in a background thread",
								20, ofGetHeight() - 80
								);

	img.constrainImageSize(200); //this will make any image loaded fit in this size!
}


void testApp::keyPressed(int key){

	string file = "img.jpg";
	string url = "http://farm8.staticflickr.com/7420/10032530563_86ff701d19_o.jpg";

	if(key=='1'){
		img.loadImageBlocking( file );
	}

	if(key=='2'){
		img.loadImageThreaded( file );
	}

	if(key=='3'){
		img.loadHttpImageBlocking( url );
	}

	if(key=='4'){
		img.loadHttpImageThreaded( url );
	}

	if(key=='5'){
		img.saveThreaded("out.jpg", OF_IMAGE_QUALITY_WORST);
	}
}
