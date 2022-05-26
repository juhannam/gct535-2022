# Homework #4: Delay-based Effects (Due June 6)

### Instruction
The problem includes C++ programming on JUCE framework. Follow the instruction, modify the given template code, implement the components using an editor convenient for you (e.g. VIM, VSCode).  

### Deliverable 
You should submit this project directory, compressed in a single zip file.
Please submit it via KLMS.


## Problem \#0: Previously... (10 pts.)
```
float getADSRCurve( float attackTime, float decayTime,
                    float sustainLevel, float releaseTime,
                    bool isRelease, float currentLevel)
    {
        ...
        return ADSR;
    }
```
```
float getCurrentSample( float carrierAmplitude, float carrierAttackTime,
                        float carrierDecayTime, float carrierSustainLevel,
                        float carrierReleaseTime, float modulatorAmplitude,
                        float modulatorFreqRatio, float modulatorAttackTime,
                        float modulatorDecayTime, float modulatorSustainLevel,
                        float modulatorReleaseTime, bool isRelease)
    {
        ...
        return Sample;
    }
```
```
void loadPreset(juce::String name)
    {
        ...
    }
```
Replace getADSRCurve, getCurrentSample, loadPreset functions with your Homework #3 solution.


## Problem \#1: Delay  (30 pts.)
<img width="448" alt="Fig1_Delay" src="https://user-images.githubusercontent.com/16303932/170556449-48eeddec-3742-4b27-975e-747ddd101454.png">

```
void process (const ProcessContext& context) noexcept
{    
        ...
        if (FXType == "Delay")
        {    
            for (size_t ch = 0; ch < numChannels; ++ch)
            {    
                ...
            }    
        }    
```

Implement the delay effect by modifying the given code. Please refer our practice code '09. Delay-based Audio Effects.ipynb'.

## Problem \#2: Chorus (30 pts.)
<img width="448" alt="Fig2_Chorus" src="https://user-images.githubusercontent.com/16303932/170556458-d21f90ef-d011-4206-b8f5-4c1fd77149c8.png">

```
void process (const ProcessContext& context) noexcept
{    
        ...
        else if (FXType == "Chorus")
        {    
            for (size_t ch = 0; ch < numChannels; ++ch)
            {    
                ...
            }    
        }    
```

Implement the chorus effect by modifying the given code. Please refer our practice code '09. Delay-based Audio Effects.ipynb'.

## Problem \#3: Flanger (30 pts.)
<img width="448" alt="Fig3_Flanger" src="https://user-images.githubusercontent.com/16303932/170556465-5c560768-e02e-4c26-a94f-929975addc51.png">

```
void process (const ProcessContext& context) noexcept
{    
        ...
        else if (FXType == "Chorus")
        {    
            for (size_t ch = 0; ch < numChannels; ++ch)
            {    
                ...
            }    
        }    
```

Implement the flanger effect by modifying the given code. Please refer our practice code '09. Delay-based Audio Effects.ipynb'.


