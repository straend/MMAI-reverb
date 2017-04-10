# MMAI-reverb [![Build Status](https://travis-ci.org/straend/MMAI-reverb.svg?branch=master)](https://travis-ci.org/straend/MMAI-reverb)

## Purpose:
Implement a reverb algorithm in C/C++.

## What should the project do:
* Our program has to:
    * read wav files
    * play them in real time withe reverberation effect on it
* The implementation should have controls for:
    * a dry-wet
    * early reflections
    * a late reverberation size
    * a spectral control (damping or equalizer)
* Our program has to:
    * have a CLI
    * be runnable on at least Linux
* Libraries that have to be used for sound file and audio I/O:
    * libsndfile
    * portaudio
    
## Milestones
* Week 15: [project plan](https://github.com/straend/MMAI-reverb/blob/master/docs/milestone1.md)
   * What have we researched about the reverbs?
      * [research](https://github.com/straend/MMAI-reverb/blob/master/docs/reverb_research.md)
   * How will we implement a reverb?
    * interface description
    * base algorithm
  * How will we share the work between us?
    * tasks
    * responsability
    * schedule   
 * Week 16: first version of the reverberation
    * working algorithm
       * can be non real-time and/ or non cross-platform
    * the executable has to be uploaded to GitHub's releases
       * this for at least 3 major operating systems
 * Week 17: reverb with fine-tuned basic control parameters
    * an executable with CLI controls for the reverbs
    * almost final version of the DSP
 * Week 18: a real-time reverb with UI
    * functions included:
      * read wav-file
      * trigger playback with UI or a hotkey
      * save result to a file
  * Week 19: debugging meeting
    * Monday:
      * last session
      * listening test of the reverb
      * submission of the project online one day before the course presentation
    * Thursday:
      * address the rubric topics as best as possible
      * explain:
        * Why were different algorithm blocks used?
        * Quality of the presentation:
          * structure
          * content
        * team work
          * Who was responsible for what?
          * collaborative programming organization

## Expected content of the repository:
  * building instructions
  * signs of activity
    * comments
    * issues
    * task tracking
    
## What we have to do to get the maximum grade:
Criterion | 5 cr.
--------- | ------
Reverberation implementation (50%) | Textbook algorithm from lecture notes. The app can process sounds from file and control parameters from  the GUI. The students experimented with the textbook algorithms and implemented a variation of a well-known one. The modifications are thought-off and the algorithm is fine-tuned. The students researched the reverberation problem by themselves and implemented the algorithm not mentioned in lecture notes, or introduced modifications that were not shown during the lectures, or made other creative additions to the lecture materials.
Presentation (20%) | The project was presented. The presentation has clear structure and provides sufficient background and information to understand how the reverberator was built. The presenter has shown good oral presentation skills.
Code (20%) | Code just works. The code is nicely organized, has consistent coding style matching idioms. The code has zero Coverity Scan issues and has Travis CI automatic build set up.
Team work (10%) | A team was able to deliver MVPs for each milestone on time. Each student's role in a group is clear and a team was. The team have organized their GitHub repo and used it extensively to manage tasks and milestones.
