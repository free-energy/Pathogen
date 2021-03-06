#define PLUG_MFR "AG Funds"
#define PLUG_NAME "PathogenSynth"

#define PLUG_CLASS_NAME PathogenSynth

#define BUNDLE_MFR "AG Funds"
#define BUNDLE_NAME "PathogenSynth"

#define PLUG_ENTRY PathogenSynth_Entry
#define PLUG_VIEW_ENTRY PathogenSynth_ViewEntry

#define PLUG_ENTRY_STR "PathogenSynth_Entry"
#define PLUG_VIEW_ENTRY_STR "PathogenSynth_ViewEntry"

#define VIEW_CLASS PathogenSynth_View
#define VIEW_CLASS_STR "PathogenSynth_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

#define PLUG_UNIQUE_ID 'Path'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'AGF'

// ProTools stuff
#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'PSN1', 'PSN2'};
#endif
#define PLUG_MFR_PT "AG Funds\nAG Funds\nAG Funds\n"
#define PLUG_NAME_PT "Pathogen\nPath"
#define PLUG_TYPE_PT "Effect" // valid options "None" "EQ" "Dynamics" "PitchShift" "Reverb" "Delay" "Modulation" "Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" instrument determined by PLUG _IS _INST
#define PLUG_DOES_AUDIOSUITE 0

#if (defined(AAX_API) || defined(RTAS_API)) 
#define PLUG_CHANNEL_IO "1-1 2-2"
#else
#define PLUG_CHANNEL_IO "0-1 0-2"
#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 1

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 1

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define KNOB_ID       101
#define BG_ID         102
#define ABOUTBOX_ID   103
#define WHITE_KEY_ID  104
#define BLACK_KEY_ID  105

#define OSC1_SELECT_ID 106
#define OSC1_SELECTUP_ID 107

#define OSC1_NORMALISE_ID 108

// Image resource locations for this plug.
#define KNOB_FN       "resources/img/knob.png"
#define BG_FN         "resources/img/PathogenBK.png"
#define ABOUTBOX_FN   "resources/img/about.png"
#define WHITE_KEY_FN  "resources/img/wk.png"
#define BLACK_KEY_FN  "resources/img/bk.png"

#define O1_SELECT_FN    "resources/img/Osc1SelectDown.png"
#define O1_SELECTUP_FN  "resources/img/Osc1SelectUp.png"

#define O1_NORMALISE_FN "resources/img/IRadioButtonsControl_x2.png"


// GUI default dimensions
#define GUI_WIDTH   900
#define GUI_HEIGHT  600

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "https://adriangin.wordpress.com/"
#define MFR_EMAIL "adrian.gin@gmail.com"
#define EFFECT_TYPE_VST3 "Instrument|Synth"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
