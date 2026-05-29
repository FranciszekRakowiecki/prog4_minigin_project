if(WIN32 AND NOT EMSCRIPTEN)
  set(GAME_EXECUTABLE_TYPE WIN32
          TextureSelector.cpp
          TextureSelector.h
          GameState.cpp
          GameState.h
          Game.cpp
          Game.h
          MainMenuState.cpp
          MainMenuState.h
          SinglePlayerState.cpp
          SinglePlayerState.h
          CoopPlayerState.cpp
          CoopPlayerState.h
          VersusPlayerState.cpp
          VersusPlayerState.h
          Button.cpp
          Button.h
          AABB.cpp
          AABB.h
          PlayerInputHandler.cpp
          PlayerInputHandler.h)
endif()

add_executable(game ${GAME_EXECUTABLE_TYPE}
        Main.cpp
)

target_compile_features(game PRIVATE cxx_std_20)
target_link_libraries(game PRIVATE engine)
target_include_directories(game PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/Minigin"
)

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT game)

if(EMSCRIPTEN)
  set_target_properties(game PROPERTIES
      OUTPUT_NAME minigin_web
      SUFFIX ".html"
      LINK_FLAGS "-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Data@/\""
  )

  add_custom_command(TARGET game POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/web" "$<TARGET_FILE_DIR:game>/"
  )
elseif(WIN32)
  set_target_properties(game PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:game>")

  add_custom_command(
    TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:game>
        $<TARGET_FILE_DIR:game>
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

  if(MINIGIN_STEAMWORKS_FOUND AND MINIGIN_STEAMWORKS_RUNTIME_DLL)
    add_custom_command(TARGET game POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINIGIN_STEAMWORKS_RUNTIME_DLL}" "$<TARGET_FILE_DIR:game>")
  endif()
elseif(APPLE)
  set_target_properties(game PROPERTIES
    MACOSX_BUNDLE TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
  )

  add_custom_command(TARGET game POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_DIR:game>/Contents/Frameworks"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3-shared>" "$<TARGET_BUNDLE_DIR:game>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_BUNDLE_DIR:game>/Contents/Frameworks/"
    COMMAND install_name_tool -change "@rpath/libSDL3_ttf.0.dylib" "@executable_path/../Frameworks/libSDL3_ttf.0.2.2.dylib" "$<TARGET_FILE:game>"
    COMMAND install_name_tool -change "@rpath/libSDL3.0.dylib" "@executable_path/../Frameworks/libSDL3.0.dylib" "$<TARGET_FILE:game>"
  )

  if(MINIGIN_STEAMWORKS_FOUND)
    add_custom_command(TARGET game POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINIGIN_STEAMWORKS_LIBRARIES}" "$<TARGET_BUNDLE_DIR:game>/Contents/Frameworks/"
    )
  endif()
elseif(MINIGIN_STEAMWORKS_FOUND)
  add_custom_command(TARGET game POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINIGIN_STEAMWORKS_LIBRARIES}" "$<TARGET_FILE_DIR:game>"
  )
endif()

if(MINIGIN_STEAMWORKS_FOUND)
  add_custom_command(TARGET game POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:game>/steam_appid.txt"
    COMMENT "Generating steam_appid.txt"
  )
endif()

add_custom_command(TARGET game POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:game>/Data"
)
