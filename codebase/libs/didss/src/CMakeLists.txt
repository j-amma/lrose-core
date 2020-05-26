# CMakeLists for dataport library

cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)

add_library(didss 
DataFileNames/DataFileNames.cc
DataFileNames/TestDfn.cc
DsAccess/DsAccess.cc
DsAccess/DsAccessFile.cc
DsAccess/IPAddress.cc
DsInputPath/DsDataFile.cc
DsInputPath/DsInputPath.cc
DsInputPath/DsTimeList.cc
DsInputPath/ds_input_path.c
DsMessage/DsMessage.cc
DsMessage/DsMsgPart.cc
DsMessage/ds_message.c
DsMessage/ds_msg_handle.c
DsURL/DsURL.cc
DsURL/DsUrlQueue.cc
LdataInfo/LdataInfo.cc
RapDataDir/RapDataDir.cc
RapDataDir/RapDataDir_r.cc
)

include_directories (./include $ENV{LROSE_INSTALL_DIR}/include)

INSTALL(FILES libdidss.a
        DESTINATION $ENV{LROSE_INSTALL_DIR}/lib
	)


# add_library (dataport ${bigend_sources} ${smallend_sources} ${swap_sources})

