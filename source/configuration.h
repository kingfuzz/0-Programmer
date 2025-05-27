#pragma once

/* This file containas configurations for parameters. We'll change this to something
 * like YAML later - soon, I promise!
 */

 // TODO: Move all configuration (incl. controllable parameters) to file.
#define MIDI_CHANNEL 1

// Play Modes
#define ENABLE_ARP_NAME         "EnableArp"
#define ENABLE_ARP_CC           117
#define ENABLE_ARP_VALUE        0
#define ENABLE_ARP_MIN_VALUE    0
#define ENABLE_ARP_MAX_VALUE    1

#define ARP_TYPE_NAME           "ArpType"
#define ARP_TYPE_CC             119
#define ARP_TYPE_VALUE          0
#define ARP_TYPE_MIN_VALUE      0
#define ARP_TYPE_MAX_VALUE      1

#define PORTAMENTO_NAME         "Portamento"
#define PORTAMENTO_CC           5
#define PORTAMENTO_VALUE        0
#define PORTAMENTO_MIN_VALUE    0
#define PORTAMENTO_MAX_VALUE    127

#define ENABLE_LEGATO_NAME        "EnableLegato"
#define ENABLE_LEGATO_CC          118
#define ENABLE_LEGATO_VALUE       0
#define ENABLE_LEGATO_MIN_VALUE   0
#define ENABLE_LEGATO_MAX_VALUE   1

// Clocks
#define ENABLE_MIDI_CLK_NAME        "EnableMidiClk"
#define ENABLE_MIDI_CLK_CC          114
#define ENABLE_MIDI_CLK_VALUE       0
#define ENABLE_MIDI_CLK_MIN_VALUE   0
#define ENABLE_MIDI_CLK_MAX_VALUE   1

#define TEMPO_IN_DIV_NAME        "TempoInDiv"
#define TEMPO_IN_DIV_CC          116
#define TEMPO_IN_DIV_VALUE       1
#define TEMPO_IN_DIV_MIN_VALUE   1
#define TEMPO_IN_DIV_MAX_VALUE   127

// MIDI A
#define MIDI_A_CHANNEL_NAME        "MidiAChannel"
#define MIDI_A_CHANNEL_CC          102
#define MIDI_A_CHANNEL_VALUE       0
#define MIDI_A_CHANNEL_MIN_VALUE   0
#define MIDI_A_CHANNEL_MAX_VALUE   16

#define MIDI_A_CV_NAME           "MidiACV"
#define MIDI_A_CV_CC             104
#define MIDI_A_CV_VALUE          0
#define MIDI_A_CV_MIN_VALUE      0
#define MIDI_A_CV_MAX_VALUE      3

#define MIDI_A_GATE_NAME         "MidiAGate"
#define MIDI_A_GATE_CC           106
#define MIDI_A_GATE_VALUE        0
#define MIDI_A_GATE_MIN_VALUE    0
#define MIDI_A_GATE_MAX_VALUE    3

#define MIDI_A_PITCH_NAME       "MidiAPitchScale"
#define MIDI_A_PITCH_CC         108
#define MIDI_A_PITCH_VALUE      0
#define MIDI_A_PITCH_MIN_VALUE  0
#define MIDI_A_PITCH_MAX_VALUE  127

#define MIDI_A_AFTERTOUCH_NAME  "MidiAAftertouchScale"
#define MIDI_A_AFTERTOUCH_CC    110
#define MIDI_A_AFTERTOUCH_VALUE 0
#define MIDI_A_AFTERTOUCH_MIN_VALUE 0
#define MIDI_A_AFTERTOUCH_MAX_VALUE 127

#define MIDI_A_VELOCITY_NAME  "MidiAVelocityScale"
#define MIDI_A_VELOCITY_CC    112
#define MIDI_A_VELOCITY_VALUE 0
#define MIDI_A_VELOCITY_MIN_VALUE 0
#define MIDI_A_VELOCITY_MAX_VALUE 127

// MIDI B
#define MIDI_B_CHANNEL_NAME        "MidiBChannel"
#define MIDI_B_CHANNEL_CC          103
#define MIDI_B_CHANNEL_VALUE       0
#define MIDI_B_CHANNEL_MIN_VALUE   0
#define MIDI_B_CHANNEL_MAX_VALUE   16

#define MIDI_B_CV_NAME           "MidiBCV"
#define MIDI_B_CV_CC             105
#define MIDI_B_CV_VALUE          0
#define MIDI_B_CV_MIN_VALUE      0
#define MIDI_B_CV_MAX_VALUE      3

#define MIDI_B_GATE_NAME         "MidiBGate"
#define MIDI_B_GATE_CC           107
#define MIDI_B_GATE_VALUE        0
#define MIDI_B_GATE_MIN_VALUE    0
#define MIDI_B_GATE_MAX_VALUE    3

#define MIDI_B_PITCH_NAME       "MidiBPitchScale"
#define MIDI_B_PITCH_CC         109
#define MIDI_B_PITCH_VALUE      0
#define MIDI_B_PITCH_MIN_VALUE  0
#define MIDI_B_PITCH_MAX_VALUE  127

#define MIDI_B_AFTERTOUCH_NAME  "MidiBAftertouchScale"
#define MIDI_B_AFTERTOUCH_CC    111
#define MIDI_B_AFTERTOUCH_VALUE 0
#define MIDI_B_AFTERTOUCH_MIN_VALUE 0
#define MIDI_B_AFTERTOUCH_MAX_VALUE 127

#define MIDI_B_VELOCITY_NAME  "MidiBVelocityScale"
#define MIDI_B_VELOCITY_CC    113
#define MIDI_B_VELOCITY_VALUE 0
#define MIDI_B_VELOCITY_MIN_VALUE 0
#define MIDI_B_VELOCITY_MAX_VALUE 127