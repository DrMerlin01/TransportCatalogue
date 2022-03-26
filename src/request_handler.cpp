#include "../inc/request_handler.h"

using namespace std;

namespace transport_catalogue {

	RequestHandler::RequestHandler(TransportCatalogue& db, renderer::MapRenderer& renderer, transport_router::TransportRouter& router) 
		: db_(db)
		, renderer_(renderer)
		, router_(router) {
	}

	optional<response::Stop> RequestHandler::GetStopResponse(const string_view stop_name) const {
		const domain::Stop* stop_from_db = db_.GetStop(stop_name);
		if (stop_from_db != nullptr) {
			return response::Stop{stop_from_db->name, db_.GetBusesThroughStop(stop_name)};
		} else {
			return nullopt;
		}
	}

	optional<response::Bus> RequestHandler::GetBusResponse(const string_view bus_name) const {
		const domain::Bus* bus_from_db = db_.GetBus(bus_name);
		if (bus_from_db != nullptr) {
			return response::Bus{bus_from_db->name, db_.GetInfoOnRoute(bus_name)};
		} else {
			return nullopt;
		}
	}

	svg::Document RequestHandler::RenderMap() const {
		return renderer_.RenderMap();
	}

	std::optional<response::Route> RequestHandler::GetRouteResponse(const std::string_view from, const std::string_view to) const {
		return router_.GetRouteResponse(from, to);
	}
}