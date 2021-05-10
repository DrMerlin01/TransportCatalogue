#pragma once

#include "transport_catalogue.h"
#include <unordered_map>
#include <istream>
#include <string>

namespace transport_catalogue {
	namespace input_reader {
		void ParseAndAddStop(TransportCatalogue& catalogue, std::string& stop, std::unordered_map<std::string, std::string>& stops);
		void ParseAdditionalInfoAboutStop(TransportCatalogue& catalogue, const std::string& first_stop, std::string& info);
		void ParseAndAddBus(TransportCatalogue& catalogue, std::string& bus);
		void ReadLineAndParseData(TransportCatalogue& catalogue, std::istream& in);
	}
}