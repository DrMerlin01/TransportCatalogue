#include "stat_reader.h"
#include "input_reader.h"
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

namespace transport_catalogue {
	namespace stat_reader {
		ostream& operator<<(ostream& out, const content::InfoOnRoute& info) {
			if (info.count_bus_stop == 0) {
				out << "not found"s << endl;
			} else {
				out << info.count_bus_stop << " stops on route, "s
					<< info.count_unique_bus_stop << " unique stops, "s
					<< setprecision(6) << info.lenght_route << " route length, "s
					<< setprecision(6) << info.curvature << " curvature"s << endl;
			}
			return out;
		}

		void PrintInfoOnBus(const TransportCatalogue& catalogue, const std::string& bus, ostream& out) {
			const string bus_name = bus.substr(4);
			out << "Bus "s << bus_name << ": "s;
			out << catalogue.GetInfoOnRoute(bus_name);
		}
		
		void PrintInfoOnStop(const TransportCatalogue& catalogue, const std::string& stop, ostream& out) {
			const string name = stop.substr(5);
			out << "Stop "s << name << ": "s;
			const auto buses_by_stop = catalogue.GetBusesThroughStop(name);
			if (catalogue.GetStop(name) == nullptr) {
				out << "not found"s << endl;
			} else if (buses_by_stop.empty()) {
				out << "no buses"s << endl;
			} else { 
				out << "buses"s;
				for (const auto bus : buses_by_stop) {
					out << " "s << bus;
				}
				out << endl;
			}
		}
		
		void GetInfoOnRoutes(const TransportCatalogue& catalogue, istream& in, ostream& out) {
			size_t count_requests = 0;
			in >> count_requests;
			string str;
			size_t count = 0;
			while (getline(in, str)) {
				if (str.find("Bus"s) != string::npos) {
					PrintInfoOnBus(catalogue, str, out);
					++count;
				} else if (str.find("Stop"s) != string::npos) {
					PrintInfoOnStop(catalogue, str, out);
					++count;
				} else if (!str.empty()) {
					++count;
				} 
				if (count >= count_requests) {
					break;
				}
			}
		}
	}
}