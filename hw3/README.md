# Homework #3: ADSR and FM Synthesizer (Due May 15)

### Instruction
The problem includes C++ programming on JUCE framework. Follow the instruction, modify the given template code, implement the components using an editor convenient for you (e.g. VIM, VSCode).  

### Deliverable 
You should submit this project directory, compressed in a single zip file.
Please submit it via KLMS.


## Problem \#1: Attack, Decay, Sustain and Release (35 pts.)
```
float getADSRCurve( float attackTime, float decayTime,
                    float sustainLevel, float releaseTime,
                    bool isRelease, float currentLevel)
    {
        ...
        return ADSR;
    }
```

Implement the function calculates ADSR value for current time.
1. Implement the attack curve linearly increases.
2. Implement the decay curve exponentially decreases.
3. Implement the release curve exponentially decreases. Consider the cases that MIDI note turned off before ADSR curve reaching to sustain level.

## Problem \#2: Frequancy Modulation (35 pts.)

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

Implement the FM synthesizer consist of two oscillators, one carrier and one modulator, with ADSR curves for each, as following equation.

<img width="987" alt="GCT535_Homework3_FMEquation" src="https://user-images.githubusercontent.com/16303932/166635793-dd79826a-71e9-4759-90fd-eed9b3415f22.png">

## Problem \#3: Bell, Brass and Electric Piano (30 pts.)

Adjust the input parameter values in source code for the presets.
Prepare presets for bell, brass and electric piano sound.
Please refer the practice code from our course material.
Build your source code, play with knobs, and listen to the sound to find proper parameters.

## Problem \#4: Your Own Sound (bonus up to 20 pts.)

Design your own sound.
Adjust the input parameter values in source code to make the preset for your sound.
You can try to reproduce the sounds of existing instrumentsm, or you can design the sounds to express something.
Please explain your sound by leaving comment in Problem #4 block in your source code.

