# Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.5)
project(gbenchmark-build CXX)

if(test)
    
    include(ProcessorCount)
    ProcessorCount(N)

    if(NOT N EQUAL 0)
        if(((${CMAKE_VERSION} VERSION_GREATER "3.12.0") OR ${CMAKE_VERSION} VERSION_EQUAL "3.12.0"))
            set(CMAKE_BUILD_FLAGS -j ${N})
        elseif(LINUX OR QNX)
            set(CMAKE_BUILD_FLAGS -- -j ${N})
        endif()
    endif()

    # set download confi, source and build paths
    set(DOWNLOAD_CONFIG_DIR ${CMAKE_BINARY_DIR}/dependencies/gbenchmark/download)
    set(SOURCE_DIR ${CMAKE_BINARY_DIR}/dependencies/gbenchmark/src)
    set(BUILD_DIR ${CMAKE_BINARY_DIR}/dependencies/gbenchmark/build)
    set(INSTALL_DIR ${CMAKE_BINARY_DIR}/dependencies/install)
    set(GOOGLETEST_DIR ${CMAKE_BINARY_DIR}/dependencies/googletest/src)

    # Download and unpack gbenchmark at configure time
    configure_file(gbenchmark.cmake.in ${DOWNLOAD_CONFIG_DIR}/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} ${ENABLE_STATIC_DEBUG} -G "${CMAKE_GENERATOR}" "${DOWNLOAD_CONFIG_DIR}"
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${DOWNLOAD_CONFIG_DIR} )
    if(result)
        message(FATAL_ERROR "CMake step [configure download] for gbenchmark failed: ${result}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build . ${CMAKE_BUILD_FLAGS}
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${DOWNLOAD_CONFIG_DIR} )
    if(result)
        message(FATAL_ERROR "Build step [download] for gbenchmark failed: ${result}")
    endif()

    file(MAKE_DIRECTORY ${BUILD_DIR})

    execute_process(COMMAND ${CMAKE_COMMAND} ${ENABLE_STATIC_DEBUG} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=Release "-DGOOGLETEST_PATH=${GOOGLETEST_DIR}" "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}" "${SOURCE_DIR}"
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${BUILD_DIR} )
    if(result)
        message(FATAL_ERROR "CMake step [configure] for gbenchmark failed: ${result}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release --target install ${CMAKE_BUILD_FLAGS}
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${BUILD_DIR} )
    if(result)
        message(FATAL_ERROR "Build step [build and install] for gbenchmark failed: ${result}")
    endif()

    list(APPEND CMAKE_PREFIX_PATH ${INSTALL_DIR})
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE INTERNAL "" FORCE)

endif(test)
