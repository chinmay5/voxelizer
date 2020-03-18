CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT ( CudaVoxelizer )

SET(CMAKE_MODULE_PATH /home/chinmay/Desktop/reference_code/cuda_voxelizer_bkp/cmake)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FIND_PACKAGE(CUDA QUIET REQUIRED)
FIND_PACKAGE(GLM REQUIRED)
FIND_PACKAGE(OpenMP REQUIRED)
find_package(HDF5 COMPONENTS C CXX HL REQUIRED)
find_package(Eigen3 REQUIRED)

SET(CUDA_VOXELIZER_EXECUTABLE cuda_voxelizer)

SET(Trimesh2_INCLUDE_DIR "/home/chinmay/softwares/trimesh2-2.16/trimesh2/include")

FIND_FILE(Trimesh2_TriMesh_h TriMesh.h /home/chinmay/softwares/trimesh2-2.16/trimesh2/include)
IF(NOT Trimesh2_TriMesh_h)
	message(SEND_ERROR "Can't find TriMesh.h in /home/chinmay/softwares/trimesh2-2.16/trimesh2/include ")
ENDIF()

# Check for the tinyply.h file
#FIND_FILE(Tinyply_h tinyply.h /home/chinmay/softwares/tinyply/source)
#IF(NOT Tinyply_h)
#	message(SEND_ERROR "Can't find tinyply.h in /home/chinmay/softwares/tinyply/source ")
#ENDIF()


MARK_AS_ADVANCED(Trimesh2_TriMesh_h)


INCLUDE_DIRECTORIES(${Trimesh2_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} src/external)

SET(Trimesh2_LINK_DIR "/home/chinmay/softwares/trimesh2-2.16/trimesh2/lib.Linux64" CACHE PATH "Path to Trimesh2 libraries")
FIND_LIBRARY(Trimesh2_LIBRARY trimesh ${Trimesh2_LINK_DIR})
IF(NOT Trimesh2_LIBRARY)
	message(SEND_ERROR "Can't find libtrimesh.a in ${Trimesh2_LINK_DIR}")
ENDIF()
MARK_AS_ADVANCED(Trimesh2_LIBRARY)

#SET(Tinyply_LINK_DIR "/home/chinmay/softwares/tinyply/build" CACHE PATH "Path to tinyply libraries")
#FIND_LIBRARY(TinyPly_LIBRARY tinyply ${Tinyply_LINK_DIR})
#IF(NOT TinyPly_LIBRARY)
#	message(SEND_ERROR "Can't find libtinyply.a in ${Tinyply_LINK_DIR}")
#ENDIF()
#MARK_AS_ADVANCED(Tinyply_LIBRARY)



SET(CUDA_VOXELIZER_SRCS
  ./src/main.cpp
  ./src/util_cuda.cpp
  ./src/util_io.cpp
  ./src/cpu_voxelizer.cpp
)
SET(CUDA_VOXELIZER_SRCS_CU
  ./src/voxelize.cu
  ./src/thrust_operations.cu
)

#set(SOURCES src/external/tinyply.h)

CUDA_ADD_EXECUTABLE(
  ${CUDA_VOXELIZER_EXECUTABLE} 
  ${CUDA_VOXELIZER_SRCS}
  ${CUDA_VOXELIZER_SRCS_CU}
#  ${SOURCES}
)

TARGET_INCLUDE_DIRECTORIES(	${CUDA_VOXELIZER_EXECUTABLE} PUBLIC
  ${Trimesh2_INCLUDE_DIR} ${HDF5_CXX_LIBRARIES}  )

TARGET_LINK_LIBRARIES ( ${CUDA_VOXELIZER_EXECUTABLE}
  ${Trimesh2_LIBRARY}
  gomp
  ${HDF5_CXX_LIBRARIES}
)
