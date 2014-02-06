#include "testApp.h"

void testApp::setup(){

	ofEnableAlphaBlending();
	ofBackground(22);
}


void testApp::draw(){

	img.draw(0,0, ofGetWidth(), ofGetHeight(), true);

	//rotating cube
	glPushMatrix();
	glTranslatef(ofGetWidth() - 60,60, 0);
	glRotatef( ofGetFrameNum() * 3, 0,0,1);
	glColor3ub(255,255,255);
	float h = 5;
	ofRect(-h/2,h/2, h,50);
	glPopMatrix();

	ofDrawBitmapStringHighlight("press '1' to load image from http on main thread\n"
								"press '2' to load disk image on background thread\n"
								"press '3' to load image from http on main thread\n"
								"press '4' to load image from http on background thread\n"
								"press '5' to save current image to disk in back thread",
								20, ofGetHeight() - 80
								);
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
