# TransportCatalogue
Transport directory. Works with JSON requests. Returns a response to a request to draw routes with an SVG format string. The JSON constructor is implemented using a chain of method calls, obvious errors are at the compilation stage.
# Building and Run
``` 
  1. mkdir TransportCatalogueBuild && cd TransportCatalogueBuild
  2. cmake .. (if cmake throws a build error that protobuf could not be found, add the variable -DCMAKE_PREFIX_PATH=<path to the assembled protobuf files>)
  3. cmake --build . 
  4. Start ./transport_catalogue or transport_catalogue.exe
```
# Example
The program accepts two types of requests "make_base" and "process_requests". With the help of "make_base", the base of the transport directory for base_requests requests is formed and its serialization into a file is carried out. At the request of process_requests, the database is deserialized from the file and used to respond to stat_requests 
requests.
 1. Enter "make_base":
```
        {
            "serialization_settings": {
                "file": "transport_catalogue.db"
            },
            "routing_settings": {
                "bus_wait_time": 2,
                "bus_velocity": 30
            },
            "render_settings": {
                "width": 1200,
                "height": 500,
                "padding": 50,
                "stop_radius": 5,
                "line_width": 14,
                "bus_label_font_size": 20,
                "bus_label_offset": [
                    7,
                    15
                ],
                "stop_label_font_size": 18,
                "stop_label_offset": [
                    7,
                    -3
                ],
                "underlayer_color": [
                    255,
                    255,
                    255,
                    0.85
                ],
                "underlayer_width": 3,
                "color_palette": [
                    "green",
                    [
                        255,
                        160,
                        0
                    ],
                    "red"
                ]
            },
            "base_requests": [
                {
                    "type": "Bus",
                    "name": "14",
                    "stops": [
                        "Улица Лизы Чайкиной",
                        "Электросети",
                        "Ривьерский мост",
                        "Гостиница Сочи",
                        "Кубанская улица",
                        "По требованию",
                        "Улица Докучаева",
                        "Улица Лизы Чайкиной"
                    ],
                    "is_roundtrip": true
                },
                {
                    "type": "Bus",
                    "name": "24",
                    "stops": [
                        "Улица Докучаева",
                        "Параллельная улица",
                        "Электросети",
                        "Санаторий Родина"
                    ],
                    "is_roundtrip": false
                },
                {
                    "type": "Bus",
                    "name": "114",
                    "stops": [
                        "Морской вокзал",
                        "Ривьерский мост"
                    ],
                    "is_roundtrip": false
                },
                {
                    "type": "Stop",
                    "name": "Улица Лизы Чайкиной",
                    "latitude": 43.590317,
                    "longitude": 39.746833,
                    "road_distances": {
                        "Электросети": 4300,
                        "Улица Докучаева": 2000
                    }
                },
                {
                    "type": "Stop",
                    "name": "Морской вокзал",
                    "latitude": 43.581969,
                    "longitude": 39.719848,
                    "road_distances": {
                        "Ривьерский мост": 850
                    }
                },
                {
                    "type": "Stop",
                    "name": "Электросети",
                    "latitude": 43.598701,
                    "longitude": 39.730623,
                    "road_distances": {
                        "Санаторий Родина": 4500,
                        "Параллельная улица": 1200,
                        "Ривьерский мост": 1900
                    }
                },
                {
                    "type": "Stop",
                    "name": "Ривьерский мост",
                    "latitude": 43.587795,
                    "longitude": 39.716901,
                    "road_distances": {
                        "Морской вокзал": 850,
                        "Гостиница Сочи": 1740
                    }
                },
                {
                    "type": "Stop",
                    "name": "Гостиница Сочи",
                    "latitude": 43.578079,
                    "longitude": 39.728068,
                    "road_distances": {
                        "Кубанская улица": 320
                    }
                },
                {
                    "type": "Stop",
                    "name": "Кубанская улица",
                    "latitude": 43.578509,
                    "longitude": 39.730959,
                    "road_distances": {
                        "По требованию": 370
                    }
                },
                {
                    "type": "Stop",
                    "name": "По требованию",
                    "latitude": 43.579285,
                    "longitude": 39.733742,
                    "road_distances": {
                        "Улица Докучаева": 600
                    }
                },
                {
                    "type": "Stop",
                    "name": "Улица Докучаева",
                    "latitude": 43.585586,
                    "longitude": 39.733879,
                    "road_distances": {
                        "Параллельная улица": 1100
                    }
                },
                {
                    "type": "Stop",
                    "name": "Параллельная улица",
                    "latitude": 43.590041,
                    "longitude": 39.732886,
                    "road_distances": {}
                },
                {
                    "type": "Stop",
                    "name": "Санаторий Родина",
                    "latitude": 43.601202,
                    "longitude": 39.715498,
                    "road_distances": {}
                }
            ]
        }
```
 2. Enter "process_requests":
```
        {
            "serialization_settings": {
                "file": "transport_catalogue.db"
            },
            "stat_requests": [
                {
                    "id": 218563507,
                    "type": "Bus",
                    "name": "14"
                },
                {
                    "id": 508658276,
                    "type": "Stop",
                    "name": "Электросети"
                },
                {
                    "id": 1964680131,
                    "type": "Route",
                    "from": "Морской вокзал",
                    "to": "Параллельная улица"
                },
                {
                    "id": 1359372752,
                    "type": "Map"
                }
            ]
        }
```
# System requirements and Stack
  1. C++17
  2. GCC version 8.1.0
  3. Protobuf-cpp 3.18.1 (download https://github.com/protocolbuffers/protobuf/releases or install it as a package for linux distribution)
  4. Cmake 3.21.2 (minimal 3.10)
# Future plans
  1. Develop a frontend for drawing a map with a route
  2. Correct JSON output
