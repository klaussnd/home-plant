#pragma once

#include <can.h>
#include <can_message/content_plant.h>

CanMsg::PlantMeasSettings readInitialSettingsFromEeprom();
void handleSettings(const CanMessage& msg, CanMsg::PlantMeasSettings& settings);
