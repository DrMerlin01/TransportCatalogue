cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(TRANSPORT_CATALOGUE_PROTO_SRCS TRANSPORT_CATALOGUE_PROTO_HDRS transport_catalogue.proto)
protobuf_generate_cpp(MAP_RENDERER_PROTO_SRCS MAP_RENDERER_PROTO_HDRS map_renderer.proto)
protobuf_generate_cpp(GRAPH_PROTO_SRCS GRAPH_PROTO_HDRS graph.proto)
protobuf_generate_cpp(TRANSPORT_ROUTER_PROTO_SRCS TRANSPORT_ROUTER_PROTO_HDRS transport_router.proto)

set(TRANSPORT_CATALOGUE_FILES main.cpp 
							 domain.cpp domain.h
							 geo.cpp geo.h
							 graph.h
							 json_builder.cpp json_builder.h
							 json_reader.cpp json_reader.h
							 json.cpp json.h
							 map_renderer.cpp map_renderer.h
							 ranges.h
							 request_handler.cpp request_handler.h
							 router.h
							 serialization.cpp serialization.h
							 svg.cpp svg.h
							 transport_catalogue.cpp transport_catalogue.h
							 transport_catalogue.proto
							 transport_router.cpp transport_router.h)

add_executable(transport_catalogue 
				${TRANSPORT_CATALOGUE_PROTO_SRCS} ${TRANSPORT_CATALOGUE_PROTO_HDRS}
				${MAP_RENDERER_PROTO_SRCS} ${MAP_RENDERER_PROTO_HDRS} 
				${GRAPH_PROTO_SRCS} ${GRAPH_PROTO_HDRS}
				${TRANSPORT_ROUTER_PROTO_SRCS} ${TRANSPORT_ROUTER_PROTO_HDRS}
				${TRANSPORT_CATALOGUE_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)