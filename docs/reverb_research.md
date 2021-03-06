# Research about reverb

## Why to choose a reverb of Schroeder?
*SEAN COSTELLO. Shroeder Reverbs: The Forgotten Algorithm. 
https://valhalladsp.com/2009/05/30/schroeder-reverbs-the-forgotten-algorithm/
May 30, 2009*

"In general, Schroeder refers to 4 or more parallel comb filters (delay lines with feedback) 
of unequal length, with the outputs summed and run through two or more allpass delays.
Such reverbs often sound bad: ringing, metallic, tec.
2 algorithms are described by Schroeder:
* the paralle comb filters into series allpasses
* series allpass delays 
	* reverberator using 5 allpass delays in series. It provides a less-colored sound.
	
Schroeder describes a way of mixing the input and output of the series allpass reverberator -> resulting sound
is allpass. 
Algo: single delay line, feeding into 5 series allpass delays and an allpas feedback around the structure.
This reverberator have a non-exponential decay. 
It increases the outer allpass -> the echo increases with time, the reverb time is longer.
This is a much more natural decay. Allpasses inside of delayed feedback loops is fundamental."

1962 AES Paper, “Natural Sounding Artificial Reverberation
http://charlesames.net/pdf/MRSchroeder/artificial-reverb.pdf

## Why to choose a Moorer reverb?
Good response with less computations due to the allpass filters used in the feedback paths.
reverb.pdf

## How to create a reverb?
*Julius O. Smith III. MUS420/EE367A Lecture 3 Artificial Reverberation and Spatialization.
https://ccrma.stanford.edu/~jos/Reverb/
2014-03-24*

"Early reflections are sparse and span a short time, tapped delay lines (TDL) are often used to 
implement it. They have a strong influence on spatial impression. But it can be cost-effective to feed 
the "late reverb" unit from an earlier tap from the TDL (overlapping).
Often, they are worked into the late-reverberation simulation. A transposed tapped delay line is then implemented.

Late reverberation has to have a smooth decay and a smooth frequency reponse of impulse reponse. 
10,000 echoes/s or more are necessary for a smooth reponse."

*http://dsp.stackexchange.com/questions/2792/high-quality-reverb-algorithm*
"Feedback delay networks are definitely the way to go. The original Schroeder algorithms with all passes 
and comb filters suffers from "spectral thinning" which makes it metallic sounding. You need to dial in 
frequency dependent attenuation on the different delay lines that's representative of the reverb time (as a function of frequency). Early reflections can be done through a tapped delay line also with some frequency dependent attenuation and a few diffusers to de-correlate them. Doing stereo also requires some means of de-correlation."

### Digital reverberation
Basic steps to create a digital reverberation with suggested data to simulate a concert hall from 
Dodge & Jerse, “Computer Music”.
https://christianfloisand.wordpress.com/2012/09/04/digital-reverberation/

### The good question
*Do I want a natural, reality based sound to emulate listening in a real room?  
What size and kind of room?*
*Or do I want to use reverb more as a noticeable effect and ear candy for the listener?*

### Different types of reverbs
*MIXING & MASTERING. Mixing with Reverb: How to Use Reverb for Depth Without Creating a Mess.
http://ledgernote.com/columns/mixing-mastering/mixing-with-reverb/
April 26, 2016*
* Room
* Hall
* Chamber
* Spring 
* Plate

### Why use the Room reverb?
* Description
	* Simulates ambient rooms of square structure and low ceilings.  
Usually most effective with short RT60 times and can contribute to the clarity and presence of an individual instrument or a mix.  
Effective use can "lift" an instrument out of a dense mix better than simply turning it up.  
Variations of small, medium, large refer to the size of the emulated room and are independent of RT60 time.  
Best used as a subtle effect.
* Uses
	* Snares, marching drums, percussion, drum kits, vocals, piano, acoustic guitar, ethnic ensembles, strings.  
Inherent high diffusion may make signals with fast transients (percussion, piano, guitar) sound grainy at long RT60 times.

**realistic, natural and lots of different sounds can be tested**
http://www.retrosynth.com/~analoguediehard/studio/effects/digitalreverb.html 

### Parameters
#### Dry/Wet
Gains for effect(reverb) output called *Wet*, and gain for the original signal called *Dry*

#### Early reflections
Important to be able to identify room size quickly.

#### Late reverberation size
Length of the reverberation

#### Damping/EQ
Higher frequencies "dies" earlier than lower frequencies, correctly implemented will make the reverberation sound more real.
"Generally speaking, damping is expressed as a ratio. For example, a damping of 1/2 on a two second reverb means the high frequencies disappear after just one second, while the rest of the signal extends for another second."
" In that case you could use a shelving filter in the low end to emphasize the sound's warmth as well as to create a larger sense of space. Boosting the highs on the other hand will produce a flashy surreal effect. Be careful not to be excessive with it or you risk drowning your mix in a reverb mush."

http://wiki.audacityteam.org/wiki/Principles_of_Reverb
 
http://en.audiofanzine.com/getting-started/editorial/articles/using-reverb-step-by-step.html

## Random Notes
Delay values should not interfere with each other ie. be multiplies of another one, -> use prime numbers 
  
Slight variations on delay/decay time for left and right channel

(RayTracing reverb..)

Combination of Moorer and Schroeder
http://arqen.com/wp-content/docs/Hybrid-Convolution-Algorithmic-Reverb.pdf
