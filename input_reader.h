#pragma once

#include "transport_catalogue.h"
#include <unordered_map>

namespace transport_catalogue {
	namespace detail {
		void ReadLineWithData(TransportCatalogue& cataloge);
	}
}