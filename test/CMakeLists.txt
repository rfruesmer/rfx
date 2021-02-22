find_package(glfw3 CONFIG REQUIRED)
link_libraries(glfw)

function(buildTest TEST_NAME)
    SET(TEST_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME})
    file(GLOB SOURCE
        ${TEST_FOLDER}/../*.cpp
        ${TEST_FOLDER}/../*.h
        ${TEST_FOLDER}/*.cpp)
    add_executable(${TEST_NAME} ${BASE_SOURCE} ${SOURCE})
endfunction()

function(buildTests)
    foreach(TEST ${TESTS})
        buildTest(${TEST})
    endforeach()
endfunction()

set(TESTS
    ColoredQuad
    TexturedQuad
)

buildTests()
