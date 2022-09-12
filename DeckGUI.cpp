/*
  ==============================================================================

    DeckGUI.cpp
    Created: 21 Feb 2022 4:28:43pm
    Author:  Asus

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse) : player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    //background Image
    backgroundImage = ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize);

    // add all components and make visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(waveformDisplay);

    // add listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    //settings for Play/Stop/Load Button
    playButton.setColour(TextButton::ColourIds::buttonColourId, Colours::green);
    stopButton.setColour(TextButton::ColourIds::buttonColourId, Colours::red);
    loadButton.setColour(TextButton::ColourIds::buttonColourId, Colours::blue);

    //Settings for volume slider/label
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5); //default volume half of max vol
    volSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    volSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setColour(Label::textColourId, Colours::greenyellow);
    volLabel.attachToComponent(&volSlider, false);
    volLabel.setJustificationType(juce::Justification::centred);

    ////Settings for Speed slider/label
    speedSlider.setRange(0.5, 2, 0); //min half speed, max speed 2x
    speedSlider.setValue(1); //default speed at 1x
    speedSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    speedSlider.setMouseDragSensitivity(80);
    speedLabel.setText("Speed Controller", juce::dontSendNotification);
    speedLabel.setColour(Label::textColourId, Colours::greenyellow);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centred);

    //Settings for Position slider
    posSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);



    //set colour scheme for sliders 
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::mediumspringgreen); //dial
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::yellow); //body
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::pink); //body


    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
    g.setColour(juce::Colours::yellow);
    g.drawRect(getLocalBounds(), 10);   // draw an outline around the component
    g.drawImage(backgroundImage, getLocalBounds().toFloat()); // draw image

}

void DeckGUI::resized()
{
    // position of slider, label, buttons on GUI
    double rowH = getHeight() / 6;
    double colW = getWidth() / 4;

    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2);

    posSlider.setBounds(0, rowH * 2, getWidth(), rowH);
    volSlider.setBounds(5, rowH * 3 + 20, colW, rowH * 3 - 30);
    speedSlider.setBounds(colW, rowH * 3 + 20, colW * 2, rowH * 2);

    playButton.setBounds(colW * 3 + 10, rowH * 3 + 10, colW - 20, rowH - 20);
    stopButton.setBounds(colW * 3 + 10, rowH * 4 + 10, colW - 20, rowH - 20);
    loadButton.setBounds(colW * 3 + 10, rowH * 5 + 10, colW - 20, rowH - 20);


}


void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        std::cout << "Play Button was clicked " << std::endl;
        player->start();

    }
    if (button == &stopButton)
    {
        std::cout << "Stop Button was clicked " << std::endl;
        player->stop();
    }
    if (button == &loadButton)
    {
        FileChooser chooser{ "Select a file..." };
        if (chooser.browseForFileToOpen())
        {
            player->loadURL(URL{ chooser.getResult() });
            waveformDisplay.loadURL(URL{ chooser.getResult() });
        }
    }

}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    std::cout << "DeckGUI::filesDropped" << std::endl;
    if (files.size() == 1)
    {
        player->loadURL(URL{ File{files[0]} });
    }
}

void DeckGUI::loadFile(juce::URL audioURL)
{
    DBG("DeckGUI::loadFile called");
    player->loadURL(audioURL);
    waveformDisplay.loadURL(audioURL);
}

void DeckGUI::timerCallback()
{
    std::cout << "DeckGUI::timerCallback()" << std::endl;
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}
