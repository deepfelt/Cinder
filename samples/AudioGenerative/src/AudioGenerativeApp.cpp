#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace ci::app;

class AudioGenerativeApp : public AppBasic {
 public:
	void setup();
	void mouseMove( MouseEvent event );
	void sineWave( uint64_t inSampleOffset, uint32_t ioSampleCount, audio::Buffer *ioBuffer );
	void draw();
	
	float mFreqTarget;
	float mPhase;
	float mPhaseAdjust;
	float mMaxFreq;
};

void AudioGenerativeApp::setup()
{
	mMaxFreq = 2000.0f;
	mFreqTarget = 0.0f;
	mPhase = 0.0f;
	mPhaseAdjust = 0.0f;
	
	audio::Output::addTrack( audio::createCallback( &AudioGenerativeApp::sineWave, *this ) );
}

void AudioGenerativeApp::mouseMove( MouseEvent event )
{
	int height = getWindowHeight();
	//gFreqTarget = ( height - event.getY() ) / (float)height * gMaxFreq/ 44100.0f;
	mFreqTarget = math<float>::clamp( ( height - event.getY() ) / (float)height * mMaxFreq, 0.0, mMaxFreq );
}

void AudioGenerativeApp::sineWave( uint64_t inSampleOffset, uint32_t ioSampleCount, audio::Buffer *ioBuffer ) {
	float * buffer = (float *)ioBuffer->mData;
	mPhaseAdjust = mPhaseAdjust * 0.95f + ( mFreqTarget / 44100.0f ) * 0.05f;
	for( int  i = 0; i < ioSampleCount; i++ ) {
		mPhase += mPhaseAdjust;
		float r = mPhase - math<float>::floor( mPhase );
		float val = math<float>::sin( r * 2.0f * M_PI );
		
		buffer[i*ioBuffer->mNumberChannels] = val;
		buffer[i*ioBuffer->mNumberChannels + 1] = val;
	}
}

void AudioGenerativeApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	
	// draw a representation of the sine wave
	const float audioToVisualScale = 10.0f;
	const float waveHeight = 200.0f;
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	glBegin( GL_LINE_STRIP );
	for( float x = -getWindowWidth() / 2; x < getWindowWidth() / 2; x += 0.5f ) {
		gl::vertex( getWindowCenter() + Vec2f( x, waveHeight * sin( x * mPhaseAdjust * audioToVisualScale ) ) );
	}
	glEnd();
}

CINDER_APP_BASIC( AudioGenerativeApp, RendererGl )
