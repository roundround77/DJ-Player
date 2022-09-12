
/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 21 Feb 2022 3:11:13pm
    Author:  Asus

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class DJAudioPlayer : public AudioSource {
public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    double getLengthInSeconds();
    void loadURL(URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    void start();
    void stop();

    //get the relative position of the playhead
    double getPositionRelative();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
};