#pragma once

/* This file containas configurations for parameters. We'll change this to something
 * like YAML later - soon, I promise!
 */

 // TODO: Move all configuration (incl. controllable parameters) to file.
#define MIDI_CHANNEL 1

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
