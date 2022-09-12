/*
  ==============================================================================

    WaveformDisplay.h
    Created: 21 Feb 2022 5:47:19pm
    Author:  Asus

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class WaveformDisplay  : public juce::Component,
                         public ChangeListener
{
public:
    WaveformDisplay(AudioFormatManager& formatManagerToUse,
                    AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void loadURL(URL audioURL);
    //set the relative position of the playhead
    void setPositionRelative(double pos);

private:
    int id;
    AudioThumbnail audioThumb;
    bool fileLoaded;
    double position;
    String fileName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
