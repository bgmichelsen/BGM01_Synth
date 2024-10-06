#include "MainComponent.h"
#include "Synthesizer.h"

//==============================================================================
MainComponent::MainComponent() :    mainOsc(0.05f, 880.0f), 
                                    subOsc(0.02f, 220.0f),
                                    noise(0.01f),
                                    keys(keysState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible(keys);
    keysState.addListener(this);

    // Setup main oscillator volume and label
    mainOsc_Volume.setRange(0.0f, 0.5f, 0.01f);
    mainOsc_Volume.setValue(0.05f);
    mainOsc_Volume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mainOsc_Volume.onValueChange = [this] { mainOsc.setLevel(mainOsc_Volume.getValue()); };
    mainOsc_Label.setText("Main Osc.", juce::dontSendNotification);
    mainOsc_Label.attachToComponent(&mainOsc_Volume, false);
    addAndMakeVisible(mainOsc_Volume);
    addAndMakeVisible(mainOsc_Label);

    // Setup main oscillator volume and label
    subOsc_Volume.setRange(0.0f, 0.5f, 0.01f);
    subOsc_Volume.setValue(0.02f);
    subOsc_Volume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    subOsc_Volume.onValueChange = [this] { subOsc.setLevel(subOsc_Volume.getValue()); };
    subOsc_Label.setText("Sub Osc.", juce::dontSendNotification);
    subOsc_Label.attachToComponent(&subOsc_Volume, true);
    addAndMakeVisible(subOsc_Volume);
    addAndMakeVisible(subOsc_Label);

    // Setup main oscillator volume and label
    noiseVolume.setRange(0.0f, 0.5f, 0.01f);
    noiseVolume.setValue(0.01f);
    noiseVolume.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    noiseVolume.onValueChange = [this] { noise.setLevel(noiseVolume.getValue()); };
    noiseLabel.setText("Noise", juce::dontSendNotification);
    noiseLabel.attachToComponent(&noiseVolume, true);
    addAndMakeVisible(noiseVolume);
    addAndMakeVisible(noiseLabel);

    // Setup low pass filter frequency slider
    filterFreq.setRange(16.0f, 20000.0f, 0.1f);
    filterFreq.setValue(1000.0f);
    filterFreq.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    filterFreq.onValueChange = [this] {
        auto freq = filterFreq.getValue();
        auto coeffs = juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass(_sampleRate, freq);
        *lpFilter.coefficients = coeffs;
    };
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.attachToComponent(&filterFreq, true);
    addAndMakeVisible(filterFreq);
    addAndMakeVisible(filterLabel);



    // Make sure you set the size of the component after
    // you add any child components.
    setSize(800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }

    // Initialize variables
    _noteOn = false;

    // Wait to grab keyboard
    startTimer(400);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    _sampleTime = static_cast<float>(1.0 / sampleRate);
    _sampleRate = sampleRate;

    juce::dsp::ProcessSpec specs = { sampleRate, (2 *samplesPerBlockExpected), 2 };
    lpFilter.prepare(specs);
    hpFilter.prepare(specs);
    auto freq = filterFreq.getValue();
    auto coeffs = juce::dsp::IIR::ArrayCoefficients<float>::makeFirstOrderLowPass(_sampleRate, freq);
    *lpFilter.coefficients = coeffs;
    coeffs = juce::dsp::IIR::ArrayCoefficients<float>::makeFirstOrderHighPass(_sampleRate, 100.0f);
    *hpFilter.coefficients = coeffs;
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    // Only play notes when a note is on
    //if (_noteOn)
    {
        processSynth(bufferToFill);
    }
}

void MainComponent::releaseResources()
{
    
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    constexpr double margin     = 3.0f;
    constexpr double sliders_h  = 200.0f;
    constexpr double slider_s   = 50.0f;
    constexpr double keys_h     = 80.0f;

    auto top_bar = area.removeFromTop(sliders_h);

    mainOsc_Volume.setBounds(top_bar.removeFromTop(slider_s).reduced(margin));
    subOsc_Volume.setBounds(top_bar.removeFromTop(slider_s).reduced(margin));
    noiseVolume.setBounds(top_bar.removeFromTop(slider_s).reduced(margin));
    filterFreq.setBounds(top_bar.removeFromTop(slider_s).reduced(margin));
    keys.setBounds(area.removeFromBottom(keys_h));
}

void MainComponent::timerCallback()
{
    keys.grabKeyboardFocus();
    stopTimer();
}

void MainComponent::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    mainOsc.setFrequency(freq);
    subOsc.setFrequency(freq / 2.0f);
    _noteOn = true;
}

void MainComponent::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    _noteOn = false;
}

void MainComponent::processSynth(const juce::AudioSourceChannelInfo& outBuffer)
{
    // NOTE:    Need to use oversampling to get a proper sawtooth sound
    //          oversampling using naive approach, i.e. buffer twice the 
    //          size as the original, where every other sample has the
    //          synth data.
    
    juce::AudioBuffer<float> oversample_buf(1, (2 * outBuffer.numSamples));
    lpFilter.reset();

    auto* data_buf  = oversample_buf.getWritePointer(0, outBuffer.startSample);
    float time     = 0.0f;
    for (int sample = 0; sample < outBuffer.numSamples; sample++)
    {
        auto sample_data    = mainOsc.sample(time);

        sample_data += subOsc.sample(time);
        sample_data += noise.sample(time);

        int idx             = (sample << 1);
        data_buf[idx]       = sample_data;
        data_buf[idx + 1]   = 0.0f;
        data_buf[idx]       = lpFilter.processSample(data_buf[idx]);
        data_buf[idx + 1]   = lpFilter.processSample(data_buf[idx + 1]);
        data_buf[idx]       = hpFilter.processSample(data_buf[idx]);
        data_buf[idx + 1]   = hpFilter.processSample(data_buf[idx + 1]);
        lpFilter.snapToZero();
        hpFilter.snapToZero();
        time += _sampleTime;
    }

    for (int channel = 0; channel < outBuffer.buffer->getNumChannels(); channel++)
    {
        auto* out_buf = outBuffer.buffer->getWritePointer(channel, outBuffer.startSample);
        for (int sample = 0; sample < outBuffer.numSamples; sample++)
        {
            out_buf[sample] = data_buf[sample << 1];
        }
    }
}
