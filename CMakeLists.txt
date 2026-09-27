cmake_minimum_required(VERSION 3.11)
project(CppLanguageServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(json)
add_executable(cpp_lsp
   src/main.cpp
   src/server.cpp
   src/parser.cpp
)

if(MSVC)
    # Strong warnings for Microsoft Visual Studio
    target_compile_options(cpp_lsp PRIVATE /W4 /WX)
else()
    # Strong warnings for GCC (Linux) and Clang (macOS/Linux)
    # -Wall: Enables common warnings
    # -Wextra: Enables extra useful warnings
    # -Wpedantic: Enforces strict C++ standard compliance
    # -Werror: Treats warnings as compilation errors to force you to fix them
    target_compile_options(cpp_lsp PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()



target_include_directories(cpp_lsp PRIVATE include)

target_link_libraries(cpp_lsp PRIVATE nlohmann_json::nlohmann_json)