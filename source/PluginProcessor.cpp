#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProgrammerProcessor::ProgrammerProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    messageQueue.reset(new ThreadSafeMessageQueue(128)); // Example capacity (number of messages)
}

ProgrammerProcessor::~ProgrammerProcessor()
{
}

//==============================================================================
const juce::String ProgrammerProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProgrammerProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProgrammerProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProgrammerProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProgrammerProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProgrammerProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProgrammerProcessor::getCurrentProgram()
{
    return 0;
}

void ProgrammerProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String ProgrammerProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void ProgrammerProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void ProgrammerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void ProgrammerProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool ProgrammerProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void ProgrammerProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // We'll clear all output channels, as we are not doing any audio
    for (auto i = 0; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());



    if (messageQueue->getNumReady() > 0)
    {
        juce::Logger::outputDebugString ("GOT MESSAGE!");
        GuiMessage message;
        messageQueue->pop(message);
        juce::Logger::outputDebugString ("Message type = " + std::to_string (message.type));
        juce::Logger::outputDebugString ("Value1 = " + std::to_string (message.value1));
        juce::Logger::outputDebugString ("Value2 = " + std::to_string (message.value2));
        juce::Logger::outputDebugString ("Value3 = " + std::to_string (message.value3));
        juce::Logger::outputDebugString ("Messages still in Queue: " + std::to_string (messageQueue->getNumReady()));

        // Add message to midi buffer
        auto myMidiMessage = juce::MidiMessage::controllerEvent(message.value1, message.value2, message.value3);
        midiMessages.addEvent( myMidiMessage, 0);
    }
}

//==============================================================================
bool ProgrammerProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProgrammerProcessor::createEditor()
{
    return new ProgrammerEditor (*this);
}

//==============================================================================
void ProgrammerProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void ProgrammerProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProgrammerProcessor();
}
