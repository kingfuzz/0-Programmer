# 0-Programmer

0-Programmer is a simple application which allows you to edit the program pages of your Make Noise 0-Coast. The goal is to provide the functionality of the Make Noise Max4Live Page programmer, but in a standalone application. Simple as that :-)

## Usage
Find and Download the app in the *Releases* section to the right :arrow_right: :arrow_right: :arrow_right:

To use the app, do as follows:
1. Connect your 0-Coast to a Midi Interface
2. Select that Midi Interface the 0-Programmer Options.
3. 

The project has just started, so not much functionality exists yet. Also it looks like a dumpster fire, as no effort has been put into the UI yet.

But here's what we'll do first:

* Make it possible to send some MIDI data to an interface by pressing a button. **[DONE]**
* Make it possible to enable/disable the 0-Coast arpeggiator. **[DONE]**
* Add end-2-end integration test: Simulate button press, verify that correct message is in buffer. **[DONE]**
* Test some initial parameters
  * Test vs MidiView **[DONE]**
  * Test vs Hardware **[DONE]**
* Clean up (CI, initial docs)
  * Make CI release unsigned builds **[DONE]**
  * Write ~~simple user guide~~ / or a help text in the UI? **[DONE]**
* Update to latest Melatonin Inspector to remove a bunch of deprecation warnings. **[DONE]**
* Initial (very limited) release _In Progress_

After these initial steps, we'll start to plan additional features/bugs using Githubs issue tracker and stop committing straight to main. But we need to have a stable main branch with some actual functionality first ;-)

A quick note on controlling the 0-Coast. The application has no way of knowing the state of the 0-Coast, ie. it can't read data, only send. 0-Coast will save parameter changes between power cycles, so some out-of-sync scenario between the programmer and the device is likely to happen. So, how do we ensure sync? We could reset all parameters, when the app loads - but that could be annoying as well. A reset button?

Stuff to add:
* Initial User Guide
  * Test to generate screenshot - see test_helpers.h
* Document general architecture (Here it would be nice to add some developer docs, etc. also)
  * Overall structure
  * Test suite overview
  * MidiView test and HW tests.
* Proper GUI layout and graphics
  * Automate/refactor a bunch of the GUI component code.
  * Column layout
  * Help text in column
* Store parameter configuration as YAML
* Add "Reset 0 Coast" Button
* Redo integration test using test_helpers.h
* Plugin version, Windows support, Linux support, iOS, etc..

Nice to have:
* A python simulator to visualize received commands - would simplify testing.


This project is built using the fantastic [PAMPLEJUCE](https://github.com/sudara/pamplejuce) template.
Releases are handled as in pamplejuce. ie: [Managing Releases](https://melatonin.dev/manuals/pamplejuce/life-with-pamplejuce/managing-releases/)

