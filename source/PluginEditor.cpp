#include "PluginEditor.h"
#include "configuration.h"

ProgrammerEditor::ProgrammerEditor (ProgrammerProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
#ifdef NDEBUG
    // If we're in release mode, disable the inspector
    enableInspector = false;
#else
    juce::Logger::outputDebugString (">>> ProgrammerEditor: Started in DEBUG mode");
#endif

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

    // Calculate the size of the UI
    // Height is header + content items + spacers + help text (1xcontent) + footer
    auto height = headerHeight + contentItemHeight*numberOfContentItems + numberOfSpacers*separatorHeight + contentItemHeight;;
    if (enableInspector == true)
    {
        height += inspectButtonHeight + footerHeight;
    }
    else
    {
        height += headerHeight;
    }
    auto width = columnWidth + ((numberOfColumns-1) * (contentWidth + rightSidebarWidth));
    setSize (width, height);

    // Start timer - this is used to scan the UI for changes
    // NOTE: Currently scanning once per second - this should be faster for a snappy UI.
    // But currently nice for debugging.
    startTimerHz(1);

    // Add headers and footers for each column
    for (size_t col = 0; col < static_cast<size_t>(numberOfColumns); ++col)
    {
        addAndMakeVisible(headerLabel[col]);
        headerLabel[col].setText ("Col Header", juce::dontSendNotification);
        headerLabel[col].setFont (juce::FontOptions (16.0f, juce::Font::bold));
        headerLabel[col].setJustificationType (juce::Justification::bottomLeft);
        addAndMakeVisible(headerSeparator[col]);
        addAndMakeVisible(footerSeparator[col]);
        addAndMakeVisible(footerHelpLabel1[col]);
        addAndMakeVisible(footerHelpLabel2[col]);
        footerHelpLabel1[col].setJustificationType (juce::Justification::left);
        footerHelpLabel2[col].setJustificationType (juce::Justification::left);
        footerHelpLabel1[col].setFont (juce::FontOptions (11.0f, juce::Font::plain));
        footerHelpLabel2[col].setFont (juce::FontOptions (11.0f, juce::Font::plain));
    }
    
    // Add header and footer text
    headerLabel[0].setText ("Play Modes", juce::dontSendNotification);
    footerHelpLabel1[0].setText ("Press and hold PGM_A to enter Program Pages. Hold PGM_B to exit.", juce::dontSendNotification);
    footerHelpLabel2[0].setText ("Ensure MIDI Output is set to interface connected to 0-Coast", juce::dontSendNotification);
    headerLabel[1].setText ("Clocks", juce::dontSendNotification);
    headerLabel[2].setText ("MIDI A", juce::dontSendNotification);
    headerLabel[3].setText ("MIDI B", juce::dontSendNotification);

    // -- Add column 0 content items --
    // Add ARP ENABLE
    addAndMakeVisible (arpEnable);
    arpEnable.setText ("Arpegiator");
    arpEnable.addItem ("Off", 1);
    arpEnable.addItem ("On", 2);
    arpEnable.setSelectedId (1);
    arpEnable.setLabelWidth (labelWidth);
    params.addParameter (ENABLE_ARP_NAME, ENABLE_ARP_CC, ENABLE_ARP_VALUE, ENABLE_ARP_MIN_VALUE, ENABLE_ARP_MAX_VALUE);

    // Add ARP TYPE
    addAndMakeVisible (arpTypeMenu);
    arpTypeMenu.setText ("Arp Mode");
    arpTypeMenu.addItem ("Normal", 1);
    arpTypeMenu.addItem ("Latch", 2);
    arpTypeMenu.setSelectedId (1);
    arpTypeMenu.setLabelWidth (labelWidth);
    params.addParameter (ARP_TYPE_NAME, ARP_TYPE_CC, ARP_TYPE_VALUE, ARP_TYPE_MIN_VALUE, ARP_TYPE_MAX_VALUE);

    // Add LEGATO ENABLE
    addAndMakeVisible (legatoEnable);
    legatoEnable.setText ("Legato");
    legatoEnable.addItem ("Off", 1);
    legatoEnable.addItem ("On", 2);
    legatoEnable.setSelectedId (1);
    legatoEnable.setLabelWidth (labelWidth);
    params.addParameter (ENABLE_LEGATO_NAME, ENABLE_LEGATO_CC, ENABLE_LEGATO_VALUE, ENABLE_LEGATO_MIN_VALUE, ENABLE_LEGATO_MAX_VALUE);

    // Add PORTAMENTO SLIDER
    addAndMakeVisible (portamentoSlider);
    portamentoSlider.setText (PORTAMENTO_NAME);
    portamentoSlider.setLabelWidth (labelWidth);
    params.addParameter (PORTAMENTO_NAME, PORTAMENTO_CC, PORTAMENTO_VALUE, PORTAMENTO_MIN_VALUE, PORTAMENTO_MAX_VALUE);

    // -- Add column 1 content items --
    // Add MIDI Clock Enable
    addAndMakeVisible (midiClkEnable);
    midiClkEnable.setText ("MIDI Clock");
    midiClkEnable.addItem ("Off", 1);
    midiClkEnable.addItem ("On", 2);
    midiClkEnable.setSelectedId (1);
    midiClkEnable.setLabelWidth (labelWidth);
    params.addParameter (ENABLE_MIDI_CLK_NAME, ENABLE_MIDI_CLK_CC, ENABLE_MIDI_CLK_VALUE, ENABLE_MIDI_CLK_MIN_VALUE, ENABLE_MIDI_CLK_MAX_VALUE);

    // Add Tempo in Divisions
    addAndMakeVisible (tempoInDiv);
    tempoInDiv.setText ("Tempo In Div");
    tempoInDiv.setRange (TEMPO_IN_DIV_MIN_VALUE, TEMPO_IN_DIV_MAX_VALUE, 1);
    tempoInDiv.setLabelWidth (labelWidth);
    params.addParameter (TEMPO_IN_DIV_NAME, TEMPO_IN_DIV_CC, TEMPO_IN_DIV_VALUE, TEMPO_IN_DIV_MIN_VALUE, TEMPO_IN_DIV_MAX_VALUE);

    // -- Add column 2 content items --
    // Add MIDI A Channel
    addAndMakeVisible (MidiAChannel);
    MidiAChannel.setText ("Channel");
    for (int i = 1; i <= 16; ++i)
    {
        MidiAChannel.addItem (juce::String (i), i);
    }
    MidiAChannel.addItem ("All", 17);
    MidiAChannel.setSelectedId (1);
    MidiAChannel.setLabelWidth (labelWidth);
    params.addParameter (MIDI_A_CHANNEL_NAME, MIDI_A_CHANNEL_CC, MIDI_A_CHANNEL_VALUE, MIDI_A_CHANNEL_MIN_VALUE, MIDI_A_CHANNEL_MAX_VALUE);
    // Add MIDI A CV
    addAndMakeVisible (MidiACV);
    MidiACV.setText ("CV");
    MidiACV.addItem ("Note", 1);
    MidiACV.addItem ("Velocity", 2);
    MidiACV.addItem ("Mod Wheel", 3);
    MidiACV.addItem ("LFO", 4);
    MidiACV.setSelectedId (1);
    MidiACV.setLabelWidth (labelWidth);
    params.addParameter (MIDI_A_CV_NAME, MIDI_A_CV_CC, MIDI_A_CV_VALUE, MIDI_A_CV_MIN_VALUE, MIDI_A_CV_MAX_VALUE);
    // Add MIDI A Gate
    addAndMakeVisible (MidiAGate);
    MidiAGate.setText ("Gate");
    MidiAGate.addItem ("Note", 1);
    MidiAGate.addItem ("Velocity", 2);
    MidiAGate.addItem ("Mod Wheel", 3);
    MidiAGate.addItem ("LFO", 4);
    MidiAGate.setSelectedId (1);
    MidiAGate.setLabelWidth (labelWidth);
    params.addParameter (MIDI_A_GATE_NAME, MIDI_A_GATE_CC, MIDI_A_GATE_VALUE, MIDI_A_GATE_MIN_VALUE, MIDI_A_GATE_MAX_VALUE);
    // Add MIDI A Pitch Scale
    addAndMakeVisible (MidiAPitchScale);
    MidiAPitchScale.setText ("Pitchbend Scale");
    MidiAPitchScale.setRange (MIDI_A_PITCH_MIN_VALUE, MIDI_A_PITCH_MAX_VALUE, 1);
    MidiAPitchScale.setLabelWidth (labelWidth);
    params.addParameter (MIDI_A_PITCH_NAME, MIDI_A_PITCH_CC, MIDI_A_PITCH_VALUE, MIDI_A_PITCH_MIN_VALUE, MIDI_A_PITCH_MAX_VALUE);
    // Add MIDI A Aftertouch Scale
    addAndMakeVisible (MidiAAftertouchScale);
    MidiAAftertouchScale.setText ("Aftertouch Scale");
    MidiAAftertouchScale.setRange (MIDI_A_AFTERTOUCH_MIN_VALUE, MIDI_A_AFTERTOUCH_MAX_VALUE, 1);
    MidiAAftertouchScale.setLabelWidth (labelWidth);
    params.addParameter (MIDI_A_AFTERTOUCH_NAME, MIDI_A_AFTERTOUCH_CC, MIDI_A_AFTERTOUCH_VALUE, MIDI_A_AFTERTOUCH_MIN_VALUE, MIDI_A_AFTERTOUCH_MAX_VALUE);
    // Add MIDI A Velocity Scale
    addAndMakeVisible (MidiAVelocityScale);
    MidiAVelocityScale.setText ("Velocity Scale");
    MidiAVelocityScale.setRange (MIDI_A_VELOCITY_MIN_VALUE, MIDI_A_VELOCITY_MAX_VALUE, 1);
    MidiAVelocityScale.setLabelWidth (labelWidth);
    params.addParameter (MIDI_A_VELOCITY_NAME, MIDI_A_VELOCITY_CC, MIDI_A_VELOCITY_VALUE, MIDI_A_VELOCITY_MIN_VALUE, MIDI_A_VELOCITY_MAX_VALUE);

    // -- Add column 3 content items --
    // Add MIDI B Channel
    addAndMakeVisible (MidiBChannel);
    MidiBChannel.setText ("Channel");
    for (int i = 1; i <= 16; ++i)
    {
        MidiBChannel.addItem (juce::String (i), i);
    }
    MidiBChannel.addItem ("All", 17);
    MidiBChannel.setSelectedId (1);
    MidiBChannel.setLabelWidth (labelWidth);
    params.addParameter (MIDI_B_CHANNEL_NAME, MIDI_B_CHANNEL_CC, MIDI_B_CHANNEL_VALUE, MIDI_B_CHANNEL_MIN_VALUE, MIDI_B_CHANNEL_MAX_VALUE);
    // Add MIDI B CV
    addAndMakeVisible (MidiBCV);
    MidiBCV.setText ("CV");
    MidiBCV.addItem ("Note", 1);
    MidiBCV.addItem ("Velocity", 2);
    MidiBCV.addItem ("Mod Wheel", 3);
    MidiBCV.addItem ("LFO", 4);
    MidiBCV.setSelectedId (1);
    MidiBCV.setLabelWidth (labelWidth);
    params.addParameter (MIDI_B_CV_NAME, MIDI_B_CV_CC, MIDI_B_CV_VALUE, MIDI_B_CV_MIN_VALUE, MIDI_B_CV_MAX_VALUE);
    // Add MIDI B Gate
    addAndMakeVisible (MidiBGate);
    MidiBGate.setText ("Gate");
    MidiBGate.addItem ("Note", 1);
    MidiBGate.addItem ("Velocity", 2);
    MidiBGate.addItem ("Mod Wheel", 3);
    MidiBGate.addItem ("LFO", 4);
    MidiBGate.setSelectedId (1);
    MidiBGate.setLabelWidth (labelWidth);
    params.addParameter (MIDI_B_GATE_NAME, MIDI_B_GATE_CC, MIDI_B_GATE_VALUE, MIDI_B_GATE_MIN_VALUE, MIDI_B_GATE_MAX_VALUE);
    // Add MIDI B Pitch Scale
    addAndMakeVisible (MidiBPitchScale);
    MidiBPitchScale.setText ("Pitchbend Scale");
    MidiBPitchScale.setRange (MIDI_B_PITCH_MIN_VALUE, MIDI_B_PITCH_MAX_VALUE, 1);
    MidiBPitchScale.setLabelWidth (labelWidth);
    params.addParameter (MIDI_B_PITCH_NAME, MIDI_B_PITCH_CC, MIDI_B_PITCH_VALUE, MIDI_B_PITCH_MIN_VALUE, MIDI_B_PITCH_MAX_VALUE);
    // Add MIDI B Aftertouch Scale
    addAndMakeVisible (MidiBAftertouchScale);
    MidiBAftertouchScale.setText ("Aftertouch Scale");
    MidiBAftertouchScale.setRange (MIDI_B_AFTERTOUCH_MIN_VALUE, MIDI_B_AFTERTOUCH_MAX_VALUE, 1);
    MidiBAftertouchScale.setLabelWidth (labelWidth);
    params.addParameter (MIDI_B_AFTERTOUCH_NAME, MIDI_B_AFTERTOUCH_CC, MIDI_B_AFTERTOUCH_VALUE, MIDI_B_AFTERTOUCH_MIN_VALUE, MIDI_B_AFTERTOUCH_MAX_VALUE);
    // Add MIDI B Velocity Scale
    addAndMakeVisible (MidiBVelocityScale);
    MidiBVelocityScale.setText ("Velocity Scale");
    MidiBVelocityScale.setRange (MIDI_B_VELOCITY_MIN_VALUE, MIDI_B_VELOCITY_MAX_VALUE, 1);
    MidiBVelocityScale.setLabelWidth (labelWidth);
    params.addParameter (MIDI_B_VELOCITY_NAME, MIDI_B_VELOCITY_CC, MIDI_B_VELOCITY_VALUE, MIDI_B_VELOCITY_MIN_VALUE, MIDI_B_VELOCITY_MAX_VALUE);
}

ProgrammerEditor::~ProgrammerEditor()
{
}

void ProgrammerEditor::paint (juce::Graphics& g)
{
    // NOTE: Move much of this to resized when adding support for resizing UI

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    // Set coulour and font for debug text and random labels
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);

    // Define area for UI
    auto area = getLocalBounds();
    
    // Add debug label if needed
    if (enableInspector == true)
    {
        auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
        g.drawText (helloWorld, area.removeFromBottom (footerHeight), juce::Justification::centred, false);
    }

    // Draw sidebar spacers
    g.drawText ("", area.removeFromLeft (leftSidebarWidth), juce::Justification::centred, false);
    g.drawText ("", area.removeFromRight (rightSidebarWidth + ((numberOfColumns-1)*(contentWidth+rightSidebarWidth))), juce::Justification::centred, false);

    // Draw the footer, inclunding the inspector button if enabled
    if (enableInspector == true)
    {      
        // Draw inspector-button
        inspectButton.setBounds (area.removeFromBottom(inspectButtonHeight));
    }
    else
    {
        g.drawText ("", area.removeFromBottom (headerHeight), juce::Justification::centred, false);
    }

    // -- Calculate bounding boxes for UI elements --
    // We'll calculate bounding boxes for each of the UI elements (content, headers, etc)
    // making it simpler to place them in the UI. Potentially, this could be used
    // to allow resizing of the UI, but for now, we'll just use it to position the elements.
    
    // Calculate the header and footer areas
    headerFooterAreas[0][0] = area.removeFromTop (headerHeight);
    headerFooterAreas[0][1] = area.removeFromTop (separatorHeight);
    headerFooterAreas[0][2] = area.removeFromBottom (contentItemHeight/2);
    headerFooterAreas[0][3] = area.removeFromBottom (contentItemHeight/2);
    headerFooterAreas[0][4] = area.removeFromBottom (separatorHeight);
    // For each remaining column, calculate the remaining header and footer areas
    for (size_t col = 1; col < static_cast<size_t>(numberOfColumns); ++col)
    {
        for (size_t i = 0; i < static_cast<size_t>(5); ++i)
        {
            headerFooterAreas[col][i] = headerFooterAreas[0][i];
            headerFooterAreas[col][i].setX (headerFooterAreas[0][i].getX() + ((int)col * (contentWidth + rightSidebarWidth)));
        }
    }

    // Calculate the content area
    for (size_t i = 0; i < static_cast<size_t>(numberOfContentItems); ++i)
    {
        contentAreas[0][i] = area.removeFromTop (contentItemHeight);
    }
    // For each remaining column, calculate the content areas
    for (size_t col = 1; col < static_cast<size_t>(numberOfColumns); ++col)
    {
        for (size_t i = 0; i < static_cast<size_t>(numberOfContentItems); ++i)
        {
            contentAreas[col][i] = contentAreas[0][i];
            contentAreas[col][i].setX (contentAreas[col][i].getX() + ((int)col * (contentWidth + rightSidebarWidth)));
        }
    }

    // -- Draw the UI elements --
    // Draw headers and footers for all columns
    for (size_t col = 0; col < static_cast<size_t>(numberOfColumns); ++col)
    {
        headerLabel[col].setBounds(headerFooterAreas[col][0]);
        headerSeparator[col].setBounds(headerFooterAreas[col][1]);
        footerHelpLabel1[col].setBounds(headerFooterAreas[col][2]);
        footerHelpLabel2[col].setBounds(headerFooterAreas[col][3]);
        footerSeparator[col].setBounds(headerFooterAreas[col][4]);
    }

    // Draw content items for column 0
    arpEnable.setBounds (contentAreas[0][0]);
    arpTypeMenu.setBounds (contentAreas[0][1]);
    legatoEnable.setBounds (contentAreas[0][2]);
    portamentoSlider.setBounds (contentAreas[0][3]);

    // Draw content items for column 1
    midiClkEnable.setBounds (contentAreas[1][0]);
    tempoInDiv.setBounds (contentAreas[1][1]);

    // Draw content items for column 2
    MidiAChannel.setBounds (contentAreas[2][0]);
    MidiACV.setBounds (contentAreas[2][1]);
    MidiAGate.setBounds (contentAreas[2][2]);
    MidiAPitchScale.setBounds (contentAreas[2][3]);
    MidiAAftertouchScale.setBounds (contentAreas[2][4]);
    MidiAVelocityScale.setBounds (contentAreas[2][5]);
    
    // Draw content items for column 3
    MidiBChannel.setBounds (contentAreas[3][0]);
    MidiBCV.setBounds (contentAreas[3][1]);
    MidiBGate.setBounds (contentAreas[3][2]);
    MidiBPitchScale.setBounds (contentAreas[3][3]);
    MidiBAftertouchScale.setBounds (contentAreas[3][4]);
    MidiBVelocityScale.setBounds (contentAreas[3][5]);

    // Example of how to add additional columns of content items
    if (numberOfColumns > 4)
    {
        for (size_t col = 4; col < static_cast<size_t>(numberOfColumns); ++col)
        {

            g.drawText ("Content 1", contentAreas[col][0], juce::Justification::left, false);
            g.drawText ("Content 2", contentAreas[col][1], juce::Justification::left, false);
            g.drawText ("Content 3", contentAreas[col][2], juce::Justification::left, false);
        }
    }

}

void ProgrammerEditor::resized()
{
    // At some point, let's enable resizing of the UI and paint stuff here   
}

void ProgrammerEditor::timerCallback()
{
    // Update the parameter value based on the button states
    params.setParameter (ENABLE_ARP_NAME, arpEnable.getValue());
    params.setParameter (ARP_TYPE_NAME, arpTypeMenu.getValue());
    params.setParameter (ENABLE_LEGATO_NAME, legatoEnable.getValue());
    params.setParameter (PORTAMENTO_NAME, (int)portamentoSlider.getValue());

    params.setParameter (ENABLE_MIDI_CLK_NAME, midiClkEnable.getValue());
    params.setParameter (TEMPO_IN_DIV_NAME, (int)tempoInDiv.getValue());

    params.setParameter (MIDI_A_CHANNEL_NAME, MidiAChannel.getValue());
    params.setParameter (MIDI_A_CV_NAME, MidiACV.getValue());
    params.setParameter (MIDI_A_GATE_NAME, MidiAGate.getValue());
    params.setParameter (MIDI_A_PITCH_NAME, (int)MidiAPitchScale.getValue());
    params.setParameter (MIDI_A_AFTERTOUCH_NAME, (int)MidiAAftertouchScale.getValue());
    params.setParameter (MIDI_A_VELOCITY_NAME, (int)MidiAVelocityScale.getValue());
    
    params.setParameter (MIDI_B_CHANNEL_NAME, MidiBChannel.getValue());
    params.setParameter (MIDI_B_CV_NAME, MidiBCV.getValue());
    params.setParameter (MIDI_B_GATE_NAME, MidiBGate.getValue());
    params.setParameter (MIDI_B_PITCH_NAME, (int)MidiBPitchScale.getValue());
    params.setParameter (MIDI_B_AFTERTOUCH_NAME, (int)MidiBAftertouchScale.getValue());
    params.setParameter (MIDI_B_VELOCITY_NAME, (int)MidiBVelocityScale.getValue());
    
    params.setParameter (ENABLE_MIDI_CLK_NAME, midiClkEnable.getValue());
    params.setParameter (TEMPO_IN_DIV_NAME, (int)tempoInDiv.getValue());

    params.setParameter (MIDI_A_CHANNEL_NAME, MidiAChannel.getValue());
    params.setParameter (MIDI_A_CV_NAME, MidiACV.getValue());
    params.setParameter (MIDI_A_GATE_NAME, MidiAGate.getValue());
    params.setParameter (MIDI_A_PITCH_NAME, (int)MidiAPitchScale.getValue());
    params.setParameter (MIDI_A_AFTERTOUCH_NAME, (int)MidiAAftertouchScale.getValue());
    params.setParameter (MIDI_A_VELOCITY_NAME, (int)MidiAVelocityScale.getValue());
    
    params.setParameter (MIDI_B_CHANNEL_NAME, MidiBChannel.getValue());
    params.setParameter (MIDI_B_CV_NAME, MidiBCV.getValue());
    params.setParameter (MIDI_B_GATE_NAME, MidiBGate.getValue());
    params.setParameter (MIDI_B_PITCH_NAME, (int)MidiBPitchScale.getValue());
    params.setParameter (MIDI_B_AFTERTOUCH_NAME, (int)MidiBAftertouchScale.getValue());
    params.setParameter (MIDI_B_VELOCITY_NAME, (int)MidiBVelocityScale.getValue());
    
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