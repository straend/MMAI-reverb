# Milestone 1 - Week 15

## How are you going to implement a reverb?

### Interface
* Supported OS
    * Linux
    * Mac OS
    * Windows
* Software
    * Sublime text/X code for Mac OS
    * Geany/Eclipse for Linux
    * Eclipse for Windows

### Implementation
* Using **C**
* Delay networks
   [Simulated using delay lines, filters and feedback connections]
* Moorer based
    - lowpass filter in the feedback
    - dampens higher frequencies more
    - more natural sound
    - "more comb filters > more better reverberation"
    - gives a warmer sound
* Different decay times for left and right for a more natural sound
* (Replacing the Pararell Combs with a Feedback Delay Network?)

```
                                                         ┌────┐
┌─────────┐                                              │    │
│  Sound  │─┬───────────────────────────────────────────▶│    │
└─────────┘ │                                            │    │
            │                                            │    │
            │                                            │ M  │
            │  ┌─────────┐                               │ i  │
            ├─▶│ DELAYS  │───────────Early Reflections──▶│ x  │
            │  └─────────┘                               │ e  │
            │                                            │ r  │
            │                                            │    │
            │                                            │    │
            │  ┌──────┐  ┌─────────┐                     │    │
            └─▶│Delay │─▶│ Moorer  │───Late Reverb──────▶│    │
               └──────┘  └─────────┘                     └────┘
```
### Moorer design
![Moorer design](https://christianfloisand.files.wordpress.com/2012/10/moorerdesign1.png)

## Group organization
* Tasks
> Using [Issues](https://github.com/straend/MMAI-reverb/issues)
* Organization
> See the assignment of [issues](https://github.com/straend/MMAI-reverb/issues) and the [schedule](https://github.com/straend/MMAI-reverb/wiki/Schedule) section in the wiki part
* Schedule
> In the [schedule](https://github.com/straend/MMAI-reverb/wiki/Schedule) section of the wiki part are displayed times of internal meetings 
and the [schedule](https://github.com/straend/MMAI-reverb/wiki/Schedule) of our internal tasks' deadlines

