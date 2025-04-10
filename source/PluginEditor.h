#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include "Parameters.h"

//==============================================================================
class ProgrammerEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit ProgrammerEditor (ProgrammerProcessor&);
    ~ProgrammerEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::ToggleButton buttonEnableArp { "Enable Arpeggiator" };
    juce::Slider portamentoSlider;
    juce::Label portamentoLabel;

    // Test interface for callback - this is not nice, figure out how to 
    // get timer to fire in test
    void testTimerCallback() { timerCallback(); }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ProgrammerProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    int inspectButtonHeight = 50;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgrammerEditor)
    
    void timerCallback() override;

    Parameters params;
};
