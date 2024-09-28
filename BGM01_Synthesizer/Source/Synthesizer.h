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
        SynthesizerBase(double lvl, double freq) : _targetLevel(lvl), _frequency(freq)
        {
            _currentLevel = lvl;
        };
        ~SynthesizerBase()
        {

        };

        virtual double  sample(double t);
        void    setFrequency(double freq);
        void    setLevel(double lvl);
        void    incrementLevel(void);
    protected:
        double  _targetLevel;
        double  _currentLevel;
        double  _frequency;
    };

    // Noise synthesizer
    class NoiseSynth : public SynthesizerBase
    {
    public:
        NoiseSynth(double lvl) : SynthesizerBase(lvl, 0.0f)
        {

        };
        ~NoiseSynth()
        {

        }

        double  sample(double t) override;
    private:
        juce::Random    _random;
    };

    // Sawtooth synthesizer
    class SawtoothSynth : public SynthesizerBase
    {
    public:
        SawtoothSynth(double lvl, double freq) : SynthesizerBase(lvl, freq)
        {

        }
        ~SawtoothSynth()
        {

        }

        double  sample(double t) override;
    private:

    };

    // Square wave synth
    class SquareSynth : public SynthesizerBase
    {
    public:
        SquareSynth(double lvl, double freq) : SynthesizerBase(lvl, freq)
        {

        }
        ~SquareSynth()
        {

        }

        double sample(double t) override;
    private:

    };
}
//==============================================================================