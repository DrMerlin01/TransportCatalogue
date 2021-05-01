#include "input_reader.h"
#include "geo.h"
#include <iostream>
#include <unordered_set>
#include <stdlib.h>
#include <string_view>

using namespace std;

vector<string> SplitIntoWords(const string& text) {
	vector<string> words;
	string word;
	for (const char c : text) {
		if (c == ' ') {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
		} else {
			word += c;
		}
	}
	if (!word.empty()) {
		words.push_back(word);
	}
	return words;
}

void ReadLineWithData(TransportCatalogue& cataloge) {
	size_t result;
	cin >> result;
	string str;
	size_t count = 0;
	unordered_set<string> buses;
	while (getline(cin, str)) {
		if (str.find("Stop") != std::string::npos) {
			auto parser = SplitIntoWords(str);
			string name;
            size_t cnt = 1;
			for (size_t i = 1; i < parser.size(); ++i) {
				if (parser[i].find(":") == std::string::npos) {
					if(name.size() != 0) {
						name += " ";
					}
					name += parser[i];
                    ++cnt;
				} else {
					if(name.size() != 0) {
						name += " ";
					}
					name += parser[i].substr(0, parser[i].size() - 1);
                    ++cnt;
					break;
				}
			}
			const auto lat = parser[cnt].substr(0, parser[cnt].size() - 1);
            ++cnt;
            string lng;
            if(parser[cnt].find(",") != std::string::npos) {
                lng = parser[cnt].substr(0, parser[cnt].size() - 1);
            } else {
                lng = parser[cnt];
            }
			cataloge.AddStop(name, { atof(lat.c_str()), atof(lng.c_str()) });
			++count;
		} else if (str.find("Bus") != std::string::npos) {
			buses.insert(str);
			++count;
		}else if (!str.empty()) {
            ++count;
        }
		if (count >= result) {
			break;
		}
	}

	for (auto& bus : buses) {
		bool circle = false;
		auto parser = SplitIntoWords(bus);
		vector<string_view> route;
		string name;
        size_t cnt = 1;
        string bus_name;
        for (size_t i = 1; i < parser.size(); ++i) {
            if (parser[i].find(":") == std::string::npos) {
                if(bus_name.size() != 0) {
                    bus_name += " ";
                }
                bus_name += parser[i];
                ++cnt;
            } else {
                if(bus_name.size() != 0) {
                    bus_name += " ";
                }
                bus_name += parser[i].substr(0, parser[i].size() - 1);
                ++cnt;
                break;
            }
        }
		for (size_t i = cnt; i < parser.size(); ++i) {
			if (parser[i] == ">"|| parser[i] == "-") {
                const auto stop_ = cataloge.GetStop(name);
				route.push_back(stop_->name);
				name = "";
			} else {
                if(!name.empty()) {
                    name += " ";
                }
				name += parser[i];
			}
		}
        const auto stop_ = cataloge.GetStop(name);
		route.push_back(stop_->name);
		if (bus.find("-") != std::string::npos) {
			circle = true;
			for (int i = route.size() - 2; i >= 0; --i) {
				route.push_back(route[i]);
			}
		}
		cataloge.AddBus(bus_name, route, circle);
	}
}