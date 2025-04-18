# 0-Programmer

_NOTE: THIS IS A WORK IN PROGRESS - THE PROJECT HAS JUST STARTED, SO NOT MUCH IS BUILT YET_

0-Programmer is a simple application which allows you to edit the program pages of your Make Noise 0-Coast. The goal is to provide the functionality of the Make Noise Max4Live Page programmer, but in a standalone application. Simple as that :-)

The project has just started, so not much functionality exists yet. Also it looks like a dumpster fire, as no effort has been put into the UI yet.

But here's what we'll do first:

* Make it possible to send some MIDI data to an interface by pressing a button. **[DONE]**
* Make it possible to enable/disable the 0-Coast arpeggiator. **[DONE]**
* Add end-2-end integration test: Simulate button press, verify that correct message is in buffer. **[DONE]**

A quick note on controlling the 0-Coast. The application has no way of knowing the state of the 0-Coast, ie. it can't read data, only send. 0-Coast will save parameter changes, so some out-of.sync scenario between the programmwe and the device is likely to happen. So, how do we ensure sync? We could reset all parameters, when the app loads - but that could be annoying as well. A reset button?


Stuff to add:
* Initial User Guide
* Document general architecture (Here it would be nice to add some developer docs, etc. also)
* Proper GUI layout and graphics
* Store parameter configuration as YAML

This project is built using the fantastic [PAMPLEJUCE](https://github.com/sudara/pamplejuce) template.
