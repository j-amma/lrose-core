# CMakeLists for dataport library

cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)

add_library(dsserver
DataMapper/DmapAccess.cc
DataMapper/DmapMessage.cc
DestUrlArray/DestUrlArray.cc
DsClient/DsClient.cc
DsClient/DsThreadedClient.cc
DsFileCopy/DsFileCopy.cc
DsFileCopy/DsFileCopyMsg.cc
DsFileIo/DsFileIo.cc
DsFileIo/DsFileIoMsg.cc
DsLdataInfo/DsLdataInfo.cc
DsLdataInfo/DsLdataMsg.cc
DsLdataInfo/DsLdataServerMsg.cc
DsLocator/DsLocator.cc
DsServer/DsProcessServer.cc
DsServer/DsServer.cc
DsServer/DsServerMsg.cc
DsServer/DsThreadedServer.cc
DsServer/ProcessServer.cc
DsSvrMgrSocket/DsSvrMgrSocket.cc
)

include_directories (./include $ENV{LROSE_INSTALL_DIR}/include)

INSTALL(FILES libdsserver.a
        DESTINATION $ENV{LROSE_INSTALL_DIR}/lib
	)

# add_library (dataport ${bigend_sources} ${smallend_sources} ${swap_sources})
 

