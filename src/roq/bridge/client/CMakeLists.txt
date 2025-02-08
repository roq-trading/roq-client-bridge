set(TARGET_NAME ${PROJECT_NAME})

add_subdirectory(flags)

set(SOURCES
    application.cpp
    bridge.cpp
    config.cpp
    controller.cpp
    session.cpp
    settings.cpp
    shared.cpp
    main.cpp)

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME})

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(
  ${TARGET_NAME}
  PRIVATE ${PROJECT_NAME}-flags
          roq-api::roq-api
          roq-client::roq-client
          roq-client::roq-client-flags
          roq-codec::roq-codec
          roq-fix::roq-fix
          roq-flags::roq-flags
          roq-web::roq-web
          roq-io::roq-io
          roq-codec::roq-codec
          roq-logging::roq-logging
          roq-logging::roq-logging-flags
          fmt::fmt
          tomlplusplus::tomlplusplus
          unordered_dense::unordered_dense)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

target_compile_definitions(
  ${TARGET_NAME} PRIVATE ROQ_PACKAGE_NAME="${TARGET_NAME}" ROQ_HOST="${ROQ_HOST}" ROQ_BUILD_VERSION="${GIT_REPO_VERSION}"
                         ROQ_GIT_DESCRIBE_HASH="${GIT_DESCRIBE_HASH}" ROQ_BUILD_NUMBER="${ROQ_BUILD_NUMBER}" ROQ_BUILD_TYPE="${ROQ_BUILD_TYPE}")

install(TARGETS ${TARGET_NAME})
