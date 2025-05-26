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
    auto height = headerHeight + contentItemHeight*numberOfContentItems + numberOfSpacers*separatorHeight;
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
    footerHelpLabel1[0].setText ("Press and hold PGM_A to go to Program Pages. Hold PGM_B to exit.", juce::dontSendNotification);
    footerHelpLabel2[0].setText ("Ensure MIDI Output is set to interface connected to 0-Coast", juce::dontSendNotification);
    
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

    // Example of how to add additional columns of content items
    if (numberOfColumns > 1)
    {
        for (size_t col = 1; col < static_cast<size_t>(numberOfColumns); ++col)
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