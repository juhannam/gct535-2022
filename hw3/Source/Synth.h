/*
  ==============================================================================

    Synth.h
    Created: May, 2022
    Author:  Minsuk Choi and Jaekwon Im 

  ==============================================================================
*/

#pragma once

//==============================================================================
struct SineWaveSound   : public juce::SynthesiserSound
{
    SineWaveSound() {}

    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

//==============================================================================
struct FMVoice   : public juce::SynthesiserVoice
{
    FMVoice() {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        tailOff = 0.0;
        level = velocity * 0.15;
        currentTime = 0.0;
        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0){
                tailOff = 1.0;
                currentTime = 0.0;
            }
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int) override      {}
    void controllerMoved (int, int) override {}

    
    // get ADSR curve for carrier or modulator from current time and ADSR parameter.
    float getADSRCurve( float attackTime, float decayTime,
                        float sustainLevel, float releaseTime,
                        bool isRelease, float currentLevel
    )
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #1 ////////////////////////////////////////////////////////////////////////////////////////////////
        // Fill this block to implement ADSR curve based on current time. ////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ADSR curve before MIDI note stop. /////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( !isRelease )
        {
            if (( currentTime < attackTime) && ( attackTime > 0 ))
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                // Implement linear attack curve. ////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                return 1.0f; 
            }
            else if (( currentTime > attackTime) && ( currentTime - attackTime < decayTime )) 
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                // Implement exponential decay curve. ////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                return 1.0f;
            }
            else
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                // Implement case for sustain. ///////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                return 1.0f;
            } 
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ADSR curve after MIDI note stop. //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        else
        {
            if ( currentTime < releaseTime )
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                // Implement exponential release curve. //////////////////////////////////////////////////////////////
                // Beware for the case that the note input stopped before reach to sustain level. ////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                return 0.0f;
            }
            else
            {
                return 0.0f;
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #1 END ////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }


    // get current sample from current angle, sample and carrier and modulator parameters.
    float getCurrentSample (    float carrierAmplitude,
                                float carrierAttackTime, float carrierDecayTime,
                                float carrierSustainLevel, float carrierReleaseTime,
                                float modulatorAmplitude, float modulatorFreqRatio,
                                float modulatorAttackTime, float modulatorDecayTime,
                                float modulatorSustainLevel, float modulatorReleaseTime,
                                bool isRelease
    )
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #2 ////////////////////////////////////////////////////////////////////////////////////////////////
        // Modify this block to implement FM synthesis with ADSR curves for both carrier and modulator. //////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get ADSR value for carrier and modulator. /////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float carrierADSR = 1.0f;
        float modulatorADSR = 1.0f;
        
        if ( !isRelease )
        {
            currentCarrierLevel = carrierADSR;
            currentModulatorLevel = modulatorADSR;
        }
        float carAmp = carrierAmplitude * carrierADSR;
        float modAmp = modulatorAmplitude * modulatorADSR;


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Return frequency modulated sound. /////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return (float) ( std::sin (currentAngle) );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #2 END ////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void renderNextBlock (  juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override {}

    void renderNextBlock (  juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples,
                            float carrierAmplitude,
                            float carrierAttackTime, float carrierDecayTime,
                            float carrierSustainLevel, float carrierReleaseTime,
                            float modulatorAmplitude, float modulatorFreqRatio,
                            float modulatorAttackTime, float modulatorDecayTime,
                            float modulatorSustainLevel, float modulatorReleaseTime
    ) 
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0) 
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = getCurrentSample(  carrierAmplitude,
                                                            carrierAttackTime, carrierDecayTime,
                                                            carrierSustainLevel, carrierReleaseTime,
                                                            modulatorAmplitude, modulatorFreqRatio,
                                                            modulatorAttackTime, modulatorDecayTime,
                                                            modulatorSustainLevel, modulatorReleaseTime,
                                                            true
                                                        ) * level;

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    currentTime  += 1/getSampleRate(); 
                    ++startSample;

                    if ( carrierReleaseTime < currentTime ) 
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = getCurrentSample(  carrierAmplitude,
                                                            carrierAttackTime, carrierDecayTime,
                                                            carrierSustainLevel, carrierReleaseTime,
                                                            modulatorAmplitude, modulatorFreqRatio,
                                                            modulatorAttackTime, modulatorDecayTime,
                                                            modulatorSustainLevel, modulatorReleaseTime,
                                                            false
                                                        ) * level;

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    currentTime  += 1/getSampleRate(); 
                    ++startSample;
                }
            }
        }
    }

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    double currentTime = 0.0, currentCarrierLevel = 0.0, currentModulatorLevel = 0.0;
};

//==============================================================================
class FMSynthesizer     : public juce::Synthesiser
{
public:
    void renderVoices (juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        for (auto* voice : voices){
            FMVoice *fmsynthVoice = dynamic_cast<FMVoice*>(voice);
            fmsynthVoice->renderNextBlock(  buffer, startSample, numSamples,
                                            carrierAmplitude,
                                            carrierAttackTime, carrierDecayTime,
                                            carrierSustainLevel, carrierReleaseTime,
                                            modulatorAmplitude, modulatorFreqRatio,
                                            modulatorAttackTime, modulatorDecayTime,
                                            modulatorSustainLevel, modulatorReleaseTime
                                          );
        }
    }
    

    void setCarrierAmplitude(float value)       {this->carrierAmplitude = value;}
    void setCarrierAttackTime(float value)  {this->carrierAttackTime = value;}
    void setCarrierDecayTime(float value)   {this->carrierDecayTime = value;}
    void setCarrierSustainLevel(float value){this->carrierSustainLevel = value;}
    void setCarrierReleaseTime(float value) {this->carrierReleaseTime = value;}
    
    void setModulatorAmplitude(float value)       {this->modulatorAmplitude = value;}
    void setModulatorFreqRatio(float value)   {this->modulatorFreqRatio = value;}
    void setModulatorAttackTime(float value)  {this->modulatorAttackTime = value;}
    void setModulatorDecayTime(float value)   {this->modulatorDecayTime = value;}
    void setModulatorSustainLevel(float value){this->modulatorSustainLevel = value;}
    void setModulatorReleaseTime(float value) {this->modulatorReleaseTime = value;}

private:
    float carrierAmplitude = 1.0f;
    float carrierAttackTime = 0.0f;
    float carrierDecayTime = 0.01f;
    float carrierSustainLevel = 1.0f;
    float carrierReleaseTime = 0.01f;
    
    float modulatorAmplitude = 0.0f;
    float modulatorFreqRatio = 1.0f;
    float modulatorAttackTime = 0.0f;
    float modulatorDecayTime = 0.01f;
    float modulatorSustainLevel = 1.0f;
    float modulatorReleaseTime = 0.01f;
};

//==============================================================================
class SynthAudioSource   : public juce::AudioSource
{
public:
    SynthAudioSource (juce::MidiKeyboardState& keyState)
        : keyboardState (keyState)
    {
        for (auto i = 0; i < 4; ++i)
            synth.addVoice (new FMVoice());

        synth.addSound (new SineWaveSound());
    }

    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate (sampleRate);
        midiCollector.reset (sampleRate);
    }

    void releaseResources() override {}

    void getNextAudioBlock ( const juce::AudioSourceChannelInfo& bufferToFill ) override
    {
        bufferToFill.clearActiveBufferRegion();

        juce::MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);

        keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                             bufferToFill.numSamples, true);

        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                               bufferToFill.startSample, bufferToFill.numSamples);
    }

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }


    void setCarrierAmplitude(float value)           {synth.setCarrierAmplitude(value);}
    void setCarrierAttackTime(float value)      {synth.setCarrierAttackTime(value);}
    void setCarrierDecayTime(float value)       {synth.setCarrierDecayTime(value);}
    void setCarrierSustainLevel(float value)    {synth.setCarrierSustainLevel(value);}
    void setCarrierReleaseTime(float value)     {synth.setCarrierReleaseTime(value);}
    void setModulatorAmplitude(float value)         {synth.setModulatorAmplitude(value);}
    void setModulatorFreqRatio(float value)     {synth.setModulatorFreqRatio(value);}
    void setModulatorAttackTime(float value)    {synth.setModulatorAttackTime(value);}
    void setModulatorDecayTime(float value)     {synth.setModulatorDecayTime(value);}
    void setModulatorSustainLevel(float value)  {synth.setModulatorSustainLevel(value);}
    void setModulatorReleaseTime(float value)   {synth.setModulatorReleaseTime(value);}

private:
    juce::MidiKeyboardState& keyboardState;
    FMSynthesizer synth;
    juce::MidiMessageCollector midiCollector;
};


//==============================================================================
class MainContentComponent   : public juce::AudioAppComponent,
                               private juce::Timer
{
public:
    void loadPreset(juce::String name)
    {
        float presetCarrierAmplitude;
        float presetCarrierAttackTime, presetCarrierDecayTime;
        float presetCarrierSustainLevel, presetCarrierReleaseTime;
        float presetModulatorAmplitude, presetModulatorFreqRatio;
        float presetModulatorAttackTime, presetModulatorDecayTime;
        float presetModulatorSustainLevel, presetModulatorReleaseTime;
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #3 ////////////////////////////////////////////////////////////////////////////////////////////////
        // Modify the preset values for brass, bell and electric piano sound as in course practice codes. ////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (name == "Brass")
        {
            presetCarrierAmplitude      = 1.0f;
            presetCarrierAttackTime     = 0.0f;
            presetCarrierDecayTime      = 0.01f;
            presetCarrierSustainLevel   = 1.0f;
            presetCarrierReleaseTime    = 0.01f;
            
            presetModulatorAmplitude    = 0.0f;
            presetModulatorFreqRatio    = 1.0f;
            presetModulatorAttackTime   = 0.0f;
            presetModulatorDecayTime    = 0.01f;
            presetModulatorSustainLevel = 1.0f;
            presetModulatorReleaseTime  = 0.01f;
        }
        else if (name == "Bell")
        {
            presetCarrierAmplitude      = 1.0f;
            presetCarrierAttackTime     = 0.0f;
            presetCarrierDecayTime      = 0.01f;
            presetCarrierSustainLevel   = 1.0f;
            presetCarrierReleaseTime    = 0.01f;
            
            presetModulatorAmplitude    = 0.0f;
            presetModulatorFreqRatio    = 1.0f;
            presetModulatorAttackTime   = 0.0f;
            presetModulatorDecayTime    = 0.01f;
            presetModulatorSustainLevel = 1.0f;
            presetModulatorReleaseTime  = 0.01f;
        }
        else if (name == "Electric Piano")
        {
            presetCarrierAmplitude      = 1.0f;
            presetCarrierAttackTime     = 0.0f;
            presetCarrierDecayTime      = 0.01f;
            presetCarrierSustainLevel   = 1.0f;
            presetCarrierReleaseTime    = 0.01f;
            
            presetModulatorAmplitude    = 0.0f;
            presetModulatorFreqRatio    = 1.0f;
            presetModulatorAttackTime   = 0.0f;
            presetModulatorDecayTime    = 0.01f;
            presetModulatorSustainLevel = 1.0f;
            presetModulatorReleaseTime  = 0.01f;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #3 END ////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #4 (Bonus) ////////////////////////////////////////////////////////////////////////////////////////
        // Design your own sound. Adjust values for the preset of your sound. Explain your sound design. /////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //// Please leave comment here ///////////////////////////////////////////////////////////////////////////////
        ////
        ////
        ////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        else if (name == "Your Sound")
        {
            presetCarrierAmplitude      = 1.0f;
            presetCarrierAttackTime     = 0.0f;
            presetCarrierDecayTime      = 0.01f;
            presetCarrierSustainLevel   = 1.0f;
            presetCarrierReleaseTime    = 0.01f;
            
            presetModulatorAmplitude    = 0.0f;
            presetModulatorFreqRatio    = 1.0f;
            presetModulatorAttackTime   = 0.0f;
            presetModulatorDecayTime    = 0.01f;
            presetModulatorSustainLevel = 1.0f;
            presetModulatorReleaseTime  = 0.01f;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Problem #4 END ////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        else // default preset.
        {
            presetCarrierAmplitude      = 1.0f;
            presetCarrierAttackTime     = 0.0f;
            presetCarrierDecayTime      = 0.01f;
            presetCarrierSustainLevel   = 1.0f;
            presetCarrierReleaseTime    = 0.01f;
            
            presetModulatorAmplitude    = 0.0f;
            presetModulatorFreqRatio    = 1.0f;
            presetModulatorAttackTime   = 0.0f;
            presetModulatorDecayTime    = 0.01f;
            presetModulatorSustainLevel = 1.0f;
            presetModulatorReleaseTime  = 0.01f;
        }
        
        setParameters(  presetCarrierAmplitude,
                        presetCarrierAttackTime, presetCarrierDecayTime,
                        presetCarrierSustainLevel, presetCarrierReleaseTime,
                        presetModulatorAmplitude, presetModulatorFreqRatio,
                        presetModulatorAttackTime, presetModulatorDecayTime,
                        presetModulatorSustainLevel, presetModulatorReleaseTime );
    }

    void setParameters(
                            float carrierAmplitude,
                            float carrierAttackTime, float carrierDecayTime,
                            float carrierSustainLevel, float carrierReleaseTime,
                            float modulatorAmplitude, float modulatorFreqRatio,
                            float modulatorAttackTime, float modulatorDecayTime,
                            float modulatorSustainLevel, float modulatorReleaseTime
    )
    {
        carrierAmplitudeSlider.setValue (carrierAmplitude, juce::dontSendNotification);
        carrierAttackTimeSlider.setValue (carrierAttackTime, juce::dontSendNotification);
        carrierDecayTimeSlider.setValue (carrierDecayTime, juce::dontSendNotification);
        carrierSustainLevelSlider.setValue (carrierSustainLevel, juce::dontSendNotification);
        carrierReleaseTimeSlider.setValue (carrierReleaseTime, juce::dontSendNotification);
        synthAudioSource.setCarrierAmplitude(carrierAmplitudeSlider.getValue());
        synthAudioSource.setCarrierAttackTime(carrierAttackTimeSlider.getValue());
        synthAudioSource.setCarrierDecayTime(carrierDecayTimeSlider.getValue());
        synthAudioSource.setCarrierSustainLevel(carrierSustainLevelSlider.getValue());
        synthAudioSource.setCarrierReleaseTime(carrierReleaseTimeSlider.getValue());
        
        modulatorAmplitudeSlider.setValue (modulatorAmplitude, juce::dontSendNotification);
        modulatorFreqRatioSlider.setValue (modulatorFreqRatio, juce::dontSendNotification);
        modulatorAttackTimeSlider.setValue (modulatorAttackTime, juce::dontSendNotification);
        modulatorDecayTimeSlider.setValue (modulatorDecayTime, juce::dontSendNotification);
        modulatorSustainLevelSlider.setValue (modulatorSustainLevel, juce::dontSendNotification);
        modulatorReleaseTimeSlider.setValue (modulatorReleaseTime, juce::dontSendNotification);
        synthAudioSource.setModulatorAmplitude(modulatorAmplitudeSlider.getValue());
        synthAudioSource.setModulatorFreqRatio(modulatorFreqRatioSlider.getValue());
        synthAudioSource.setModulatorAttackTime(modulatorAttackTimeSlider.getValue());
        synthAudioSource.setModulatorDecayTime(modulatorDecayTimeSlider.getValue());
        synthAudioSource.setModulatorSustainLevel(modulatorSustainLevelSlider.getValue());
        synthAudioSource.setModulatorReleaseTime(modulatorReleaseTimeSlider.getValue());
    }

    MainContentComponent()
        : synthAudioSource  (keyboardState),
          keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible (carrierAmplitudeSlider);
        carrierAmplitudeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        carrierAmplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        carrierAmplitudeSlider.setRange (0, 5, 0.1);
        carrierAmplitudeSlider.setValue (1, juce::dontSendNotification);
        carrierAmplitudeSlider.onValueChange = [this] { synthAudioSource.setCarrierAmplitude(carrierAmplitudeSlider.getValue()); };

        addAndMakeVisible (carrierAttackTimeSlider);
        carrierAttackTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        carrierAttackTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        carrierAttackTimeSlider.setRange (0, 5, 0.01);
        carrierAttackTimeSlider.setValue (0, juce::dontSendNotification);
        carrierAttackTimeSlider.onValueChange = [this] { synthAudioSource.setCarrierAttackTime(carrierAttackTimeSlider.getValue()); };

        addAndMakeVisible (carrierDecayTimeSlider);
        carrierDecayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        carrierDecayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        carrierDecayTimeSlider.setRange (0.01, 5, 0.01);
        carrierDecayTimeSlider.setValue (0, juce::dontSendNotification);
        carrierDecayTimeSlider.onValueChange = [this] { synthAudioSource.setCarrierDecayTime(carrierDecayTimeSlider.getValue()); };

        addAndMakeVisible (carrierSustainLevelSlider);
        carrierSustainLevelSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        carrierSustainLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        carrierSustainLevelSlider.setRange (0.1, 1, 0.1);
        carrierSustainLevelSlider.setValue (1, juce::dontSendNotification);
        carrierSustainLevelSlider.onValueChange = [this] { synthAudioSource.setCarrierSustainLevel(carrierSustainLevelSlider.getValue()); };

        addAndMakeVisible (carrierReleaseTimeSlider);
        carrierReleaseTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        carrierReleaseTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        carrierReleaseTimeSlider.setRange (0.01, 5, 0.01);
        carrierReleaseTimeSlider.setValue (0, juce::dontSendNotification);
        carrierReleaseTimeSlider.onValueChange = [this] { synthAudioSource.setCarrierReleaseTime(carrierReleaseTimeSlider.getValue()); };

        modulatorAmplitudeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        modulatorAmplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        modulatorAmplitudeSlider.setRange (0, 5, 0.1);
        modulatorAmplitudeSlider.setValue (0, juce::dontSendNotification);
        modulatorAmplitudeSlider.onValueChange = [this] { synthAudioSource.setModulatorAmplitude(modulatorAmplitudeSlider.getValue()); };
        addAndMakeVisible (modulatorAmplitudeSlider);

        modulatorFreqRatioSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        modulatorFreqRatioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        modulatorFreqRatioSlider.setRange (0.1, 10, 0.1);
        modulatorFreqRatioSlider.setValue (1, juce::dontSendNotification);
        modulatorFreqRatioSlider.onValueChange = [this] { synthAudioSource.setModulatorFreqRatio(modulatorFreqRatioSlider.getValue()); };
        addAndMakeVisible (modulatorFreqRatioSlider);

        addAndMakeVisible (modulatorAttackTimeSlider);
        modulatorAttackTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        modulatorAttackTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        modulatorAttackTimeSlider.setRange (0, 5, 0.01);
        modulatorAttackTimeSlider.setValue (0, juce::dontSendNotification);
        modulatorAttackTimeSlider.onValueChange = [this] { synthAudioSource.setModulatorAttackTime(modulatorAttackTimeSlider.getValue()); };

        addAndMakeVisible (modulatorDecayTimeSlider);
        modulatorDecayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        modulatorDecayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        modulatorDecayTimeSlider.setRange (0.01, 5, 0.01);
        modulatorDecayTimeSlider.setValue (0.01, juce::dontSendNotification);
        modulatorDecayTimeSlider.onValueChange = [this] { synthAudioSource.setModulatorDecayTime(modulatorDecayTimeSlider.getValue()); };

        addAndMakeVisible (modulatorSustainLevelSlider);
        modulatorSustainLevelSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        modulatorSustainLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        modulatorSustainLevelSlider.setRange (0.01, 1, 0.01);
        modulatorSustainLevelSlider.setValue (1, juce::dontSendNotification);
        modulatorSustainLevelSlider.onValueChange = [this] { synthAudioSource.setModulatorSustainLevel(modulatorSustainLevelSlider.getValue()); };

        addAndMakeVisible (modulatorReleaseTimeSlider);
        modulatorReleaseTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        modulatorReleaseTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        modulatorReleaseTimeSlider.setRange (0.01, 5, 0.01);
        modulatorReleaseTimeSlider.setValue (0.01, juce::dontSendNotification);
        modulatorReleaseTimeSlider.onValueChange = [this] { synthAudioSource.setModulatorReleaseTime(modulatorReleaseTimeSlider.getValue()); };

        addAndMakeVisible (keyboardComponent);

        titleLabel                  .setText("GCT535 FM Synthesizer", juce::dontSendNotification);
        carrierLabel                .setText("Carrier Parameters", juce::dontSendNotification);
        carrierAmplitudeLabel       .setText("Amplitude", juce::dontSendNotification);
        carrierAttackTimeLabel      .setText("Attack [s]", juce::dontSendNotification);
        carrierDecayTimeLabel       .setText("Decay [s]", juce::dontSendNotification);
        carrierSustainLevelLabel    .setText("Sustain", juce::dontSendNotification);
        carrierReleaseTimeLabel     .setText("Release [s]", juce::dontSendNotification);
        modulatorLabel              .setText("Modulator Parameters", juce::dontSendNotification);
        modulatorAmplitudeLabel     .setText("Amplitude", juce::dontSendNotification);
        modulatorFreqRatioLabel     .setText("Freq. Ratio", juce::dontSendNotification);
        modulatorAttackTimeLabel    .setText("Attack [s]", juce::dontSendNotification);
        modulatorDecayTimeLabel     .setText("Decay [s]", juce::dontSendNotification);
        modulatorSustainLevelLabel  .setText("Sustain", juce::dontSendNotification);
        modulatorReleaseTimeLabel   .setText("Release [s]", juce::dontSendNotification);
        presetListLabel             .setText("Presets", juce::dontSendNotification);

        titleLabel                  .setJustificationType(juce::Justification::centredLeft);
        carrierLabel                .setJustificationType(juce::Justification::centred);
        carrierAmplitudeLabel       .setJustificationType(juce::Justification::centred);
        carrierAttackTimeLabel      .setJustificationType(juce::Justification::centred);
        carrierDecayTimeLabel       .setJustificationType(juce::Justification::centred);
        carrierSustainLevelLabel    .setJustificationType(juce::Justification::centred);
        carrierReleaseTimeLabel     .setJustificationType(juce::Justification::centred);
        modulatorLabel              .setJustificationType(juce::Justification::centred);
        modulatorAmplitudeLabel     .setJustificationType(juce::Justification::centred);
        modulatorFreqRatioLabel     .setJustificationType(juce::Justification::centred);
        modulatorAttackTimeLabel    .setJustificationType(juce::Justification::centred);
        modulatorDecayTimeLabel     .setJustificationType(juce::Justification::centred);
        modulatorSustainLevelLabel  .setJustificationType(juce::Justification::centred);
        modulatorReleaseTimeLabel   .setJustificationType(juce::Justification::centred);
        presetListLabel             .setJustificationType(juce::Justification::centred);

        addAndMakeVisible (titleLabel);
        addAndMakeVisible (carrierLabel);
        addAndMakeVisible (carrierAmplitudeLabel);
        addAndMakeVisible (carrierAttackTimeLabel);
        addAndMakeVisible (carrierDecayTimeLabel);
        addAndMakeVisible (carrierSustainLevelLabel);
        addAndMakeVisible (carrierReleaseTimeLabel);
        addAndMakeVisible (modulatorLabel);
        addAndMakeVisible (modulatorAmplitudeLabel);
        addAndMakeVisible (modulatorFreqRatioLabel);
        addAndMakeVisible (modulatorAttackTimeLabel);
        addAndMakeVisible (modulatorDecayTimeLabel);
        addAndMakeVisible (modulatorSustainLevelLabel);
        addAndMakeVisible (modulatorReleaseTimeLabel);
        addAndMakeVisible (presetListLabel);
        
        addAndMakeVisible (presetList);
        juce::StringArray presetNames;
        presetNames.add("Default");
        presetNames.add("Bell");
        presetNames.add("Brass");
        presetNames.add("Electric Piano");
        presetNames.add("Your Sound");
        presetList.addItemList( presetNames, 1 );
        presetList.setSelectedItemIndex(0);
        presetList.onChange = [this] { loadPreset (presetList.getItemText(presetList.getSelectedItemIndex())); };

        setAudioChannels (0, 2);
        setSize(820, 320);
        startTimer (400);
    }

    ~MainContentComponent() override
    {
        shutdownAudio();
    }

    void resized() override
    {
        int borderLeft  = 10; 
        int borderTop   = 10;
        int dialWidth   = 70;
        int dialHeight  = 70;
        int labelHeight = 40;

        carrierLabel                .setBounds (0, borderTop, 410, 20);
        modulatorLabel              .setBounds (410, borderTop, 410, 20);
        carrierAmplitudeLabel       .setBounds (borderLeft   + dialWidth*0,  borderTop+25, 70, 20);
        carrierAttackTimeLabel      .setBounds (borderLeft   + dialWidth*1,  borderTop+25, 70, 20);
        carrierDecayTimeLabel       .setBounds (borderLeft   + dialWidth*2,  borderTop+25, 70, 20);
        carrierSustainLevelLabel    .setBounds (borderLeft   + dialWidth*3,  borderTop+25, 70, 20);
        carrierReleaseTimeLabel     .setBounds (borderLeft   + dialWidth*4,  borderTop+25, 70, 20);
        modulatorAmplitudeLabel     .setBounds (borderLeft*4 + dialWidth*5,  borderTop+25, 70, 20);
        modulatorFreqRatioLabel     .setBounds (borderLeft*4 + dialWidth*6,  borderTop+25, 70, 20);
        modulatorAttackTimeLabel    .setBounds (borderLeft*4 + dialWidth*7,  borderTop+25, 70, 20);
        modulatorDecayTimeLabel     .setBounds (borderLeft*4 + dialWidth*8,  borderTop+25, 70, 20);
        modulatorSustainLevelLabel  .setBounds (borderLeft*4 + dialWidth*9,  borderTop+25, 70, 20);
        modulatorReleaseTimeLabel   .setBounds (borderLeft*4 + dialWidth*10, borderTop+25, 70, 20);

        carrierAmplitudeSlider      .setBounds (borderLeft   + dialWidth*0,  borderTop+labelHeight, dialWidth, dialHeight);
        carrierAttackTimeSlider     .setBounds (borderLeft   + dialWidth*1,  borderTop+labelHeight, dialWidth, dialHeight);
        carrierDecayTimeSlider      .setBounds (borderLeft   + dialWidth*2,  borderTop+labelHeight, dialWidth, dialHeight);
        carrierSustainLevelSlider   .setBounds (borderLeft   + dialWidth*3,  borderTop+labelHeight, dialWidth, dialHeight);
        carrierReleaseTimeSlider    .setBounds (borderLeft   + dialWidth*4,  borderTop+labelHeight, dialWidth, dialHeight);
        modulatorAmplitudeSlider    .setBounds (borderLeft*4 + dialWidth*5,  borderTop+labelHeight, dialWidth, dialHeight);
        modulatorFreqRatioSlider    .setBounds (borderLeft*4 + dialWidth*6,  borderTop+labelHeight, dialWidth, dialHeight);
        modulatorAttackTimeSlider   .setBounds (borderLeft*4 + dialWidth*7,  borderTop+labelHeight, dialWidth, dialHeight);
        modulatorDecayTimeSlider    .setBounds (borderLeft*4 + dialWidth*8,  borderTop+labelHeight, dialWidth, dialHeight);
        modulatorSustainLevelSlider .setBounds (borderLeft*4 + dialWidth*9,  borderTop+labelHeight, dialWidth, dialHeight);
        modulatorReleaseTimeSlider  .setBounds (borderLeft*4 + dialWidth*10, borderTop+labelHeight, dialWidth, dialHeight);

        keyboardComponent           .setBounds (borderLeft, borderTop+labelHeight+dialHeight+10, 800, 150);

        titleLabel                  .setBounds ( 30,  290, 300, 20);
        presetListLabel             .setBounds ( 595, 290, 80,  20);
        presetList                  .setBounds ( 665, 290, 120, 20);
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        synthAudioSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        synthAudioSource.getNextAudioBlock (bufferToFill);
    }

    void releaseResources() override
    {
        synthAudioSource.releaseResources();
    }

private:
    void timerCallback() override
    {
        keyboardComponent.grabKeyboardFocus();
        stopTimer();
    }

    juce::Label titleLabel;
    juce::Label carrierLabel;
    juce::Label modulatorLabel;

    juce::Label carrierAmplitudeLabel;
    juce::Label carrierAttackTimeLabel;
    juce::Label carrierDecayTimeLabel;
    juce::Label carrierSustainLevelLabel;
    juce::Label carrierReleaseTimeLabel;
    juce::Slider carrierAmplitudeSlider;
    juce::Slider carrierAttackTimeSlider;
    juce::Slider carrierDecayTimeSlider;
    juce::Slider carrierSustainLevelSlider;
    juce::Slider carrierReleaseTimeSlider;

    juce::Label modulatorAmplitudeLabel;
    juce::Label modulatorFreqRatioLabel;
    juce::Label modulatorAttackTimeLabel;
    juce::Label modulatorDecayTimeLabel;
    juce::Label modulatorSustainLevelLabel;
    juce::Label modulatorReleaseTimeLabel;
    juce::Slider modulatorAmplitudeSlider;
    juce::Slider modulatorFreqRatioSlider;
    juce::Slider modulatorAttackTimeSlider;
    juce::Slider modulatorDecayTimeSlider;
    juce::Slider modulatorSustainLevelSlider;
    juce::Slider modulatorReleaseTimeSlider;

    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthAudioSource;
    juce::MidiKeyboardComponent keyboardComponent;

    juce::Label presetListLabel;
    juce::ComboBox presetList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
