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

/* Custom combo box, which allows for custom placement of label. 
 * Basically, we just wrap the juce::ComboBox and add a label to it,
 * which is eassier to place than the default label of the combobox.
 */
class CustomComboBox : public juce::Component
{
public:
    CustomComboBox ()
        : customLabel (),
          customComboBox ()
    {
        addAndMakeVisible (customLabel);
        addAndMakeVisible (customComboBox);
    }

    void addItem (const juce::String& text, int itemId)
    {
        customComboBox.addItem (text, itemId);
    }

    void addItem (const juce::String& text, juce::var userData)
    {
        customComboBox.addItem (text, userData);
    }

    void setSelectedId (int id)
    {
        customComboBox.setSelectedId (id, juce::dontSendNotification);
    }

    int getSelectedId () const
    {
        return customComboBox.getSelectedId();
    }

    /* getValue aims to unify the combobox selected value
     * with the value of the parameter. The parameter is 0-indexed,
     * while the combobox is 1-indexed. So we subtract 1 from the
     * selected id to get the value.
     * 
     * Furthermore, the syntax is aligned with getValue of slider  
     * and other components, which is a bit more intuitive.
     *
     * @return int The selected value of the combobox, adjusted for 0-indexing.
    */
    int getValue() const
    {
        return customComboBox.getSelectedId()-1;
    }

    void setValue (int value)
    {
        customComboBox.setSelectedId (value + 1, juce::dontSendNotification);
    }
    
    void setText(const juce::String &newText)
    {
        customLabel.setText (newText, juce::dontSendNotification);
    }

    void setLabelWidth (int newWidth)
    {
        labelWidth = newWidth;
    }

    void setSpacerWidth (int newWidth)
    {
        spacerWidth = newWidth;
    }
    
    void paint (juce::Graphics& g) override
    {
        (void) g; // Suppress unused parameter warning
        // Custom placement of label and combo box
        customLabel.setBounds (getLocalBounds());
        auto customLabelBounds = customLabel.getBounds();
        auto customComboBoxBounds = customLabelBounds;
        customLabelBounds.setWidth(labelWidth);
        customLabel.setBounds (customLabelBounds);
        customComboBoxBounds.setX (customLabelBounds.getX() + labelWidth + spacerWidth);
        customComboBoxBounds.setWidth (getWidth() - labelWidth - spacerWidth);
        customComboBox.setBounds (customComboBoxBounds);
        customComboBox.setBounds (customComboBoxBounds.reduced(5));
    }

private:
    int labelWidth = 75;
    int spacerWidth = 10;
    juce::Label customLabel;
    juce::ComboBox customComboBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomComboBox)
};

/* Custom slider, which allows for custom placement of label. 
 * Same basic idea as the custom combobox, but with a slider. 
 */
class CustomSlider : public juce::Component
{
public:
    CustomSlider ()
        : customLabel (),
          customSlider ()
    {
        addAndMakeVisible (customLabel);
        addAndMakeVisible (customSlider);

        customSlider.setRange (0, 127, 1);
        customSlider.setPopupDisplayEnabled (true, false, this);
        customSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    }

    double getValue()
    {
        return customSlider.getValue();
    }

    void setText(const juce::String &newText)
    {
        customLabel.setText (newText, juce::dontSendNotification);
    }

    void setLabelWidth (int newWidth)
    {
        labelWidth = newWidth;
    }

    void setSpacerWidth (int newWidth)
    {
        spacerWidth = newWidth;
    }

    void setRange (double newMin, double newMax, double newInterval)
    {
        customSlider.setRange (newMin, newMax, newInterval);
    }
    
    void paint (juce::Graphics& g) override
    {
        (void) g; // Suppress unused parameter warning
        // Custom placement of label and combo box
        customLabel.setBounds (getLocalBounds());
        auto customLabelBounds = customLabel.getBounds();
        auto customSliderBounds = customLabelBounds;
        customLabelBounds.setWidth(labelWidth);
        customLabel.setBounds (customLabelBounds);
        customSliderBounds.setX (customLabelBounds.getX() + labelWidth + spacerWidth);
        customSliderBounds.setWidth (getWidth() - labelWidth - spacerWidth);
        customSlider.setBounds (customSliderBounds);
    }

private:
    int labelWidth = 75;
    int spacerWidth = 10;
    juce::Label customLabel;
    juce::Slider customSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomSlider)
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
    void testEnableArp() { arpEnable.setValue(1); }

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
    const int labelWidth = 100;
    static constexpr int numberOfContentItems = 6;
    const int numberOfSpacers = 2;
    static constexpr int numberOfColumns = 4;
    // Bounding boxes for UI elements
    std::array<std::array<juce::Rectangle<int>, numberOfContentItems>, numberOfColumns> contentAreas;
    std::array<std::array<juce::Rectangle<int>, 5>, numberOfColumns> headerFooterAreas;

    // UI Content Elements
    CustomComboBox arpEnable;
    CustomComboBox arpTypeMenu;
    CustomComboBox legatoEnable;
    CustomSlider portamentoSlider;

    CustomComboBox midiClkEnable;
    CustomSlider tempoInDiv;

    CustomComboBox MidiAChannel;
    CustomComboBox MidiACV;
    CustomComboBox MidiAGate;
    CustomSlider MidiAPitchScale;
    CustomSlider MidiAAftertouchScale;
    CustomSlider MidiAVelocityScale;

    CustomComboBox MidiBChannel;
    CustomComboBox MidiBCV;
    CustomComboBox MidiBGate;
    CustomSlider MidiBPitchScale;
    CustomSlider MidiBAftertouchScale;
    CustomSlider MidiBVelocityScale;

    // Footer and Header Elements
    std::array<juce::Label, numberOfColumns> footerHelpLabel1;
    std::array<juce::Label, numberOfColumns> footerHelpLabel2;
    std::array<HorizontalSeparator, numberOfColumns> headerSeparator;
    std::array<HorizontalSeparator, numberOfColumns> footerSeparator;
    std::array<juce::Label, numberOfColumns> headerLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgrammerEditor)
    void timerCallback() override;

    Parameters params;
};
