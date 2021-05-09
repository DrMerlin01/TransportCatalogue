#include "input_reader.h"
#include "geo.h"
#include <iostream>
#include <unordered_set>
#include <stdlib.h>
#include <string_view>

using namespace std;

namespace transport_catalogue {
	namespace detail {
		void ReadLineWithData(TransportCatalogue& cataloge) {
			size_t result;
			cin >> result;
			string str;
			size_t count = 0;
			unordered_set<string> buses;
			unordered_map<string, string> stops;
			while (getline(cin, str)) {
				if (str.find("Stop") != std::string::npos) {
					auto pos = str.find(":");
					const string name = str.substr(5, pos - 5);
					str = str.substr(pos + 2);
					pos = str.find(",");
					const auto lat = str.substr(0, pos);
					str = str.substr(pos + 2);
					pos = str.find(",");
					string lng;
					if(pos != std::string::npos) {
						lng = str.substr(0, pos);
						stops[name] = str.substr(pos + 2);
					} else {
						lng = str;
					}
					cataloge.AddStop(name, { atof(lat.c_str()), atof(lng.c_str()) });
					++count;
				} else if (str.find("Bus") != std::string::npos) {
					buses.insert(str);
					++count;
				}
				if (count >= result) {
					break;
				}
			}

			for(auto [first_stop, distances] : stops) {
				while(!distances.empty()) {
					auto pos = distances.find("m");
					const string distance = distances.substr(0, pos);
					distances = distances.substr(pos + 5);
					pos = distances.find(",");
					string second_stop;
					if(pos != std::string::npos) {
						second_stop = distances.substr(0, pos);
						const auto stop_first = cataloge.GetStop(first_stop);
						const auto stop_second = cataloge.GetStop(second_stop);
						cataloge.SetDistanceBetweenStops(stop_first->name, stop_second->name, atoi(distance.c_str()));
						distances = distances.substr(pos + 2);
					} else {
						second_stop = distances.substr(0, pos);
						const auto stop_first = cataloge.GetStop(first_stop);
						const auto stop_second = cataloge.GetStop(second_stop);
						cataloge.SetDistanceBetweenStops(stop_first->name, stop_second->name, atoi(distance.c_str()));
						break;
					}
				}
			}

			for (string bus : buses) {
				bool is_circle = false;
				vector<string_view> route;
				auto pos = bus.find(":");
				const string bus_name = bus.substr(4, pos - 4);
				bus = bus.substr(pos + 2);
				string name;
				string spliter = ">";
				if (bus.find("-") != std::string::npos) {
					spliter = "-";
					is_circle = true;
				}
				while(!bus.empty()) {
					pos = bus.find(spliter);
					if(pos == std::string::npos) {
						name = bus;
						break;
					} 
					name = bus.substr(0, pos - 1);
					const auto stop = cataloge.GetStop(name);
					if(stop != nullptr) {
						route.push_back(stop->name);
					}
					bus = bus.substr(pos + 2);
				}
				const auto stop = cataloge.GetStop(name);
				if(stop != nullptr) {
					route.push_back(stop->name);
				}
				if (is_circle) {
					for (int i = route.size() - 2; i >= 0; --i) {
						route.push_back(route[i]);
					}
				}
				cataloge.AddBus(bus_name, route, is_circle);
			}
		}
	}
}