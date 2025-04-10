#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <PluginEditor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("one is equal to one", "[dummy]")
{
    REQUIRE (1 == 1);
}

TEST_CASE ("Plugin instance", "[instance]")
{
    ProgrammerProcessor testPlugin;

    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("0-Programmer"));
    }
}

#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2022.0.0 (r0x131e93b0)"));
}
#endif

TEST_CASE("Processor/Editor End2End Test", "[Send ControllerChange on button press]")
{
    ProgrammerProcessor testPlugin;
    ProgrammerEditor testPluginEditor(testPlugin);
    // -- Setup of properties and buffers
    // Properties
    double sampleRate   = 48000;
    int samplesPerBlock = 512;
    int numChannels     = 2;
    // Set IO buffers
    juce::AudioBuffer<float> myBuffer;
    myBuffer.setSize(numChannels, samplesPerBlock);
    myBuffer.clear();
    juce::MidiBuffer myMidiBuffer;
    myMidiBuffer.clear();

    // Check that buffer is empty
    CHECK( myMidiBuffer.isEmpty() == true );

    // Do initialization
    testPlugin.prepareToPlay (sampleRate, samplesPerBlock);

    // -- Test 1 --
    // Run a block
    testPlugin.processBlock (myBuffer, myMidiBuffer);

    // Midi Buffer is empty and theres no event
    CHECK( myMidiBuffer.isEmpty() == true );
    CHECK( myMidiBuffer.getNumEvents() == 0);
  
    // Clear buffers for next subtest
    myBuffer.clear();
    myMidiBuffer.clear();
  
    // -- Test 2 --
    // Simulate button press
    juce::TextButton myButton;
    testPluginEditor.buttonEnableArp.setToggleState (true, juce::NotificationType::dontSendNotification);
    // Wait a bit (1sec) so timer can trigger. NOTE: Doesn't work - Fire callback manually instead?
    //juce::Thread::sleep(2000);
    testPluginEditor.testTimerCallback();
    
    // Run another block
    testPlugin.processBlock (myBuffer, myMidiBuffer);
  
    // Midi Buffer is NOT empty and theres one event
    CHECK( myMidiBuffer.isEmpty() == false );
    CHECK( myMidiBuffer.getNumEvents() == 1);

    // Iterate over midibuffer. Check that:
    // - Theres a message 
    // - It's a CC
    // - It's on channel 1 
    // - CC is #117 and value is 1
    for (const auto metadata : myMidiBuffer ) {
      auto midiMessage = metadata.getMessage();
      CHECK( midiMessage.isController() == true );
      CHECK( midiMessage.getChannel() == 1 );
      CHECK( midiMessage.getControllerNumber() == 117 );
      CHECK( midiMessage.getControllerValue() == 1 );
    }
  
    // Clear buffers for next subtest
    myBuffer.clear();
    myMidiBuffer.clear();

    // -- Test 3 --
    // Run another block
    testPlugin.processBlock (myBuffer, myMidiBuffer);
    // Midi Buffer is empty and theres no event
    CHECK( myMidiBuffer.isEmpty() == true );
    CHECK( myMidiBuffer.getNumEvents() == 0);

}
