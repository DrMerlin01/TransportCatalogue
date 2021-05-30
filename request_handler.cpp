#include "request_handler.h"

namespace transport_catalogue {
    using namespace std;
    
    RequestHandler::RequestHandler(TransportCatalogue& db/*, const renderer::MapRenderer& renderer*/) 
        : db_(db) {
    }

    void RequestHandler::FillingCatalogue(const ContentRequests& base_requests) {
        for(const auto& stop : base_requests.stops) {
            AddStop(stop);
        }
        for(const auto& stop : base_requests.stops) {
            SetDistanceBetweenStops(stop);
        }
        for(const auto& bus : base_requests.buses) {
            AddBus(bus);
        }
    }

    optional<response::Stop> RequestHandler::GetStopResponse(const string_view& stop_name) const {
        const domain::Stop* stop_from_db = db_.GetStop(stop_name);
        if (stop_from_db != nullptr) {
            return response::Stop{stop_from_db->name, db_.GetBusesThroughStop(stop_name)};
        } else {
            return nullopt;
        }
    }

    optional<response::Bus> RequestHandler::GetBusResponse(const string_view& bus_name) const {
        const domain::Bus* bus_from_db = db_.GetBus(bus_name);
        if (bus_from_db != nullptr) {
            return response::Bus{bus_from_db->name, db_.GetInfoOnRoute(bus_name)};
        } else {
            return nullopt;
        }
    }

    void RequestHandler::AddStop(const request::Stop& stop) {
        db_.AddStop(stop.name, stop.coordinates);
    }

    void RequestHandler::AddBus(const request::Bus& bus) {
        vector<string_view> bus_stops;
        for(const auto& stop_name: bus.bus_stops) {
            bus_stops.push_back(db_.GetStop(stop_name)->name);
        }
        db_.AddBus(bus.name, bus_stops, bus.is_roundtrip);
    }

    void RequestHandler::SetDistanceBetweenStops(const request::Stop& stop) {
        for(const auto [stop_to, distance] : stop.stop_to_dist) {
            db_.SetDistanceBetweenStops(stop.name, stop_to, distance);
        }
    }
}