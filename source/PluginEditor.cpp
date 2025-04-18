#include "PluginEditor.h"
#include "configuration.h"

ProgrammerEditor::ProgrammerEditor (ProgrammerProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    /* MELATONIN UI INSPECTOR */
    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Add ARP ENABLE
    addAndMakeVisible (buttonEnableArp);
    params.addParameter (ENABLE_ARP_NAME, ENABLE_ARP_CC, ENABLE_ARP_VALUE, ENABLE_ARP_MIN_VALUE, ENABLE_ARP_MAX_VALUE);

    // Add ARP TYPE
    addAndMakeVisible (arpTypeMenu);
    arpTypeMenu.addItem ("Normal", 1);
    arpTypeMenu.addItem ("Latch", 2);
    arpTypeMenu.setSelectedId (1);
    params.addParameter (ARP_TYPE_NAME, ARP_TYPE_CC, ARP_TYPE_VALUE, ARP_TYPE_MIN_VALUE, ARP_TYPE_MAX_VALUE);

    // Add LEGATO ENABLE
    addAndMakeVisible (buttonEnableLegato);
    params.addParameter (ENABLE_LEGATO_NAME, ENABLE_LEGATO_CC, ENABLE_LEGATO_VALUE, ENABLE_LEGATO_MIN_VALUE, ENABLE_LEGATO_MAX_VALUE);

    // Add PORTAMENTO SLIDER
    addAndMakeVisible (portamentoSlider);
    portamentoSlider.setRange (0, 127, 1);
    portamentoSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    portamentoSlider.setPopupDisplayEnabled (true, false, this);
    params.addParameter (PORTAMENTO_NAME, PORTAMENTO_CC, PORTAMENTO_VALUE, PORTAMENTO_MIN_VALUE, PORTAMENTO_MAX_VALUE);
    addAndMakeVisible (portamentoLabel);
    portamentoLabel.setText (PORTAMENTO_NAME, juce::dontSendNotification);
    portamentoLabel.attachToComponent (&portamentoSlider, true);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize (400, 300 + inspectButtonHeight);

    // Start timer - this is used to scan the UI for changes
    // NOTE: Currently scanning once per second - this should be faster for a snappy UI.
    // But currently nice for debugging.
    startTimerHz(1);
}

ProgrammerEditor::~ProgrammerEditor()
{
}

void ProgrammerEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);

    // Define spacers for UI
    auto area = getLocalBounds();
    auto headerHeight = 60;
    auto footerHeight = 36;
    auto contentItemHeight = 28;
    auto rightSidebarWidth = 50;
    auto leftSidebarWidth = 75;
    
    // NOTE: Move to resized when adding support for resizing UI
    // Draw the header and footer
    auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    g.drawText (helloWorld, area.removeFromBottom (footerHeight), juce::Justification::centred, false);
    // Draw inspector-button
    inspectButton.setBounds (area.removeFromBottom(inspectButtonHeight));


    auto helpText = juce::String ("Enter Program Pages on 0-Coast in order to use this app.\nPress and hold PGM_A to go to Program Pages.\nHold PGM_B to exit.");
    g.drawMultiLineText (helpText, 0,20, 400 , juce::Justification::centred);
    area.removeFromTop(headerHeight);

    // Draw sidebar spacers
    g.drawText ("", area.removeFromLeft (leftSidebarWidth), juce::Justification::centred, false);
    g.drawText ("", area.removeFromRight (rightSidebarWidth), juce::Justification::centred, false);

    // Draw the content area
    buttonEnableArp.setBounds (area.removeFromTop(contentItemHeight));
    arpTypeMenu.setBounds (area.removeFromTop(contentItemHeight));
    buttonEnableLegato.setBounds (area.removeFromTop(contentItemHeight));
    portamentoSlider.setBounds (area.removeFromTop(contentItemHeight));

}

void ProgrammerEditor::resized()
{
    // At some point, let's enable resizing of the UI and paint stuff here   
}

void ProgrammerEditor::timerCallback()
{
    // Update the parameter value based on the button states
    params.setParameter (ENABLE_ARP_NAME, buttonEnableArp.getToggleState() ? 1 : 0);
    params.setParameter (ARP_TYPE_NAME, arpTypeMenu.getSelectedId() - 1); // -1 because ComboBox is 1-indexed
    params.setParameter (ENABLE_LEGATO_NAME, buttonEnableLegato.getToggleState() ? 1 : 0);
    params.setParameter (PORTAMENTO_NAME, (int)portamentoSlider.getValue());
    
    // Iterate over parameters and check if they are updated
    auto allParams = params.getAllParameters();
    for (const auto& param : allParams)
    {
        auto paramName = param.first;
        if (params.isParameterUpdated(paramName) == true)
        {
            auto paramValue = param.second.value;
            juce::Logger::outputDebugString (paramName + " updated to " + std::to_string (paramValue));

            // send a message to the processor
            // In this particular case, we're sending CC messages
            // value1: Midi Channel
            // value2: CC #
            // value3: CC value
            GuiMessage message;
            message.type = GuiMessage::cc;
            message.value1 = MIDI_CHANNEL;
            message.value2 = param.second.cc;
            message.value3 = param.second.value;
            if (! processorRef.messageQueue->push(message))
            {
                juce::Logger::outputDebugString ("Message queue is full!");
                // Handle queue full scenario (e.g., drop oldest, indicate error)
            }
        }
    }
}