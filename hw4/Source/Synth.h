/*
  ==============================================================================

    Synth.h
    Created: May, 2022
    Author:  Minsuk Choi and Jaekwon Im 

  ==============================================================================
*/

#pragma once
#define PI        3.14159265358979323846264338327950288

//==============================================================================
template <typename Type>
class DelayLine
{
public:
    void clear() noexcept
    {
        std::fill (rawData.begin(), rawData.end(), Type (0));
    }

    size_t size() const noexcept
    {
        return rawData.size();
    }

    void resize (size_t newValue)
    {
        rawData.resize (newValue);
    }

    Type get (size_t readPointer) const noexcept
    {
        return rawData[readPointer];
    }

    void push (size_t writePointer, Type valueToAdd) noexcept
    {
        rawData[writePointer] = valueToAdd;
    }

private:
    std::vector<Type> rawData;
};

//==============================================================================
template <typename Type, size_t maxNumChannels = 2>
class Effect
{
public:
    Effect()
    {
        setMaxDelayTime (2.0f);
        setDelayTimes (0.1f);
        setWetDry (0.5f);
        setFeedback (0.5f);
        setLFORate (2.0f);
        setLFODepth (0.01f);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.numChannels <= maxNumChannels);
        sampleRate = (Type) spec.sampleRate;
        updateDelayLineSize();
    }

    void reset() noexcept
    {
        for (size_t ch = 0; ch < getNumChannels(); ++ch)
        {
            delayLines[ch].clear();
            writePointer[ch] = 0;
        }
    }

    size_t getNumChannels() const noexcept
    {
        return delayLines.size();
    }

    void setSampleRate(Type newValue)
    {
        sampleRate = newValue;
        maxDelaySample = (float)(sampleRate*maxDelayTime);
        LFOPhaseIncrement = 2.0f*PI*LFORate/sampleRate;
    }

    void setMaxDelayTime (Type newValue)
    {
        jassert (newValue > Type (0));
        maxDelayTime = newValue;
        updateDelayLineSize();
        maxDelaySample = (float)(sampleRate*maxDelayTime);
    }

    void setFeedback (Type newValue) noexcept
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        feedback = newValue;
    }

    void setWetDry (Type newValue) noexcept
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        wetDry = newValue;
    }

    void setDelayTime (size_t channel, Type newValue)
    {
        if (channel >= getNumChannels())
        {
            jassertfalse;
            return;
        }

        jassert (newValue >= Type (0));
        delayTimes[channel] = newValue;
    }
    
    void setDelayTimes (Type newValue)
    {
        for (size_t ch = 0; ch < getNumChannels(); ++ch)
        {
            jassert (newValue >= Type (0));
            delayTimes[ch] = newValue;
        }
    }

    void setLFORate (Type newValue)
    {
        LFORate = newValue;
        LFOPhaseIncrement = 2.0f*PI*LFORate/sampleRate;
    }

    void setLFODepth (Type newValue)
    {
        LFODepth = newValue;
    }

    void setFXType (juce::String newValue)
    {
        this->reset();
        FXType = newValue;
    }

    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto& inputBlock  = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples  = outputBlock.getNumSamples();
        auto numChannels = outputBlock.getNumChannels();

        jassert (inputBlock.getNumSamples() == numSamples);
        jassert (inputBlock.getNumChannels() == numChannels);
      
        if (delayTimes[0] > 0)
        {
            if (FXType == "Delay")
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Problem #1 ////////////////////////////////////////////////////////////////////////////////////////////////
                // Modify this block to implement a simple delay effect.  ////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                for (size_t ch = 0; ch < numChannels; ++ch)
                {
                    auto* input  = inputBlock .getChannelPointer (ch);
                    auto* output = outputBlock.getChannelPointer (ch);
                    auto& dline = delayLines[ch];
                    auto delayTimeSamples = (float)(delayTimes[ch]*sampleRate);

                    for (size_t i = 0; i < numSamples; ++i)
                    {
                        float outputPointer;
                        readPointer = 0;
                        float readPointerFrac;
                        float tapOut;
                        auto inputSample = input[i];
                        dline.push (writePointer[ch], 0);

                        auto outputSample = wetDry*inputSample;
                        if (outputSample > 1) {outputSample = 1.0f;}
                        output[i] = outputSample;
                    }
                }

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Problem #1 END ////////////////////////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            else if (FXType == "Chorus")
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Problem #2 ////////////////////////////////////////////////////////////////////////////////////////////////
                // Modify this block to implement a chorus effect.  //////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                for (size_t ch = 0; ch < numChannels; ++ch)
                {
                    auto* input  = inputBlock .getChannelPointer (ch);
                    auto* output = outputBlock.getChannelPointer (ch);
                    auto& dline = delayLines[ch];
                    auto delayTimeSamples = (float)(delayTimes[ch]*sampleRate);
                    
                    for (size_t i = 0; i < numSamples; ++i)
                    {
                        LFOPhase[ch] = 0.0f;
                        float LFOOut;
                        float outputPointer;
                        readPointer = 0;
                        float readPointerFrac;
                        float tapOut;
                        auto inputSample = input[i];
                        dline.push (writePointer[ch], 0);
                        writePointer[ch] = writePointer[ch] + 1;
                        if (writePointer[ch] >= maxDelaySample)
                        {
                            writePointer[ch] = 0;
                        }

                        auto outputSample = wetDry*inputSample;
                        if (outputSample > 1) {outputSample = 1.0f;}
                        output[i] = outputSample;
                    }
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Problem #2 END ////////////////////////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
            else if (FXType == "Flanger")
            {
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Problem #3 ////////////////////////////////////////////////////////////////////////////////////////////////
                // Modify this block to implement a flanger effect.  /////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                for (size_t ch = 0; ch < numChannels; ++ch)
                {
                    auto* input  = inputBlock .getChannelPointer (ch);
                    auto* output = outputBlock.getChannelPointer (ch);
                    auto& dline = delayLines[ch];
                    auto delayTimeSamples = (float)(delayTimes[ch]*sampleRate);
                    auto variableDelayTimeSamples = (float)((delayTimes[ch]*1.125f)*sampleRate);
                    
                    for (size_t i = 0; i < numSamples; ++i)
                    {
                        LFOPhase[ch] = 0.0f;
                        float LFOOut;
                        float staticOutputPointer = 0;
                        float staticReadPointerFrac;
                        float staticTapOut;
                        float variableOutputPointer;
                        float variableReadPointerFrac;
                        float variableTapOut;
                        float tapOut;
                        auto inputSample = input[i];
                        dline.push (writePointer[ch], 0);

                        auto outputSample = wetDry*inputSample;
                        if (outputSample > 1) {outputSample = 1.0f;}
                        output[i] = outputSample;
                    }
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Problem #3 END ////////////////////////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
        }
    }
private:
    std::array<DelayLine<Type>, maxNumChannels> delayLines;
    std::array<Type, maxNumChannels> delayTimes;
    Type feedback { Type (0) };
    Type wetDry { Type (0) };

    Type sampleRate   { Type (48000) };
    Type maxDelayTime { Type (2.0f) };

    float maxDelaySample = (float)(sampleRate*maxDelayTime);

    void updateDelayLineSize()
    {
        auto delayLineSizeSamples = (size_t) std::ceil (maxDelayTime * sampleRate);
        for (auto& dline : delayLines)
            dline.resize (delayLineSizeSamples);
    }

    float LFORate = 0.0f;
    float LFODepth = 0.0f;
    float LFOPhase[2] = {};
    float LFOPhaseIncrement = 0.0f;

    juce::String FXType = "None";
    
    size_t readPointer;
    size_t staticReadPointer;
    size_t variableReadPointer;
    size_t writePointer[2] = {0, 0};
};

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
        // Problem #0 ////////////////////////////////////////////////////////////////////////////////////////////////
        // Replace this block with your Homework #3 solution. ////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( !isRelease )
        {
            if (( currentTime < attackTime) && ( attackTime > 0 ))
            {
                return 1.0f; 
            }
            else if (( currentTime > attackTime) && ( currentTime - attackTime < decayTime )) 
            {
                return 1.0f;
            }
            else
            {
                return 1.0f;
            } 
        }
        else
        {
            if ( currentTime < releaseTime )
            {
                return 0.0f;
            }
            else
            {
                return 0.0f;
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        // Problem #0 ////////////////////////////////////////////////////////////////////////////////////////////////
        // Replace this block with your Homework #3 solution. ////////////////////////////////////////////////////////
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
        
        return (float) ( std::sin (currentAngle) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        auto block = juce::dsp::AudioBlock<float> (buffer).getSubBlock(startSample, numSamples);
        auto context = juce::dsp::ProcessContextReplacing<float> (block);
        if (FXType != "None") { FX.process(context); }
    }

    void setCarrierAmplitude(float value)       {this->carrierAmplitude = value;}
    void setCarrierAttackTime(float value)      {this->carrierAttackTime = value;}
    void setCarrierDecayTime(float value)       {this->carrierDecayTime = value;}
    void setCarrierSustainLevel(float value)    {this->carrierSustainLevel = value;}
    void setCarrierReleaseTime(float value)     {this->carrierReleaseTime = value;}
    
    void setModulatorAmplitude(float value)     {this->modulatorAmplitude = value;}
    void setModulatorFreqRatio(float value)     {this->modulatorFreqRatio = value;}
    void setModulatorAttackTime(float value)    {this->modulatorAttackTime = value;}
    void setModulatorDecayTime(float value)     {this->modulatorDecayTime = value;}
    void setModulatorSustainLevel(float value)  {this->modulatorSustainLevel = value;}
    void setModulatorReleaseTime(float value)   {this->modulatorReleaseTime = value;}

    void setFXType (juce::String value) {FX.reset(); FX.setFXType(value); this->FXType = value;}
    void setFeedback (float value)      {FX.reset(); FX.setFeedback(value);}
    void setDelayTime (float value)     {FX.reset(); FX.setDelayTimes(value);}
    void setWetDry (float value)        {FX.reset(); FX.setWetDry(value);}
    void setLFORate (float value)       {FX.reset(); FX.setLFORate(value);}
    void setLFODepth (float value)      {FX.reset(); FX.setLFODepth(value);}
    void setSampleRate ()               {FX.setSampleRate(getSampleRate());}

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

    Effect<float> FX;
    juce::String FXType = "None";
};


//==============================================================================
class SynthAudioSource   :  public juce::AudioSource
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


    void setCarrierAmplitude(float value)       {synth.setCarrierAmplitude(value);}
    void setCarrierAttackTime(float value)      {synth.setCarrierAttackTime(value);}
    void setCarrierDecayTime(float value)       {synth.setCarrierDecayTime(value);}
    void setCarrierSustainLevel(float value)    {synth.setCarrierSustainLevel(value);}
    void setCarrierReleaseTime(float value)     {synth.setCarrierReleaseTime(value);}
    void setModulatorAmplitude(float value)     {synth.setModulatorAmplitude(value);}
    void setModulatorFreqRatio(float value)     {synth.setModulatorFreqRatio(value);}
    void setModulatorAttackTime(float value)    {synth.setModulatorAttackTime(value);}
    void setModulatorDecayTime(float value)     {synth.setModulatorDecayTime(value);}
    void setModulatorSustainLevel(float value)  {synth.setModulatorSustainLevel(value);}
    void setModulatorReleaseTime(float value)   {synth.setModulatorReleaseTime(value);}

    void setFXType (juce::String value) {synth.setFXType(value);}
    void setFeedback (float value)      {synth.setFeedback(value);}
    void setDelayTime (float value)     {synth.setDelayTime(value);}
    void setWetDry (float value)        {synth.setWetDry(value);}
    void setLFORate (float value)       {synth.setLFORate(value);}
    void setLFODepth (float value)      {synth.setLFODepth(value);}
    void setSampleRate ()               {synth.setSampleRate();}

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
        // Problem #0 ////////////////////////////////////////////////////////////////////////////////////////////////
        // Replace this block with your Homework #3 solution. ////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        else
        {
            presetCarrierAmplitude      = 1.0f;
            presetCarrierAttackTime     = 0.01f;
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
        
        setFMParameters(  presetCarrierAmplitude,
                        presetCarrierAttackTime, presetCarrierDecayTime,
                        presetCarrierSustainLevel, presetCarrierReleaseTime,
                        presetModulatorAmplitude, presetModulatorFreqRatio,
                        presetModulatorAttackTime, presetModulatorDecayTime,
                        presetModulatorSustainLevel, presetModulatorReleaseTime );
    }

    void setFMParameters(
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

    void loadFX(juce::String name)
    {
        float feedback, delaytime, wetdry, lforate, lfodepth;
        if (name == "Delay")
        {
            feedbackSlider.setEnabled(true);
            delayTimeSlider.setEnabled(true);
            wetDrySlider.setEnabled(true);
            LFORateSlider.setEnabled(false);
            LFODepthSlider.setEnabled(false);
            feedback = 0.5f;
            delaytime = 0.3f;
            wetdry = 0.5f;
            lforate = 0.0f;
            lfodepth = 0.0f;
        }
        else if (name == "Chorus")
        {
            feedbackSlider.setEnabled(false);
            delayTimeSlider.setEnabled(true);
            wetDrySlider.setEnabled(true);
            LFORateSlider.setEnabled(true);
            LFODepthSlider.setEnabled(true);
            feedback = 0.0f;
            delaytime = 0.1f;
            wetdry = 0.5f;
            lforate = 2.0f;
            lfodepth = 0.0005f;
        }
        else if (name == "Flanger")
        {
            feedbackSlider.setEnabled(false);
            delayTimeSlider.setEnabled(true);
            wetDrySlider.setEnabled(true);
            LFORateSlider.setEnabled(true);
            LFODepthSlider.setEnabled(true);
            feedback = 0.0f;
            delaytime = 0.02f;
            wetdry = 0.5f;
            lforate = 0.4f;
            lfodepth = 0.001f;
        }
        else
        {
            feedbackSlider.setEnabled(false);
            delayTimeSlider.setEnabled(false);
            wetDrySlider.setEnabled(false);
            LFORateSlider.setEnabled(false);
            LFODepthSlider.setEnabled(false);
            feedback = 0.0f;
            delaytime = 0.0f;
            wetdry = 0.0f;
            lforate = 0.0f;
            lfodepth = 0.0f;
        }
        synthAudioSource.setFXType(name);
        synthAudioSource.setFeedback(feedback);
        synthAudioSource.setDelayTime(delaytime);
        synthAudioSource.setWetDry(wetdry);
        synthAudioSource.setLFORate(lforate);
        synthAudioSource.setLFODepth(lfodepth);

        feedbackSlider.setValue (feedback, juce::dontSendNotification); 
        delayTimeSlider.setValue (delaytime, juce::dontSendNotification); 
        wetDrySlider.setValue (wetdry, juce::dontSendNotification); 
        LFORateSlider.setValue (lforate, juce::dontSendNotification); 
        LFODepthSlider.setValue (lfodepth, juce::dontSendNotification); 
    }

    MainContentComponent()
        : synthAudioSource  (keyboardState),
          keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        synthAudioSource.setSampleRate();

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
        carrierSustainLevelSlider.setRange (0.01, 1, 0.01);
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

        addAndMakeVisible (feedbackSlider);
        feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        feedbackSlider.setRange (0.0, 1.0, 0.01);
        feedbackSlider.setValue (0.0, juce::dontSendNotification);
        feedbackSlider.onValueChange = [this] { synthAudioSource.setFeedback(feedbackSlider.getValue()); };
        feedbackSlider.setEnabled(false);

        addAndMakeVisible (delayTimeSlider);
        delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        delayTimeSlider.setRange (0.0, 1.0, 0.001);
        delayTimeSlider.setValue (0.0, juce::dontSendNotification);
        delayTimeSlider.onValueChange = [this] { synthAudioSource.setDelayTime(delayTimeSlider.getValue()); };
        delayTimeSlider.setEnabled(false);

        addAndMakeVisible (wetDrySlider);
        wetDrySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        wetDrySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        wetDrySlider.setRange (0.0, 1.0, 0.01);
        wetDrySlider.setValue (0.0, juce::dontSendNotification);
        wetDrySlider.onValueChange = [this] { synthAudioSource.setWetDry(wetDrySlider.getValue()); };
        wetDrySlider.setEnabled(false);

        addAndMakeVisible (LFORateSlider);
        LFORateSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        LFORateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        LFORateSlider.setRange (0.0, 10.0, 0.01);
        LFORateSlider.setValue (0.0, juce::dontSendNotification);
        LFORateSlider.onValueChange = [this] { synthAudioSource.setLFORate(LFORateSlider.getValue()); };
        addAndMakeVisible (LFODepthSlider);
        LFODepthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        LFODepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
        LFODepthSlider.setRange (0.0, 0.002, 0.00001);
        LFODepthSlider.setValue (0.0, juce::dontSendNotification);
        LFODepthSlider.onValueChange = [this] { synthAudioSource.setLFODepth(LFODepthSlider.getValue()); };
        LFODepthSlider.setEnabled(false);

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
        fxLabel                     .setText("FX Parameters", juce::dontSendNotification);
        fxListLabel                 .setText("FX", juce::dontSendNotification);
        feedbackLabel               .setText("Feedback", juce::dontSendNotification);
        delayTimeLabel              .setText("Delay Time [s]", juce::dontSendNotification);
        wetDryLabel                 .setText("Wet/Dry", juce::dontSendNotification);
        LFORateLabel                .setText("LFO Rate [Hz]", juce::dontSendNotification);
        LFODepthLabel               .setText("LFO Depth", juce::dontSendNotification);

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
        fxLabel                     .setJustificationType(juce::Justification::centred);
        fxListLabel                 .setJustificationType(juce::Justification::centredRight);
        feedbackLabel               .setJustificationType(juce::Justification::centred);
        delayTimeLabel              .setJustificationType(juce::Justification::centred);
        wetDryLabel                 .setJustificationType(juce::Justification::centred);
        LFORateLabel                .setJustificationType(juce::Justification::centred);
        LFODepthLabel               .setJustificationType(juce::Justification::centred);

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
        addAndMakeVisible (fxListLabel);

        addAndMakeVisible (fxLabel);
        addAndMakeVisible (feedbackLabel);
        addAndMakeVisible (delayTimeLabel);
        addAndMakeVisible (wetDryLabel);
        addAndMakeVisible (LFORateLabel);
        addAndMakeVisible (LFODepthLabel);
        
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
        
        addAndMakeVisible (fxList);
        juce::StringArray fxNames;
        fxNames.add("None");
        fxNames.add("Delay");
        fxNames.add("Chorus");
        fxNames.add("Flanger");
        fxList.addItemList( fxNames, 1 );
        fxList.setSelectedItemIndex(0);
        fxList.onChange = [this] { loadFX (fxList.getItemText(fxList.getSelectedItemIndex())); synthAudioSource.setSampleRate(); };

        setAudioChannels (0, 2);
        setSize(820, 430);
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

        fxLabel             .setBounds (0, borderTop+labelHeight+dialHeight+10, 410, 20);
        fxListLabel         .setBounds (410, borderTop+labelHeight+dialHeight+10, 100,  20);
        fxList              .setBounds (515, borderTop+labelHeight+dialHeight+10, 140, 20);
        feedbackLabel       .setBounds (borderLeft+dialWidth*1, borderTop+labelHeight+dialHeight+35, 140, 20);
        delayTimeLabel      .setBounds (borderLeft+dialWidth*3, borderTop+labelHeight+dialHeight+35, 140, 20);
        wetDryLabel         .setBounds (borderLeft+dialWidth*5, borderTop+labelHeight+dialHeight+35, 140, 20);
        LFORateLabel        .setBounds (borderLeft+dialWidth*7, borderTop+labelHeight+dialHeight+35, 140, 20);
        LFODepthLabel       .setBounds (borderLeft+dialWidth*9, borderTop+labelHeight+dialHeight+35, 140, 20);

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
        
        feedbackSlider              .setBounds (borderLeft+dialWidth*1+35, borderTop+labelHeight+dialHeight+50, dialWidth, dialHeight);
        delayTimeSlider             .setBounds (borderLeft+dialWidth*3+35, borderTop+labelHeight+dialHeight+50, dialWidth, dialHeight);
        wetDrySlider                .setBounds (borderLeft+dialWidth*5+35, borderTop+labelHeight+dialHeight+50, dialWidth, dialHeight);
        LFORateSlider               .setBounds (borderLeft+dialWidth*7+35, borderTop+labelHeight+dialHeight+50, dialWidth, dialHeight);
        LFODepthSlider              .setBounds (borderLeft+dialWidth*9+35, borderTop+labelHeight+dialHeight+50, dialWidth, dialHeight);

        keyboardComponent           .setBounds (borderLeft, 250, 800, 150);

        titleLabel                  .setBounds ( 30,  405, 300, 20);
        presetListLabel             .setBounds ( 595, 405, 80,  20);
        presetList                  .setBounds ( 665, 405, 120, 20);
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        synthAudioSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
        synthAudioSource.setSampleRate();
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

    juce::Label fxLabel;
    juce::Label feedbackLabel;
    juce::Label delayTimeLabel;
    juce::Label wetDryLabel;
    juce::Label LFORateLabel;
    juce::Label LFODepthLabel;
    juce::Slider feedbackSlider;
    juce::Slider delayTimeSlider;
    juce::Slider wetDrySlider;
    juce::Slider LFORateSlider;
    juce::Slider LFODepthSlider;

    juce::Label fxListLabel;
    juce::ComboBox fxList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
