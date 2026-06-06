cmake_minimum_required(VERSION 3.16)

project(quiz_sfml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(quiz_core
    Data.cpp
    QuizEngine.cpp
    PlayerManager.cpp
    QuestionEditor.cpp
    RankingView.cpp
)

target_include_directories(quiz_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(quiz_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

find_package(SFML 3 CONFIG REQUIRED COMPONENTS System Window Graphics)

add_executable(quiz_sfml
    main.cpp
    App.cpp
    Button.cpp
)

target_link_libraries(quiz_sfml PRIVATE
    quiz_core
    SFML::System
    SFML::Window
    SFML::Graphics
)

target_compile_options(quiz_sfml PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

enable_testing()

find_package(Threads REQUIRED)

add_executable(quiz_tests
    tests/quiz_core_tests.cpp
)

target_include_directories(quiz_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/doctest
)

target_link_libraries(quiz_tests PRIVATE quiz_core Threads::Threads)

target_compile_options(quiz_tests PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8>
    $<$<CXX_COMPILER_ID:GNU>:-fexec-charset=UTF-8>
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

add_test(NAME quiz_core_tests COMMAND quiz_tests)

find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
endif()