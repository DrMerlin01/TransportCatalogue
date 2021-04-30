#pragma once

#include "geo.h"
#include <string>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <vector>
#include <set>

struct InfoOnRoute {
    size_t count_bus_stop;
    size_t count_unique_bus_stop;
    double lenght_route;
};

class TransportCatalogue {
private:
    struct Stop {
        std::string name;
        Coordinates coordinates;
    };
    struct Bus {
        std::string name;
        bool is_circular;
        std::vector<std::string_view> route;
    };
    std::deque<Bus> buses_;
    std::deque<Stop> bus_stops_;
    std::unordered_map<std::string_view, const Stop*> stop_by_name_;
    std::unordered_map<std::string_view, const Bus*> bus_by_name_;
    
    double GetDistanceBetweenStops(std::string_view from, std::string_view to) const;
public:
    void AddBus(const std::string& name, const std::vector<std::string_view>& bus_stops, bool is_circular);
    
    void AddStop(const std::string& name, const Coordinates coords);
    
    const Stop* GetStop(std::string_view name) const;
    
    const Bus* GetBus(std::string_view name) const;
    
    InfoOnRoute GetInfoOnRoute(std::string_view name) const;
};