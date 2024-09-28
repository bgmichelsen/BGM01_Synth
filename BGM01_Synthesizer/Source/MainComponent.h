#pragma once

#include <JuceHeader.h>
#include "Synthesizer.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  :  public juce::AudioAppComponent,
                        private juce::Timer,
                        private juce::MidiKeyboardStateListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Private member variables...
    void timerCallback() override;
    void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    // Objects
    BGM01::SawtoothSynth    mainOsc;
    BGM01::SquareSynth      subOsc;
    BGM01::NoiseSynth       noise;

    juce::MidiKeyboardState     keysState;
    juce::MidiKeyboardComponent keys;

    juce::Slider            mainOsc_Volume;
    juce::Slider            subOsc_Volume;
    juce::Slider            noiseVolume;
    juce::Slider            filterFreq;
    juce::Label             mainOsc_Label;
    juce::Label             subOsc_Label;
    juce::Label             noiseLabel;
    juce::Label             filterLabel;

    juce::dsp::IIR::Filter<float>  lpFilter;
    
    // Variables
    double                  _sampleRate;
    double                  _sampleTime;
    bool                    _noteOn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
