cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
	set(SYSTEM_LIBS -lstdc++)
else()
	set(SYSTEM_LIBS)
endif()

set(INCLUDE_DIR inc)
set(SOURCE_DIR src)
set(PROTO_DIR protofiles)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(TRANSPORT_CATALOGUE_PROTO_SRCS TRANSPORT_CATALOGUE_PROTO_HDRS "${PROTO_DIR}/transport_catalogue.proto")
protobuf_generate_cpp(TRANSPORT_ROUTER_PROTO_SRCS TRANSPORT_ROUTER_PROTO_HDRS "${PROTO_DIR}/transport_router.proto")
protobuf_generate_cpp(MAP_RENDERER_PROTO_SRCS MAP_RENDERER_PROTO_HDRS "${PROTO_DIR}/map_renderer.proto")
protobuf_generate_cpp(GRAPH_PROTO_SRCS GRAPH_PROTO_HDRS "${PROTO_DIR}/graph.proto")
protobuf_generate_cpp(SVG_PROTO_SRCS SVG_PROTO_HDRS "${PROTO_DIR}/svg.proto")

set(TRANSPORT_CATALOGUE_FILES "${SOURCE_DIR}/domain.cpp"
							  "${INCLUDE_DIR}/domain.h"
							  "${SOURCE_DIR}/geo.cpp"
							  "${INCLUDE_DIR}/geo.h"
							  "${INCLUDE_DIR}/graph.h"
							  "${SOURCE_DIR}/json_builder.cpp"
							  "${INCLUDE_DIR}/json_builder.h"
							  "${SOURCE_DIR}/json_reader.cpp"
							  "${INCLUDE_DIR}/json_reader.h"
							  "${SOURCE_DIR}/json.cpp"
							  "${INCLUDE_DIR}/json.h"
							  "${SOURCE_DIR}/map_renderer.cpp"
							  "${INCLUDE_DIR}/map_renderer.h"
							  "${INCLUDE_DIR}/ranges.h"
							  "${SOURCE_DIR}/request_handler.cpp"
							  "${INCLUDE_DIR}/request_handler.h"
							  "${INCLUDE_DIR}/router.h"
							  "${SOURCE_DIR}/serialization.cpp"
							  "${INCLUDE_DIR}/serialization.h"
							  "${SOURCE_DIR}/svg.cpp"
							  "${INCLUDE_DIR}/svg.h"
							  "${SOURCE_DIR}/transport_catalogue.cpp"
							  "${INCLUDE_DIR}/transport_catalogue.h"
							  "${SOURCE_DIR}/transport_router.cpp"
							  "${INCLUDE_DIR}/transport_router.h")
set(PROTO_FILES "${PROTO_DIR}/transport_catalogue.proto"
				"${PROTO_DIR}/transport_router.proto"
				"{PROTO_DIR}/map_renderer.proto"
				"${PROTO_DIR}/graph.proto"
				"${PROTO_DIR}/svgh.proto")
set(MAIN_FILES "${SOURCE_DIR}/main.cpp")

source_group("Transport Catalogue" FILES ${TRANSPORT_CATALOGUE_FILES})
source_group("Protofiles" FILES ${PROTO_FILES})
source_group("Source" FILES ${MAIN_FILES})

add_executable(transport_catalogue ${TRANSPORT_CATALOGUE_PROTO_SRCS} ${TRANSPORT_CATALOGUE_PROTO_HDRS}
								   ${MAP_RENDERER_PROTO_SRCS} ${MAP_RENDERER_PROTO_HDRS} 
								   ${GRAPH_PROTO_SRCS} ${GRAPH_PROTO_HDRS}
								   ${TRANSPORT_ROUTER_PROTO_SRCS} ${TRANSPORT_ROUTER_PROTO_HDRS}
								   ${SVG_PROTO_SRCS} ${SVG_PROTO_HDRS}
								   ${TRANSPORT_CATALOGUE_FILES}
								   ${MAIN_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)