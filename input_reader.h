#pragma once

#include "transport_catalogue.h"
#include <vector>
#include <string>

std::vector<std::string> SplitIntoWords(const std::string& text);
void ReadLineWithData(TransportCatalogue& cataloge);