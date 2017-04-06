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
    - gives a varmer sound
* Different decay times for left and right for a more naturla sound
* (Replacing the Pararell Combs with a FDN?)

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

## Group organization
* Tasks
* Organization
* Schedule

topic | meeting time
------|-------------
First meeting | Tuesday 7pm to 9pm
