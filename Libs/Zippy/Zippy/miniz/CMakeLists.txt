#=======================================================================================================================
# Define MINIZ library target
#=======================================================================================================================
add_library(miniz INTERFACE)
target_include_directories(miniz SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR})