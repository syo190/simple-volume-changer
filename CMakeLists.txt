cmake_minimum_required(VERSION 3.0.0)
project(VoiceChanger VERSION 0.1.0)



add_executable(
    VoiceChanger WIN32
    src/main.cpp
    src/Slider.cpp
    src/SoundControler.cpp
    src/TaskTray.cpp
    src/EditBox.cpp
)
target_sources(
    VoiceChanger PRIVATE resource.rc
)

# MUST: after add_executable
add_compile_options(/clr)

target_link_libraries( # define libraries that the target links
    VoiceChanger PRIVATE winmm gdi32 comctl32 Propsys
)
