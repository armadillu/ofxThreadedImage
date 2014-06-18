#pragma once

#include "ofMain.h"
#include "ofxSimpleHttp.h"

/*
 
 This is an extension of the basic ofImage, allowing:
 
	- save to jpeg and specify jpeg quality
	- save to disk in a different thread
	- load from disk threaded (with a drawback)

	load from disk threaded:
 
	this will work automatically if you draw from the supplied ofxThreadedImage::draw();
	It will not work if you try to draw the texture directly (ofImage.getTextureReference().draw())
	if you want to draw thorugh the texture, make sure you call updateTextureIfNeeded() before doing so
 
	learn by example:
 
		ofxThreadedImage img;
	 
		void testApp::setup(){
			ofAddListener(imageReadyEvent, this, &testApp::imageReady); //
			img.loadImageThreaded( "monkey.jpeg" );
		}

		void testApp::newResponse(ofxThreadedImage& img){
			//img is ready to draw!
		}

		void testApp::draw(){
			img.draw(0,0);
		}

		this will start loading the image in a secondary thread, not blocking.
		it is likely the computer will try to draw the image b4 it is loaded by the second thread;
		it will be drawn as soon as it becomes available, and it won't block your main thread.
 
 */

#define IMG_DOWNLOAD_FOLDER_NAME "ofxThreadedImageDownloads"

//if an image requies resizing, do it with openCV of with regular ofImage methods (much slower)
#ifndef USE_OPENCV_TO_RESIZE
//#define USE_OPENCV_TO_RESIZE	true
#endif

class ofxThreadedImage;

struct ofxThreadedImageEvent{
	bool				loaded;
	ofxThreadedImage *	image;
	ofxThreadedImageEvent(){
		loaded = true;
		image = NULL;
	}
};


class ofxThreadedImage : public ofThread, public ofImage {

	public:

		enum Action		{ SAVE = 0, LOAD, LOAD_HTTP };

		string					fileName;
		string					url;
		ofImageQualityType		quality;
		bool					jpeg;
		bool					pendingTexture;
		Action					whatToDo;
				
		ofxThreadedImage();	
		virtual ~ofxThreadedImage();

		void setTexCompression(ofTexCompression c);
	
		//this will load the pixels only, not GL upload.
		//Will also store a flag so that when u try to draw first, it's uploaded to GL on demand (on main thread)
		void loadImageBlocking(string fileName);
		void loadImageThreaded(string fileName);

		void constrainImageSize(int largestSide); //if the loaded image turns out to be larger than this
	//(bigger side), then we shrink it down to fit this new size (for the largest side)

		bool loadHttpImageBlocking(string url);
		void loadHttpImageThreaded(string url);
		void setHttpRequestTimeOut(float t){ timeOut = t;}

		//save file as jpeg with custom quality (regardless of file extension!)
		void saveThreaded(string where, ofImageQualityType qualityLevel);

		//call this method before draw, only in case you draw through the textureReference.
		//If you call ofxThreadedImage::draw(), there's no need to use this
		void updateTextureIfNeeded();	

		//to find out if the background thread has loaded pixels (so we are ready to send to GPU)
		bool arePixelsAvailable();

		//wrappers for ofImage draw, so that we can load the pixels to GL before drawing if necessary
		void draw(float _x, float _y, bool fadeInOnDelayedLoad = true);
		void draw(float _x, float _y, float _w, float _h, bool fadeInOnDelayedLoad = true);

		void update(); //this is only needed if you want notifications when the img is loaded
		bool isReadyToDraw();

		/*per frame alpha increment [0..1]*/
		void setFadeInSpeed(float alphaRiseSpeed_){ alphaRiseSpeed = alphaRiseSpeed_; }

		ofEvent<ofxThreadedImageEvent>		imageReadyEvent;

	private:

		ofTexCompression compression;
		void threadedFunction();
	void resizeIfNeeded();

		float timeOut;
		bool pendingNotification;
		float alpha;
		float alphaRiseSpeed;
		bool imageLoaded;		//pixels are ready
		bool readyToDraw;	//tex is ready
		bool problemLoading;

	bool resizeAfterLoad;
	int maxSideSize; //if resize, constrain into this side for the largest side
};

