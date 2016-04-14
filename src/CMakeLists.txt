
include_directories(.)
include_directories(common)
include_directories(decoder)
include_directories(mblas)
include_directories(dl4mt)

add_library(libamunn OBJECT
  cnpy/cnpy.cpp
  rescorer/nbest.cpp
  common/utils.cpp
)

cuda_add_executable(
  amunn
  decoder/decoder_main.cu
  $<TARGET_OBJECTS:libamunn>
)

cuda_add_executable(
  rescorer
  rescorer/rescorer_main.cu
  $<TARGET_OBJECTS:libamunn>
)

foreach(exec amunn rescorer)
  target_link_libraries(${exec} ${EXT_LIBS})
  cuda_add_cublas_to_target(${exec})
  set_target_properties(${exec} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endforeach(exec)