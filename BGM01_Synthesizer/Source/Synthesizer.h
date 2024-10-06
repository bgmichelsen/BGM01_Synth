/*
  ==============================================================================

    Synthesizer.h
    Created: 22 Sep 2024 3:18:41pm
    Author:  brand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
// BGM01 Namespace for synthesizer
namespace BGM01
{
    // Base synthesizer that all others inherit from
    class SynthesizerBase
    {
    public:
        SynthesizerBase(float lvl, float freq) : _targetLevel(lvl), _frequency(freq)
        {
            _currentLevel = lvl;
        };
        ~SynthesizerBase()
        {

        };

        virtual float  sample(float t);
        void    setFrequency(float freq);
        void    setLevel(float lvl);
        void    incrementLevel(void);
    protected:
        float  _targetLevel;
        float  _currentLevel;
        float  _frequency;
    };

    // Noise synthesizer
    class NoiseSynth : public SynthesizerBase
    {
    public:
        NoiseSynth(float lvl) : SynthesizerBase(lvl, 0.0f)
        {

        };
        ~NoiseSynth()
        {

        }

        float  sample(float t) override;
    private:
        juce::Random    _random;
    };

    // Sawtooth synthesizer
    class SawtoothSynth : public SynthesizerBase
    {
    public:
        SawtoothSynth(float lvl, float freq) : SynthesizerBase(lvl, freq)
        {

        }
        ~SawtoothSynth()
        {

        }

        float  sample(float t) override;
    private:

    };

    // Square wave synth
    class SquareSynth : public SynthesizerBase
    {
    public:
        SquareSynth(float lvl, float freq) : SynthesizerBase(lvl, freq)
        {

        }
        ~SquareSynth()
        {

        }

        float sample(float t) override;
    private:

    };
}
//==============================================================================