#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include "Parameters.h"

//==============================================================================
// CUSTOM UI ELEMENTS

class HorizontalSeparator : public juce::Component
{
public:
    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colour (0xFF42A2C8));
        g.drawLine (0.0f, getHeight() / 2.0f, (float)getWidth(), getHeight() / 2.0f, 1.0f);
    }
};


//==============================================================================
// Editor class
class ProgrammerEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit ProgrammerEditor (ProgrammerProcessor&);
    ~ProgrammerEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // Enable melatonin inspector here - will only be enabled in
    // debug builds
    bool enableInspector = false;
    
    // Test interface for callback - this is not nice, figure out how to 
    // get timer to fire in test
    void testTimerCallback() { timerCallback(); }
    juce::ToggleButton buttonEnableArp { "Enable Arpeggiator" };

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ProgrammerProcessor& processorRef;
    
    // Melatonin Inspector Stuff
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    int inspectButtonHeight = 50;

    // UI Layout values
    const int columnWidth  = 400;
    const int headerHeight = 36;
    const int footerHeight = 36;
    const int contentItemHeight = 32;
    const int separatorHeight = contentItemHeight / 2;
    const int rightSidebarWidth = 50;
    const int leftSidebarWidth = 50;
    const int contentWidth = columnWidth - leftSidebarWidth - rightSidebarWidth;
    const int labelWidth = 75;
    const int spacerWidth = 10;
    static constexpr int numberOfContentItems = 6;
    const int numberOfSpacers = 2;
    static constexpr int numberOfColumns = 1;
    std::array<juce::Rectangle<int>, numberOfContentItems> contentAreas;

    // UI Content Elements
    juce::Label arpTypeLabel;
    juce::ComboBox arpTypeMenu;
    juce::ToggleButton buttonEnableLegato { "Enable Legato" };
    juce::Slider portamentoSlider;
    juce::Label portamentoLabel;

    // Footer and Header Elements
    juce::Label footerHelpLabel1;
    juce::Label footerHelpLabel2;
    std::array<HorizontalSeparator, numberOfColumns> headerSeparator;
    std::array<HorizontalSeparator, numberOfColumns> footerSeparator;
    std::array<juce::Label, numberOfColumns> headerLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgrammerEditor)
    void timerCallback() override;

    Parameters params;
};
