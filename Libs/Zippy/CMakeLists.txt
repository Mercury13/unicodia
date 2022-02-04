#=======================================================================================================================
# Load the miniz subdirectory
#=======================================================================================================================
#add_subdirectory(Zippy/miniz)

#=======================================================================================================================
# Define Zippy library targets
#=======================================================================================================================
add_library(Zippy INTERFACE)
add_library(Zippy::Zippy ALIAS Zippy)
#target_link_libraries(Zippy INTERFACE miniz)
target_include_directories(Zippy INTERFACE ${CMAKE_CURRENT_LIST_DIR})
