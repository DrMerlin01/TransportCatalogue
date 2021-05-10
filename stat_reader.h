#pragma once

#include <iostream>
#include "transport_catalogue.h"

namespace transport_catalogue {
	namespace stat_reader {
		std::ostream& operator<<(std::ostream& out, const content::InfoOnRoute& info);
		void PrintInfoOnBus(const TransportCatalogue& catalogue, const std::string& bus, std::ostream& out);
		void PrintInfoOnStop(const TransportCatalogue& catalogue, const std::string& stop, std::ostream& out);
		void GetInfoOnRoutes(const TransportCatalogue& catalogue, std::istream& in, std::ostream& out);
	}
}