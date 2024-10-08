/*
  ==============================================================================

    Synthesizer.cpp
    Created: 22 Sep 2024 3:18:41pm
    Author:  brand

  ==============================================================================
*/

#include "Synthesizer.h"
#include <math.h>

//==============================================================================
// BGM01 Namespace for synthesizer
namespace BGM01
{
    // @brief       Function for sampling the waveform
    // @param       t = Sample time
    float SynthesizerBase::sample(float t)
    {
        // The base class is just a DC source
        return 0.0f;
    }

    // @brief       Function to set the waveform frequency
    // @param       freq = The frequency to set to
    void SynthesizerBase::setFrequency(float freq)
    {
        // Make sure frequency is within human hearing range (16Hz to 20kHz)
        if ((10.0f <= freq) && (20000.0f >= freq))
        {
            _frequency = freq;
        }
    }

    // @brief       Function to set the level for waveform
    // @param       lvl = The level to set to
    void SynthesizerBase::setLevel(float lvl)
    {
        // Make sure level is between 0 and 1
        if ((0.0f <= lvl) && (1.0f >= lvl))
        {
            _targetLevel = lvl;
        }
    }

    // @brief       Function for incrementing level
    // @param       None
    void SynthesizerBase::incrementLevel()
    {
        // Only increment while under the target level
        if (_currentLevel < (_targetLevel - FLT_EPSILON))
        {
            _currentLevel += 0.01;
        }
        else
        {
            _currentLevel = _targetLevel;
        }
    }

    // @brief       Function override of sample for noise synth
    // @param       t = Sample time
    float NoiseSynth::sample(float t)
    {
        float lvlScale = 0.0f;

        if (0.0f < _targetLevel)
        {
            incrementLevel();
            lvlScale = _currentLevel * 2.0f;
            return _random.nextFloat() * lvlScale - _currentLevel;
        }
        else
        {
            return 0.0f;
        }
    }

    // @brief       Function override of sample for sawtooth synth
    // @param       t = Sample time
    float SawtoothSynth::sample(float t)
    {
        float period       = (1.0f / _frequency);
        float sample_mod   = fmod(t, period);
        if (0.0f < _targetLevel)
        {
            incrementLevel();
            return _currentLevel * ((sample_mod * _frequency) * 2.0f - 1.0f);
        }
        else
        {
            return 0.0f;
        }
    }

    // @brief       Function override of sample for square wave synth
    // @param       t = Sample time
    float SquareSynth::sample(float t)
    {
        float period = (1.0f / _frequency);
        float half_period = (period / 2.0f);
        float time = fmod(t, period);

        if (0.0f < _targetLevel)
        {
            incrementLevel();
            if (time < half_period)
            {
                return (1.0f * _currentLevel);
            }
            else
            {
                return (-1.0f * _currentLevel);
            }
        }
        else
        {
            return 0.0f;
        }
    }
}
//==============================================================================