# Research about reverb

## First reference
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


## Second reference
*Julius O. Smith III. MUS420/EE367A Lecture 3 Artificial Reverberation and Spatialization.
https://ccrma.stanford.edu/~jos/Reverb/
2014-03-24*

"Early reflections are sparse and span a short time, tapped delay lines (TDL) are often used to 
implement it. They have a strong influence on spatial impression. But it can be cost-effective to feed 
the "late reverb" unit from an earlier tap from the TDL (overlapping).
Often, they are worked into the late-reverberation simulation. A transposed tapped delay line is then implemented.

Late reverberation has to have a smooth decay and a smooth frequency reponse of impulse reponse. 
10,000 echoes/s or more are necessary for a smooth reponse."


### Parameters
#### Dry/Wet
Gains for effect(reverb) output called *Wet*, and gain for the original signal called *Dry*

#### Early reflections
Important to be able to identify room size quickly.

#### Late reverberation size
Length of the reverberation

#### Damping/EQ
Higher frequencies "dies" earlier than lower frequencies, correctly implemented will make the reverberation sound more real.

http://wiki.audacityteam.org/wiki/Principles_of_Reverb
 
http://en.audiofanzine.com/getting-started/editorial/articles/using-reverb-step-by-step.html
