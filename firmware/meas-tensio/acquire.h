#pragma once

#include <stdint.h>

/** Configures the ports */
void initAcquisition();
/** Acquires a raw tensiometer value
 *
 *  This function selects the MUX, reads out the value, and disables the MUX afterwards.
 *  It assumes that the analogue circuit has already been enabled.
 *
 *  @param channel  Mux channel (0 ... 3)
 */
uint16_t acquireRaw(uint8_t channel);

/** Enable the analogue circuit and sensors */
void enableAnalogue();
/** Disable the analogue circuit and sensors */
void disableAnalogue();
void muxSelectChannel(uint8_t channel);
void muxDeselectAllChannels();
/** Reads out a raw tensiometer value from the currently selected MUX channel.
 *  Assumes that the analogue circuit has already been enabled and the MUX selected.
 */
uint16_t readout();
