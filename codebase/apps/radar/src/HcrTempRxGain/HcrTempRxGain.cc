// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
//////////////////////////////////////////////////////////////////////////
// HcrTempRxGain.cc
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// August 2018
//
///////////////////////////////////////////////////////////////
//
// HcrTempRxGain reads in HCR monitoring data from SPDB,
// computes the gain correction from the temperatures,
// and writes out the results to SPDB. The HCR monitoring
// data is generated by TsStatusMonitor, which reads in time
// series and writes out the status XML to SPDB.
//
////////////////////////////////////////////////////////////////

#include "HcrTempRxGain.hh"
#include <toolsa/pmu.h>
#include <toolsa/uusleep.h>
#include <Spdb/DsSpdb.hh>
using namespace std;

// Constructor

HcrTempRxGain::HcrTempRxGain(int argc, char **argv)
  
{

  OK = TRUE;
  
  // set programe name
  
  _progName = "HcrTempRxGain";
  
  // parse command line args
  
  if (_args.parse(argc, argv, _progName)) {
    cerr << "ERROR: " << _progName << endl;
    cerr << "Problem with command line args." << endl;
    OK = FALSE;
    return;
  }
  
  // get TDRP params
  
  _paramsPath = (char *) "unknown";
  if (_params.loadFromArgs(argc, argv,
			   _args.override.list, &_paramsPath)) {
    cerr << "ERROR: " << _progName << endl;
    cerr << "Problem with TDRP parameters." << endl;
    OK = FALSE;
    return;
  }
  
  // init process mapper registration

  if (_params.mode == Params::REALTIME) {
    PMU_auto_init((char *) _progName.c_str(),
                  _params.instance,
                  PROCMAP_REGISTER_INTERVAL);
  }

}

// destructor

HcrTempRxGain::~HcrTempRxGain()

{

  // unregister process

  PMU_auto_unregister();

}

//////////////////////////////////////////////////
// Run

int HcrTempRxGain::Run()
{

  if (_params.mode == Params::REALTIME) {
    return _runRealtime();
  } else {
    return _runArchive();
  }

}

//////////////////////////////////////////////////
// Run in realtime mode

int HcrTempRxGain::_runRealtime()
{

  int iret = 0;
  time_t prevTime = 0;

  while (true) {

    PMU_auto_register("reading realtime data");

    time_t now = time(NULL);
    int secsSincePrev = now - prevTime;
    if (secsSincePrev >= _params.realtime_wait_interval_secs) {
      if (_processRealtime(now)) {
        iret = -1;
      }
      prevTime = now;
    }
    umsleep(100);

  } // while

  return iret;

}

//////////////////////////////////////////////////
// Run in archive mode

int HcrTempRxGain::_runArchive()
{

  int iret = 0;

  time_t archiveStartTime = DateTime::parseDateTime(_params.archive_start_time);
  if (archiveStartTime == DateTime::NEVER) {
    cerr << "ERROR - HcrTempRxGain::_runArchive()" << endl;
    cerr << "  Bad start time: " << _params.archive_start_time << endl;
    return -1;
  }

  time_t archiveEndTime = DateTime::parseDateTime(_params.archive_end_time);
  if (archiveEndTime == DateTime::NEVER) {
    cerr << "ERROR - HcrTempRxGain::_runArchive()" << endl;
    cerr << "  Bad end time: " << _params.archive_end_time << endl;
    return -1;
  }

  time_t archiveTime = archiveStartTime;
  while (archiveTime <= archiveEndTime) {
    if (_processArchive(archiveTime)) {
      iret = -1;
    }
    archiveTime += _params.archive_processing_interval_secs;
  }

  return iret;

}

//////////////////////////////////////////////////
// Process realtime mode

int HcrTempRxGain::_processRealtime(time_t now)
{
  
  time_t retrieveStart = 
    now - _params.lna_temperature_time_lag_secs - _params.temperature_smoothing_interval_secs;
  time_t retrieveEnd = 
    now + _params.lna_temperature_time_lag_secs + _params.temperature_smoothing_interval_secs;

  if (_retrieveFromSpdb(retrieveStart, retrieveEnd)) {
    cerr << "ERROR - HcrTempRxGain::_processRealtime()" << endl;
    return -1;
  }

  if (_processTime(now)) {
    cerr << "ERROR - HcrTempRxGain::_processRealtime()" << endl;
    return -1;
  }

  return 0;

}

//////////////////////////////////////////////////
// Process archive mode

int HcrTempRxGain::_processArchive(time_t archiveTime)
{
  
  time_t retrieveStart = 
    archiveTime -
    _params.lna_temperature_time_lag_secs -
    _params.temperature_smoothing_interval_secs;

  time_t retrieveEnd = 
    archiveTime +
    _params.archive_processing_interval_secs +
    _params.lna_temperature_time_lag_secs +
    _params.temperature_smoothing_interval_secs;
  
  if (_retrieveFromSpdb(retrieveStart, retrieveEnd)) {
    cerr << "ERROR - HcrTempRxGain::_processArchive()" << endl;
    return -1;
  }

  int iret = 0;
  for (time_t procTime = archiveTime; 
       procTime < archiveTime + _params.archive_processing_interval_secs;
       procTime++) {

    if (_processTime(procTime)) {
      cerr << "ERROR - HcrTempRxGain::_processRealtime()" << endl;
      iret = -1;
    }

  }

  return iret;

}

//////////////////////////////////////////////////
// Retrieve temp data from SPDB

int HcrTempRxGain::_retrieveFromSpdb(time_t retrieveStart, time_t retrieveEnd)
  
{
  
  // read XML status from spdb

  DsSpdb spdb;

  if (_params.debug) {
    cerr << "Retrieving XML status from spdb, url: " << _params.input_spdb_url << endl;
    cerr << "  retrieveStart: " << DateTime::strm(retrieveStart) << endl;
    cerr << "  retrieveEnd: " << DateTime::strm(retrieveEnd) << endl;
  }
  
  if (spdb.getInterval(_params.input_spdb_url,
                       retrieveStart, retrieveEnd)) {
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "WARNING - HcrTempRxGain::_retrieveFromSpdb" << endl;
      cerr << "  Cannot read data from URL: " << _params.input_spdb_url << endl;
      cerr << "  startTime: " << DateTime::strm(retrieveStart) << endl;
      cerr << "  endTime: " << DateTime::strm(retrieveEnd) << endl;
      cerr << spdb.getErrStr() << endl;
    }
    return -1;
  }
  
  // got chunks
  
  const vector<Spdb::chunk_t> &chunks = spdb.getChunks();
  if (chunks.size() < 1) {
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "WARNING - RadxPartRain::_retrieveSiteTempFromSpdb" << endl;
      cerr << "  No temp data from URL: " << _params.input_spdb_url << endl;
      cerr << "  startTime: " << DateTime::strm(retrieveStart) << endl;
      cerr << "  endTime: " << DateTime::strm(retrieveEnd) << endl;
    }
    return -1;
  }

  // loop through chunks
  
  for (size_t ii = 0; ii < chunks.size(); ii++) {

    // create string from chunk, ensure null termination
    const Spdb::chunk_t &chunk = chunks[ii];
    string xml((char *) chunk.data, chunk.len - 1);
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "================ XML STATUS =================" << endl;
      cerr << "Valid time: " << DateTime::strm(chunk.valid_time) << endl;
      cerr << xml << endl;
      cerr << "=============================================" << endl;
    }

  } // ii

  return 0;

}

//////////////////////////////////////////////////
// Process the data for the specified time

int HcrTempRxGain::_processTime(time_t procTime)

{

  return 0;

}


