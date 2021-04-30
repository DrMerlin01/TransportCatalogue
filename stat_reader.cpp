#include "stat_reader.h"
#include "input_reader.h"
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

ostream& operator<<(ostream& out, const InfoOnRoute& info) {
	if (info.count_bus_stop == 0) {
		out << "not found"s << endl;
	}
	else {
		out << info.count_bus_stop << " stops on route, "s
			<< info.count_unique_bus_stop << " unique stops, "s
			<< setprecision(6) << info.lenght_route << " route length"s << endl;
	}
	return out;
}

void GetInfoOnRoutes(TransportCatalogue& cataloge) {
	size_t count_requests = 0;
	cin >> count_requests;
	string str;
	size_t count = 0;
	vector<string> buses;
	while (getline(cin, str)) {
		if (str.find("Bus") != std::string::npos) {
			auto parser = SplitIntoWords(str);
            string bus_name;
            for (size_t i = 1; i < parser.size(); ++i) {
                if (bus_name.size() != 0) {
                    bus_name += " ";
                }
                bus_name += parser[i];
            }
			buses.push_back(bus_name);
			++count;
		} else if (!str.empty()) {
            ++count;
        } 
		if (count >= count_requests) {
			break;
		}
	}

	for (const auto& bus : buses) {
		cout << "Bus "s << bus << ": "s;
		cout << cataloge.GetInfoOnRoute(bus);
	}
}