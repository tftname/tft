file(GLOB HEADERS "include/dncio/testing/*.hpp")

## SORT .cpp by most likely to change / break compile
add_library( dncio_testing
             tester.cpp
             tester_network.cpp
             ${HEADERS}
        )

target_link_libraries( dncio_testing dncio_chain dnc_utilities fc chainbase Logging IR WAST WASM Runtime )
target_include_directories( dncio_testing
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../wasm-jit/Include"
                                   "${CMAKE_BINARY_DIR}/contracts"
                            )

add_dependencies( dncio_testing dncio.bios )

if(MSVC)
  set_source_files_properties( db_init.cpp db_block.cpp database.cpp block_log.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

