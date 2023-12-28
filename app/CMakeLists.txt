#Compile options
add_compile_options(
        -Werror

        -Wall
        -Wextra
        -Wpedantic

        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wenum-compare
        -Wfloat-equal
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-conversion
        -Wsign-promo
)


add_executable(${APP_NAME}
        ${APP_MAIN}
        ${APP_SOURCES}
)
set_target_properties(${APP_NAME} PROPERTIES ENABLE_EXPORTS 1)

target_sources(${APP_NAME} PUBLIC ${APP_SOURCES})
target_include_directories(${APP_NAME} PUBLIC ${APP_INCLUDE_PATH} ${CMAKE_HOME_DIRECTORY})

target_link_libraries(${APP_NAME}
        PRIVATE
        log4cplus::log4cplus
        nlohmann_json::nlohmann_json
)