smartsdr-dsp
============

SmartSDR DSP API

SmartSDR-DSP contains the source code for a wrapper, as well as an implemented example of a modem for any signal processing block that could be attached to a SmartSDR radio.

The implemented example uses the CODEC2 digital voice coder and FreeDV HF Modem developed and released by David Rowe.

The intention is that the API, defined by example, in the wrapper could be used to attach a PSK-31 Modem, or a RTTY modem,
or any signalling system to the radio.  Provisions are included for multiple audio or I-Q baseband input or output streams.

Although originally intended for bi-directional signalling modems, the actual implementation could be expanded well beyond simple signalling to such things as noise-blankers or a skimmer, or any third party software that would benefit from baseband or audio stream access.

The API and the wrapper are "open" under GPL3, and therefore allow third party access to an otherwise "closed" software defined radio.

This interface, by using VITA-49 Ethernet packets for control, status and data transport enables an architecture where the
"MODEM" can be located inside a separate process inside the radio, on a separate processor attached to the network, or in a separate PC, particularly for development purposes.

For information on this and other APIs, see the FlexRadio web page at www.flexradio.com and the FlexRadio Community at community.flexradio.com

If you are totally lost, you may send an email at developer /at/ flex radio 0x2E com
