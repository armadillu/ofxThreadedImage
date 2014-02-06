
#include "ofxThreadedImage.h"

ofxThreadedImage::ofxThreadedImage(){
	pendingTexture = false;
	timeOut = 10;
	alpha = 1;
	alphaRiseSpeed = 0.05;
	imageLoaded = false;
}

ofxThreadedImage::~ofxThreadedImage(){
	if (isThreadRunning()){
		waitForThread(false);
	}
}


void ofxThreadedImage::threadedFunction(){

	if( lock() ){
		
		switch (whatToDo) {
			case SAVE:
				ofSaveImage(getPixelsRef(), fileName, quality);
				break;
				
			case LOAD:
				alpha = 0.0f;
				loadImageBlocking(fileName);
				break;
				
			case LOAD_HTTP:	
				alpha = 0;
				ofxSimpleHttp http;
				http.setTimeOut(timeOut);
				ofxSimpleHttpResponse response = http.fetchURLBlocking(url);
				if (response.status == 200){

					ofDirectory dir;
					dir.open(ofToDataPath(IMG_DOWNLOAD_FOLDER_NAME, false));
					if ( !dir.exists() ){
						dir.create();
					}
					string filePath = ofToDataPath( (string)IMG_DOWNLOAD_FOLDER_NAME + "/" + response.fileName, false );
					FILE * file = fopen( filePath.c_str(), "wb");
					fwrite (response.responseBody.c_str() , 1 , response.responseBody.length() , file );
					fclose( file);
					imageLoaded = false;
					setUseTexture(false);
					bool loaded = loadImage((string)IMG_DOWNLOAD_FOLDER_NAME + "/" + response.fileName);
					if (loaded){
						imageLoaded = true;
					}else{
						ofLog(OF_LOG_ERROR, "loadHttpImageBlocking() failed to load from disk (%d) > %s\n", response.status, url.c_str() );
					}
				}else{
					ofLog(OF_LOG_ERROR, "loadHttpImageBlocking() failed to download (%d) > %s\n", response.status, url.c_str() );
				}
				
				break;
		}		
		unlock();
		
	} else {
		printf("ofxThreadedImage::threadedFunction Can't %s %s, thread is already running", whatToDo == SAVE ? "Save" : "Load",  fileName.c_str() );
	}
	stopThread();
}


void ofxThreadedImage::loadImageThreaded(string fileName_){
	alpha = 0.0;
	whatToDo = LOAD;
	fileName = fileName_;
	startThread(true, false);
}


void ofxThreadedImage::loadImageBlocking(string fileName){
	imageLoaded = false;
	whatToDo = LOAD;
	setUseTexture(false);
	alpha = 0.0;
	bool loaded = loadImage(fileName);
	if (loaded){
		pendingTexture = true;
		imageLoaded = true;
	}
}


bool ofxThreadedImage::loadHttpImageBlocking(string url_){
	alpha = 0;
	whatToDo = LOAD_HTTP;
	url = url_;
	pendingTexture = true;
	setUseTexture(false);
	ofxSimpleHttp http;
	http.setTimeOut(timeOut);
	ofxSimpleHttpResponse response = http.fetchURLBlocking(url);
	if (response.status == 200){
		ofDirectory dir;
		dir.open(ofToDataPath(IMG_DOWNLOAD_FOLDER_NAME, false));
		if ( !dir.exists()){
			dir.create();
		}
		string filePath = ofToDataPath( (string)IMG_DOWNLOAD_FOLDER_NAME + "/" + response.fileName, false );
		FILE * file = fopen( filePath.c_str(), "wb");
		fwrite (response.responseBody.c_str() , 1 , response.responseBody.length() , file );
		fclose( file);
	}else{
		ofLog(OF_LOG_ERROR, "loadHttpImageBlocking() failed (%d) > %s\n", response.status, url.c_str() );
		return false;
	}
	imageLoaded = false;
	bool ok = loadImage((string)IMG_DOWNLOAD_FOLDER_NAME + "/" + response.fileName);
	if(ok){
		imageLoaded = true;
	}
	return ok;
}


bool ofxThreadedImage::loadHttpImageThreaded(string url_){
	alpha = 0;
	whatToDo = LOAD_HTTP;
	url = url_;
	pendingTexture = true;	
	setUseTexture(false);
	startThread(true, false);
	return true; //TODO! 
}


bool ofxThreadedImage::arePixelsAvailable(){
	return imageLoaded; //TODO!
}

void ofxThreadedImage::updateTextureIfNeeded(){
	if (pendingTexture){
 		setUseTexture(true);
		tex.allocate(getPixelsRef());
		update();
		pendingTexture = false;
	}	
}

void ofxThreadedImage::saveThreaded(string where, ofImageQualityType quality_){
	whatToDo = SAVE;
	this->fileName = where;
	this->quality = quality_;
	startThread(false, false);   // !blocking, !verbose
};


void ofxThreadedImage::draw(float _x, float _y){
	ofxThreadedImage::draw(_x, _y, getPixelsRef().getWidth(), getPixelsRef().getHeight() );
}

void ofxThreadedImage::draw(float _x, float _y, float _w, float _h, bool fadeInOnLoad){

	if(imageLoaded){
		updateTextureIfNeeded();
	}

	if (imageLoaded && fadeInOnLoad && alpha < 1.0f){
		if (whatToDo == LOAD_HTTP || whatToDo == LOAD){
			alpha += alphaRiseSpeed;
			if(alpha > 1.0) alpha = 1.0;
		}
		glPushAttrib(GL_CURRENT_BIT);
			glColor4ub(255,255,255, 255 * alpha);
			ofImage::draw(_x, _y, _w, _h);
		glPopAttrib();
	}else{
		if(tex.bAllocated()){
			ofImage::draw(_x, _y, _w, _h);
		}
	}
}

