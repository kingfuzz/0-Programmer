#include "PluginEditor.h"

// TODO: Move all configuration (incl. controllable parameters) to file.
#define MIDI_CHANNEL 1

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

    // Add arp button and parameter
    addAndMakeVisible (buttonEnableArp);
    params.addParameter ("EnableArp", 117, 0, 0, 1);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

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
    auto headerFooterHeight = 36;
    auto contentItemHeight = 24;
    auto sidebarWidth = 50;
    
    // NOTE: Move to resized when adding support for resizing UI
    // Draw the header and footer
    auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    g.drawText (helloWorld, area.removeFromBottom (headerFooterHeight), juce::Justification::centred, false);

    auto helpText = juce::String ("Enter Program Pages on 0-Coast in order to use this app.\nPress and hold PGM_A to go to Program Pages. Hold PGM_B to exit.");
    g.drawText (helpText, area.removeFromTop(headerFooterHeight) , juce::Justification::centred, false);

    // Draw sidebar spacers
    g.drawText ("", area.removeFromLeft (sidebarWidth), juce::Justification::centred, false);
    g.drawText ("", area.removeFromRight (sidebarWidth), juce::Justification::centred, false);

    // Draw the content area
    buttonEnableArp.setBounds (area.removeFromTop(contentItemHeight));
}

void ProgrammerEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    area.removeFromBottom(50);
    inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre(100, 50));
}

void ProgrammerEditor::timerCallback()
{
    // Update the parameter value based on the button states
    params.setParameter ("EnableArp", buttonEnableArp.getToggleState() ? 1 : 0);
    
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