# Homework #2: Convolution, FIR Filter, IIR Filter, and Wind Sound Design (Due Apr 15)

### Instruction

The problem sets include short answers and Python programming. Organize your answers, figures and supporting comments using a word editor convenient for you (e.g., MS Word, Latex). You should use the Jupyter notebook for Python programming. 

### Deliverable 
You should submit two items: the report and .ipynb file. Please compress them as a single file (e.g. zip) and submit it via KLMS.

## Problem #1: Convolution (30 pts.)

1. You have an input signal x(n) = [1, 2, 3, 2] and feed it into an LTI system with the impulse response, h(n) = [3, -1, -2]. What is the output (i.e. convolution between x(n) and h(n))? (10pts.)


2. Implement your own convolution function and verify the result in 1) (10pts.)

```
def my_conv(x,h):
# x = input signal
# h = impulse response
# y = output signal

    ### Your Implementation of convolution ###

    return y 
```

3. Run “my_conv.m” with the following simple filters using the provided snare drum file, “Snare.wav” as an input. 

    - h1 = [0.2, 0.2, 0.2, 0.2, 0.2]
    - h2 = [1, -2, 1]

Submit the outputs as wav files with 16 bit and 44.1kHz sampling rate format (10pts.). 


## Problem #2: FIR Filter (20 pts.)

Suppose that you are given a moving-average filter, y(n) = 0.25x (n) + 0.25x (n-1) + 0.25x(n-2) + 0.25x(n-3).  

1. Compute the impulse response, h(n)
2. Compute the transfer function, H(z)
3. What are the zeros?  Display them on the z-plane.
4. Plot the amplitude response using the "freqz" function and discuss the result with zeros that you computed in 2).   

Hint1) Polynomial factorization:  1 - z^N = (1 - z)(1 + z^1 + z^2 + z^3 + … + z^(N-1))  
Hint2) The roots of the equation, z^N = 1, are obtained by uniformly dividing the unit circle into N pieces uniformly on the z-plane. 


## Problem #3: IIR Filter (20 pts.)
Suppose that you are given a bi-quad filter, y(n) =  x(n) – x(n-2) + 0.9y(n-1) - 0.81y(n-2)

1. Compute the transfer function, H(z)
2. What are the zeros and poles? Display them on the z-plane
3. Plot the amplitude response using the "freqz" function and discuss the result with poles and zeros that you computed.   


## Problem #4: Wind Sound Design (30 pts.)
We design a wind sound using a white noise and a resonant low-pass filter. 

1. Generate 2-sec white noise using the “np.rand” function. 
2. Take a segment of the white noise with a large window size (e.g. 4096 samples) and plot the FFT magnitude.
3. Implement a resonant filter using “signal.lfilter” (you can use the implementation from our practice code) and find an apropriate cut-off frequency and Q value to design a wind sound. Submit the output as a wav file with 16 bit and 44.1kHz sampling rate format. 
4. Plot the amplitude response of the resonant low-pass filter using the "freqz" function.
5. Take a segment of the wind sound with a large window size (e.g. 4096 samples) and plot the FFT magnitude.

## Problem #5: Design a question for midterm exam (bonus up to 20pts.)
This question is optional. But, if you design a good question for midterm exam, you will earn some bunus point. If it is adopted in the midterm exam, you will get 20 extra points! 



