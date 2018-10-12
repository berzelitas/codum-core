cmake_minimum_required(VERSION 3.10)

project(mbps VERSION 0.0.1)

if(EOSIO_CDT_ROOT STREQUAL "" OR NOT EOSIO_CDT_ROOT)
    set(EOSIO_CDT_ROOT "/usr/local/eosio.cdt")
endif()

include_directories(${EOSIO_CDT_ROOT}/include)

list(APPEND CMAKE_MODULE_PATH ${EOSIO_CDT_ROOT}/lib/cmake)
include(EosioWasmToolchain)
