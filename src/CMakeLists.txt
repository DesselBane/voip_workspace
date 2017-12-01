cmake_minimum_required( VERSION 3.0 )

project( VoIP_Communicator )

include_directories( ${VOIP_EXT_INC_DIR} )

file(GLOB SRC *.cpp )

file(GLOB HDR *.h )

# NOTE: Change this to voipcomm_yourstudentid, e.g. voipcomm_foobar4242
set( EXECUTABLE_NAME voipcomm_morawietzja61762 )

if( ${EXECUTABLE_NAME} STREQUAL "voipcomm_default" )
  message( FATAL_ERROR "\n!!! NOTE !!!\nPlease change the executable name to reflect your student id!\nThe default is not allowed. CMake will now exit." )
endif()

# Note: the headers in ${HDR} are added only
# to display them when generating projects for
# your favorite IDE
add_executable( ${EXECUTABLE_NAME} ${SRC} ${HDR})
target_link_libraries( ${EXECUTABLE_NAME} ${VOIP_EXT_LIBS} )
