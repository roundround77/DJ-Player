/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 22 Feb 2022 3:39:53am
    Author:  Asus

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1,
    DeckGUI* _deckGUI2,
    DJAudioPlayer* _playerForParsingMetaData
) : deckGUI1(_deckGUI1),
deckGUI2(_deckGUI2),
playerForParsingMetaData(_playerForParsingMetaData)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    // add components
    addAndMakeVisible(importButton);
    addAndMakeVisible(searchField);
    addAndMakeVisible(library);
    addAndMakeVisible(addToPlayer1Button);
    addAndMakeVisible(addToPlayer2Button);

    // attach listeners
    importButton.addListener(this);
    searchField.addListener(this);
    addToPlayer1Button.addListener(this);
    addToPlayer2Button.addListener(this);

    // searchField configuration
    searchField.setTextToShowWhenEmpty("Search Tracks (enter to submit)",
        juce::Colours::orange);
    searchField.onReturnKey = [this] { searchLibrary(searchField.getText()); };

    // setup table and load library from file
    library.getHeader().addColumn("Tracks", 1, 1);
    library.getHeader().addColumn("Length", 2, 1);
    library.getHeader().addColumn("Delete", 3, 1);
    library.setModel(this);
    loadLibrary();
}

PlaylistComponent::~PlaylistComponent()
{
    saveLibrary();
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
}

void PlaylistComponent::resized()
{
    double rowH = getHeight() / 8;

    searchField.setBounds(0, 0, getWidth(), rowH);
    library.setBounds(0, rowH, getWidth(), rowH * 5);
    importButton.setBounds(0, rowH * 6, getWidth(), rowH);
    addToPlayer1Button.setBounds(0, rowH * 7, getWidth() / 2, rowH);
    addToPlayer2Button.setBounds(getWidth() * 0.5, rowH * 7, getWidth() / 2, rowH);

    //set columns
    library.getHeader().setColumnWidth(1, 14 * getWidth() / 20);
    library.getHeader().setColumnWidth(2, 4 * getWidth() / 20);
    library.getHeader().setColumnWidth(3, 2 * getWidth() / 20);
}

int PlaylistComponent::getNumRows()
{
    return librarys.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected
)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::lightcyan);
    }
    else
    {
        g.fillAll(juce::Colours::blueviolet);
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected
)
{
    if(rowNumber < getNumRows()){
        if (rowNumber < getNumRows())
        {
            if (columnId == 1)
            {
                g.drawText(librarys[rowNumber].title,
                    2,
                    0,
                    width - 4,
                    height,
                    juce::Justification::centredLeft,
                    true
                );
            }


            if (columnId == 2)
            {
                g.drawText(librarys[rowNumber].length,
                    2,
                    0,
                    width - 4,
                    height,
                    juce::Justification::centred,
                    true
                );
            }
        }
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate)
{
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "X" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);

            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    if (button == &importButton)
    {
        DBG("Load button clicked");
        importToLibrary();
        library.updateContent();
    }
    else if (button == &addToPlayer1Button)
    {
        DBG("Add to Player 1 clicked");
        loadInPlayer(deckGUI1);
    }
    else if (button == &addToPlayer2Button)
    {
        DBG("Add to Player 2 clicked");
        loadInPlayer(deckGUI2);
    }
    else
    {
        int id = std::stoi(button->getComponentID().toStdString());
        DBG(librarys[id].title + " removed from Library");
        deleteFromLibrarys(id);
        library.updateContent();
    }
}

void PlaylistComponent::loadInPlayer(DeckGUI* deckGUI)
{
    int selectedRow{ library.getSelectedRow() };
    if (selectedRow != -1)
    {
        DBG("Adding: " << librarys[selectedRow].title << " to Player");
        deckGUI->loadFile(librarys[selectedRow].URL);
    }
    else
    {
        juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
            "Add to Deck Information:",
            "Please select a track to add to deck",
            "OK",
            nullptr
        );
    }
}

void PlaylistComponent::importToLibrary()
{
    DBG("PlaylistComponent::importToLibrary called");

    //initialize file chooser
    juce::FileChooser chooser{ "Select files" };
    if (chooser.browseForMultipleFilesToOpen())
    {
        for (const juce::File& file : chooser.getResults())
        {
            juce::String fileNameWithoutExtension{ file.getFileNameWithoutExtension() };
            if (!isInLibrarys(fileNameWithoutExtension)) // if not already loaded
            {
                Library newLibrary{ file };
                juce::URL audioURL{ file };
                newLibrary.length = getLength(audioURL);
                librarys.push_back(newLibrary);
                DBG("loaded file: " << newLibrary.title);
            }
            else // display msg
            {
                juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                    "Load information:",
                    fileNameWithoutExtension + " already loaded",
                    "OK",
                    nullptr
                );
            }
        }
    }
}

bool PlaylistComponent::isInLibrarys(juce::String fileNameWithoutExtension)
{
    return (std::find(librarys.begin(), librarys.end(), fileNameWithoutExtension) != librarys.end());
}

void PlaylistComponent::deleteFromLibrarys(int id)
{
    librarys.erase(librarys.begin() + id);
}

juce::String PlaylistComponent::getLength(juce::URL audioURL)
{
    playerForParsingMetaData->loadURL(audioURL);
    double seconds{ playerForParsingMetaData->getLengthInSeconds() };
    juce::String minutes{ secondsToMinutes(seconds) };
    return minutes;
}

juce::String PlaylistComponent::secondsToMinutes(double seconds)
{
    // making seconds and minutes into a string
    int secondsRounded{ int(std::round(seconds)) };
    juce::String min{ std::to_string(secondsRounded / 60) };
    juce::String sec{ std::to_string(secondsRounded % 60) };

    if (sec.length() < 2) // 
    {
        //add '0' to seconds until seconds is length 2
        sec = sec.paddedLeft('0', 2);
    }
    return juce::String{ min + ":" + sec };
}

void PlaylistComponent::searchLibrary(juce::String searchText)
{
    DBG("Searching library for: " << searchText);
    if (searchText != "")
    {
        int rowNumber = whereInLibrarys(searchText);
        library.selectRow(rowNumber);
    }
    else
    {
        library.deselectAllRows();
    }
}

int PlaylistComponent::whereInLibrarys(juce::String searchText)
{
    // find track title
    auto it = find_if(librarys.begin(), librarys.end(),
        [&searchText](const Library& obj) {return obj.title.contains(searchText); });
    int i = -1;

    if (it != librarys.end())
    {
        i = std::distance(librarys.begin(), it);
    }

    return i;
}

void PlaylistComponent::saveLibrary()
{
    // make a csv file to save library
    std::ofstream myLibrary("my-library.csv");

    // save library to file 
    for (Library& t : librarys)
    {
        myLibrary << t.file.getFullPathName() << "," << t.length << "\n";
    }
}

void PlaylistComponent::loadLibrary()
{
    // create input stream from saved library
    std::ifstream myLibrary("my-library.csv");
    std::string filePath;
    std::string length;

    // Reading of data
    if (myLibrary.is_open())
    {
        while (getline(myLibrary, filePath, ',')) {
            juce::File file{ filePath };
            Library newLibrary{ file };

            getline(myLibrary, length);
            newLibrary.length = length;
            librarys.push_back(newLibrary);
        }
    }
    myLibrary.close();
}