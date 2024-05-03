cmake_minimum_required(VERSION 3.12)
project(Chess)

include_directories(include) # header files there

add_library(fenParser src/FenParser.cpp include/FenParser.h)
add_library(gameState src/GameState.cpp include/GameState.h)
add_library(move src/Move.cpp include/Move.h)
add_library(piece src/Piece.cpp)
add_library(game include/Game.h)

target_link_libraries(move PRIVATE piece)

target_link_libraries(fenParser PRIVATE move)

target_link_libraries(gameState PRIVATE fenParser)

target_link_libraries(game PRIVATE gameState)

add_executable(
  lookup_generator
  lookup_generator.cpp
)
add_executable(
  main
  main.cpp
)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(main PRIVATE gameState)
target_link_libraries(main PRIVATE game)
target_link_libraries(lookup_generator PRIVATE fenParser)
target_link_libraries(lookup_generator PRIVATE gameState)

enable_testing()

add_executable(
  google_testing
  tests/MoveGenerationTest.cpp
  tests/PerftGenerationTest.cpp
)
target_link_libraries(
  google_testing
  GTest::gtest_main
  piece
  move
  fenParser
  gameState
)

include(GoogleTest)
gtest_discover_tests(google_testing)