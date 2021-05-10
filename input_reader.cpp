#include "input_reader.h"
#include "geo.h"
#include <unordered_set>
#include <string_view>
#include <stdlib.h>

using namespace std;

namespace transport_catalogue {
	namespace input_reader {
		void ParseAndAddStop(TransportCatalogue& catalogue, std::string& stop, unordered_map<std::string, std::string>& stops) {
			auto position = stop.find(":"s);
			const string stop_name = stop.substr(5, position - 5);
			stop = stop.substr(position + 2);
			position = stop.find(","s);
			const auto lat = stop.substr(0, position);
			stop = stop.substr(position + 2);
			position = stop.find(","s);
			string lng;
			if (position != string::npos) {
				lng = stop.substr(0, position);
				stops[stop_name] = stop.substr(position + 2);
			} else {
				lng = stop;
			}
			catalogue.AddStop(stop_name, { atof(lat.c_str()), atof(lng.c_str()) });
		}
		
		void ParseAndAddAdditionalInfoAboutStop(TransportCatalogue& catalogue, const std::string& first_stop, std::string& info) {
			while (!info.empty()) {
				auto position = info.find("m"s);
				const string distance = info.substr(0, position);
				info = info.substr(position + 5);
				position = info.find(","s);
				const string second_stop = info.substr(0, position);
				const auto stop_first = catalogue.GetStop(first_stop);
				const auto stop_second = catalogue.GetStop(second_stop);
				catalogue.SetDistanceBetweenStops(stop_first->name, stop_second->name, atoi(distance.c_str()));
				if (position != string::npos) {
					info = info.substr(position + 2);
				} else {
					break;
				}
			}
		}
		
		void ParseAndAddBus(TransportCatalogue& catalogue, std::string& bus) {
			bool is_circle = false;
			vector<string_view> route;
			auto position = bus.find(":"s);
			const string bus_name = bus.substr(4, position - 4);
			bus = bus.substr(position + 2);
			string name;
			string spliter = ">"s;
			if (bus.find("-"s) != string::npos) {
				spliter = "-"s;
				is_circle = true;
			}
			while (!bus.empty()) {
				position = bus.find(spliter);
				if (position == string::npos) {
					name = bus;
					break;
				} 
				name = bus.substr(0, position - 1);
				const auto stop = catalogue.GetStop(name);
				if (stop != nullptr) {
					route.push_back(stop->name);
				}
				bus = bus.substr(position + 2);
			}
			const auto stop = catalogue.GetStop(name);
			if (stop != nullptr) {
				route.push_back(stop->name);
			}
			if (is_circle) {
				for (int i = route.size() - 2; i >= 0; --i) {
					route.push_back(route[i]);
				}
			}
			catalogue.AddBus(bus_name, route, is_circle);
		}
		
		void ReadLineAndParseData(TransportCatalogue& catalogue, istream& in) {
			size_t count_requests;
			in >> count_requests;
			string line;
			size_t count = 0;
			unordered_set<string> buses;
			unordered_map<string, string> stops;
			while (getline(in, line)) {
				if (line.find("Stop"s) != string::npos) {
					ParseAndAddStop(catalogue, line, stops);
					++count;
				} else if (line.find("Bus"s) != string::npos) {
					buses.insert(line);
					++count;
				}
				if (count >= count_requests) {
					break;
				}
			}

			for (auto [first_stop, distances] : stops) {
				ParseAndAddAdditionalInfoAboutStop(catalogue, first_stop, distances);
			}

			for (string bus : buses) {
				ParseAndAddBus(catalogue, bus);
			}
		}
	}
}