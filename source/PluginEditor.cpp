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

    // Add header and footer
    addAndMakeVisible(headerLabel);
    headerLabel.setText ("Play Modes", juce::dontSendNotification);
    headerLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    headerLabel.setJustificationType (juce::Justification::bottomLeft);
    addAndMakeVisible(headerSeparator);
    addAndMakeVisible(footerSeparator);
    addAndMakeVisible(footerHelpLabel1);
    addAndMakeVisible(footerHelpLabel2);
    footerHelpLabel1.setText ("Press and hold PGM_A to go to Program Pages. Hold PGM_B to exit.", juce::dontSendNotification);
    footerHelpLabel2.setText ("Ensure MIDI Output is set to interface connected to 0-Coast", juce::dontSendNotification);
    footerHelpLabel1.setJustificationType (juce::Justification::left);
    footerHelpLabel2.setJustificationType (juce::Justification::left);
    footerHelpLabel1.setFont (juce::Font (11.0f, juce::Font::plain));
    footerHelpLabel2.setFont (juce::Font (11.0f, juce::Font::plain));

    // Add ARP ENABLE
    addAndMakeVisible (buttonEnableArp);
    params.addParameter (ENABLE_ARP_NAME, ENABLE_ARP_CC, ENABLE_ARP_VALUE, ENABLE_ARP_MIN_VALUE, ENABLE_ARP_MAX_VALUE);

    // Add ARP TYPE
    addAndMakeVisible (arpTypeLabel);
    arpTypeLabel.setText ("Arp Mode", juce::dontSendNotification);
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
    
    // Calculate the size of the UI
    auto height = headerHeight + contentItemHeight*numberOfContentItems + numberOfSpacers*contentItemHeight/2;
    if (enableInspector == true)
    {
        height += inspectButtonHeight + footerHeight;
    }
    else
    {
        height += headerHeight;
    }
    // Set the size of the UI
    auto width = 400 * numberOfColumns;
    setSize (width, height);

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
    
    // NOTE: Move to resized when adding support for resizing UI
    // Draw the header and footer
    if (enableInspector == true)
    {
        auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
        g.drawText (helloWorld, area.removeFromBottom (footerHeight), juce::Justification::centred, false);
        // Draw inspector-button
        inspectButton.setBounds (area.removeFromBottom(inspectButtonHeight));
    }
    else
    {
        g.drawText ("", area.removeFromBottom (headerHeight), juce::Justification::centred, false);
    }


    // Draw sidebar spacers
    g.drawText ("", area.removeFromLeft (leftSidebarWidth), juce::Justification::centred, false);
    g.drawText ("", area.removeFromRight (rightSidebarWidth), juce::Justification::centred, false);


    // Draw the header    
    headerLabel.setBounds( area.removeFromTop (headerHeight)) ;
    headerSeparator.setBounds (area.removeFromTop (contentItemHeight/2));
    //area.removeFromTop(headerHeight);

        
    // Draw the footer - This will be a help text and a separator
    footerHelpLabel1.setBounds (area.removeFromBottom (contentItemHeight/2));
    footerHelpLabel2.setBounds (area.removeFromBottom (contentItemHeight/2));
    footerSeparator.setBounds (area.removeFromBottom (contentItemHeight/2));
    
    // Draw the content area
    buttonEnableArp.setBounds (area.removeFromTop(contentItemHeight));

    // Doing the tricky label + comboBox positioning here
    arpTypeLabel.setBounds (area.removeFromTop(contentItemHeight));
    auto arpTypeLabelBounds = arpTypeLabel.getBounds();
    auto arpTypeMenuBounds = arpTypeLabelBounds;
    arpTypeLabelBounds.setWidth(labelWidth);
    arpTypeLabel.setBounds (arpTypeLabelBounds);
    arpTypeMenuBounds.setX (arpTypeLabelBounds.getX() + labelWidth + spacerWidth);
    arpTypeMenuBounds.setWidth (area.getWidth() - labelWidth - spacerWidth);
    arpTypeMenu.setBounds (arpTypeMenuBounds);
    arpTypeMenu.setBounds (arpTypeMenuBounds.reduced(5));
    // NOTE: This is a bit of a hack - we should really use a custom component for the comboBox
    
    buttonEnableLegato.setBounds (area.removeFromTop(contentItemHeight));
    
    // The slider and label are a bit tricky to position - we should fix this
    // using a custom slider + label component
    portamentoLabel.setBounds (area.removeFromTop(contentItemHeight));
    auto labelBounds = portamentoLabel.getBounds();
    auto sliderBounds = labelBounds;
    labelBounds.setWidth(labelWidth);
    portamentoLabel.setBounds (labelBounds);
    sliderBounds.setX (labelBounds.getX() + labelWidth + spacerWidth);
    sliderBounds.setWidth (area.getWidth() - labelWidth - spacerWidth);
    portamentoSlider.setBounds (sliderBounds);


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