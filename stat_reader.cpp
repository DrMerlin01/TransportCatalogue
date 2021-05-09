#include "stat_reader.h"
#include "input_reader.h"
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

namespace transport_catalogue {
	namespace detail {
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

		void GetInfoOnRoutes(TransportCatalogue& cataloge) {
			size_t count_requests = 0;
			cin >> count_requests;
			string str;
			size_t count = 0;
			while (getline(cin, str)) {
				if (str.find("Bus") != std::string::npos) {
					const string bus_name = str.substr(4);
					cout << "Bus "s << bus_name << ": "s;
					cout << cataloge.GetInfoOnRoute(bus_name);
					++count;
				} else if (str.find("Stop") != std::string::npos) {
					const string name = str.substr(5);
					cout << "Stop "s << name << ": "s;
					const auto buses_by_stop = cataloge.GetBusesThroughStop(name);
					if (cataloge.GetStop(name) == nullptr) {
						cout << "not found"s << endl;
					} else if (buses_by_stop.empty()) {
						cout << "no buses"s << endl;
					} else { 
						cout << "buses"s;
						for (const auto bus : buses_by_stop) {
							cout << " "s << bus;
						}
						cout << endl;
					}
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