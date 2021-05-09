#pragma once

#include <iostream>
#include "transport_catalogue.h"

namespace transport_catalogue {
	namespace detail {
		std::ostream& operator<<(std::ostream& out, const content::InfoOnRoute& r);
		void GetInfoOnRoutes(TransportCatalogue& cataloge);
	}
}