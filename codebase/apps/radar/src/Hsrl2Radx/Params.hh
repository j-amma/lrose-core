/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR                                                         */
/* ** University Corporation for Atmospheric Research (UCAR)                 */
/* ** National Center for Atmospheric Research (NCAR)                        */
/* ** Boulder, Colorado, USA                                                 */
/* ** BSD licence applies - redistribution and use in source and binary      */
/* ** forms, with or without modification, are permitted provided that       */
/* ** the following conditions are met:                                      */
/* ** 1) If the software is modified to produce derivative works,            */
/* ** such modified software should be clearly marked, so as not             */
/* ** to confuse it with the version available from UCAR.                    */
/* ** 2) Redistributions of source code must retain the above copyright      */
/* ** notice, this list of conditions and the following disclaimer.          */
/* ** 3) Redistributions in binary form must reproduce the above copyright   */
/* ** notice, this list of conditions and the following disclaimer in the    */
/* ** documentation and/or other materials provided with the distribution.   */
/* ** 4) Neither the name of UCAR nor the names of its contributors,         */
/* ** if any, may be used to endorse or promote products derived from        */
/* ** this software without specific prior written permission.               */
/* ** DISCLAIMER: THIS SOFTWARE IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESS  */
/* ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/* ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
////////////////////////////////////////////
// Params.hh
//
// TDRP header file for 'Params' class.
//
// Code for program Hsrl2Radx
//
// This header file has been automatically
// generated by TDRP, do not modify.
//
/////////////////////////////////////////////

/**
 *
 * @file Params.hh
 *
 * This class is automatically generated by the Table
 * Driven Runtime Parameters (TDRP) system
 *
 * @class Params
 *
 * @author automatically generated
 *
 */

#ifndef Params_hh
#define Params_hh

#include <tdrp/tdrp.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cfloat>

using namespace std;

// Class definition

class Params {

public:

  // enum typedefs

  typedef enum {
    DEBUG_OFF = 0,
    DEBUG_NORM = 1,
    DEBUG_VERBOSE = 2,
    DEBUG_EXTRA = 3
  } debug_t;

  typedef enum {
    REALTIME_FMQ = 0,
    REALTIME_FILE = 1,
    ARCHIVE = 2,
    FILELIST = 3
  } mode_t;

  typedef enum {
    DEGREES_CELCIUS = 0,
    DEGREES_KELVIN = 1
  } temperature_units_t;

  typedef enum {
    PA = 0,
    HPA = 1
  } pressure_units_t;

  typedef enum {
    START_AND_END_TIMES = 0,
    START_TIME_ONLY = 1,
    END_TIME_ONLY = 2,
    SPECIFY_FILE_NAME = 3
  } filename_mode_t;

  // struct typedefs

  typedef struct {
    char* field_name;
    tdrp_bool_t apply_mask;
    char* mask_name;
    int mask_valid_value;
    char* output_name;
    char* cf_standard_name;
  } mhayman_field_t;

  ///////////////////////////
  // Member functions
  //

  ////////////////////////////////////////////
  // Default constructor
  //

  Params ();

  ////////////////////////////////////////////
  // Copy constructor
  //

  Params (const Params&);

  ////////////////////////////////////////////
  // Destructor
  //

  ~Params ();

  ////////////////////////////////////////////
  // Assignment
  //

  void operator=(const Params&);

  ////////////////////////////////////////////
  // loadFromArgs()
  //
  // Loads up TDRP using the command line args.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   char **params_path_p:
  //     If this is non-NULL, it is set to point to the path
  //     of the params file used.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int loadFromArgs(int argc, char **argv,
                   char **override_list,
                   char **params_path_p,
                   bool defer_exit = false);

  bool exitDeferred() { return (_exitDeferred); }

  ////////////////////////////////////////////
  // loadApplyArgs()
  //
  // Loads up TDRP using the params path passed in, and applies
  // the command line args for printing and checking.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   const char *param_file_path: the parameter file to be read in
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int loadApplyArgs(const char *params_path,
                    int argc, char **argv,
                    char **override_list,
                    bool defer_exit = false);

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  //

  static bool isArgValid(const char *arg);

  ////////////////////////////////////////////
  // load()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to load
  // up more than one class for a single application. It is a
  // lower-level routine than loadFromArgs, and hence more
  // flexible, but the programmer must do more work.
  //
  //   const char *param_file_path: the parameter file to be read in.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int load(const char *param_file_path,
           char **override_list,
           int expand_env, int debug);

  ////////////////////////////////////////////
  // loadFromBuf()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to
  // load up more than one module for a single application,
  // using buffers which have been read from a specified source.
  //
  //   const char *param_source_str: a string which describes the
  //     source of the parameter information. It is used for
  //     error reporting only.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   const char *inbuf: the input buffer
  //
  //   int inlen: length of the input buffer
  //
  //   int start_line_num: the line number in the source which
  //     corresponds to the start of the buffer.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int loadFromBuf(const char *param_source_str,
                  char **override_list,
                  const char *inbuf, int inlen,
                  int start_line_num,
                  int expand_env, int debug);

  ////////////////////////////////////////////
  // loadDefaults()
  //
  // Loads up default params for a given class.
  //
  // See load() for more detailed info.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int loadDefaults(int expand_env);

  ////////////////////////////////////////////
  // sync()
  //
  // Syncs the user struct data back into the parameter table,
  // in preparation for printing.
  //
  // This function alters the table in a consistent manner.
  // Therefore it can be regarded as const.
  //

  void sync() const;

  ////////////////////////////////////////////
  // print()
  // 
  // Print params file
  //
  // The modes supported are:
  //
  //   PRINT_SHORT:   main comments only, no help or descriptions
  //                  structs and arrays on a single line
  //   PRINT_NORM:    short + descriptions and help
  //   PRINT_LONG:    norm  + arrays and structs expanded
  //   PRINT_VERBOSE: long  + private params included
  //

  void print(FILE *out, tdrp_print_mode_t mode = PRINT_NORM);

  ////////////////////////////////////////////
  // checkAllSet()
  //
  // Return TRUE if all set, FALSE if not.
  //
  // If out is non-NULL, prints out warning messages for those
  // parameters which are not set.
  //

  int checkAllSet(FILE *out);

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int checkIsSet(const char *param_name);

  ////////////////////////////////////////////
  // arrayRealloc()
  //
  // Realloc 1D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int arrayRealloc(const char *param_name,
                   int new_array_n);

  ////////////////////////////////////////////
  // array2DRealloc()
  //
  // Realloc 2D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int array2DRealloc(const char *param_name,
                     int new_array_n1,
                     int new_array_n2);

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void freeAll(void);

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  static void usage(ostream &out);

  ///////////////////////////
  // Data Members
  //

  char _start_; // start of data region
                // needed for zeroing out data
                // and computing offsets

  debug_t debug;

  char* instance;

  char* input_dir;

  mode_t mode;

  char* input_fmq_path;

  int max_realtime_data_age_secs;

  tdrp_bool_t latest_data_info_avail;

  tdrp_bool_t search_recursively;

  int max_recursion_depth;

  int wait_between_checks;

  int file_quiescence;

  char* search_ext;

  tdrp_bool_t override_start_range;

  double start_range_km;

  tdrp_bool_t override_gate_spacing;

  double gate_spacing_km;

  tdrp_bool_t read_georef_data_from_aircraft_system;

  char* georef_data_spdb_url;

  int georef_data_search_margin_secs;

  tdrp_bool_t correct_elevation_angle_for_roll;

  double instrument_latitude_deg;

  double instrument_longitude_deg;

  double instrument_altitude_meters;

  tdrp_bool_t set_range_relative_to_instrument;

  int telescope_direction_is_up;

  tdrp_bool_t combine_bins_on_read;

  int n_bins_per_gate;

  double raw_bin_start_range_km;

  double raw_bin_spacing_km;

  char* combined_hi_field_name;

  char* combined_lo_field_name;

  char* molecular_field_name;

  char* cross_field_name;

  char* baseline_calibration_path;

  char* diff_default_geofile_path;

  char* geofile_default_path;

  char* afterpulse_default_path;

  char* calvals_gvhsrl_path;

  char* combined_hi_dead_time_name;

  char* combined_lo_dead_time_name;

  char* cross_pol_dead_time_name;

  char* molecular_dead_time_name;

  int molecular_count_median_filter_len;

  char* bin_width_name;

  char* scan_adjustment_name;

  char* molecular_gain_name;

  int ngates_for_background_correction;

  int nrays_for_background_correction;

  double optical_depth_reference_range_m;

  int optical_depth_n_reference_obs;

  double optical_depth_reference_value;

  int optical_depth_median_filter_len;

  double rate_censoring_threshold;

  tdrp_bool_t apply_speckle_filter;

  int speckle_filter_len;

  tdrp_bool_t mhayman_combine_bins_on_read;

  int mhayman_n_bins_per_gate;

  tdrp_bool_t mhayman_specify_fields;

  mhayman_field_t *_mhayman_fields;
  int mhayman_fields_n;

  int combined_high_count_threshold_for_backscat_coeff;

  int combined_high_count_threshold_for_vol_depol_ratio;

  tdrp_bool_t read_temp_and_pressure_profile_from_model_files;

  char* model_temperature_field_name;

  temperature_units_t temperature_profile_units;

  char* model_pressure_field_name;

  pressure_units_t pressure_profile_units;

  char* model_profile_mdv_data_url;

  int model_profile_search_margin_secs;

  int compression_level;

  tdrp_bool_t override_instrument_name;

  char* instrument_name;

  tdrp_bool_t override_site_name;

  char* site_name;

  char* output_dir;

  filename_mode_t output_filename_mode;

  char* output_filename_prefix;

  tdrp_bool_t include_instrument_name_in_file_name;

  tdrp_bool_t include_subsecs_in_file_name;

  tdrp_bool_t use_hyphen_in_file_name_datetime_part;

  char* output_filename;

  tdrp_bool_t append_day_dir_to_output_dir;

  tdrp_bool_t append_year_dir_to_output_dir;

  tdrp_bool_t write_latest_data_info;

  tdrp_bool_t write_output_files_on_time_boundaries;

  int output_file_time_interval_secs;

  tdrp_bool_t censor_gates_below_surface;

  double surface_height_for_censoring;

  char* version_override;

  char* title_override;

  char* institution_override;

  char* references_override;

  char* source_override;

  char* history_override;

  char* comment_override;

  char* author_override;

  char* output_fmq_url;

  int output_fmq_size;

  int output_fmq_nslots;

  tdrp_bool_t output_fmq_compress;

  int nrays_for_params;

  tdrp_bool_t write_blocking;

  int data_mapper_report_interval;

  char _end_; // end of data region
              // needed for zeroing out data

private:

  void _init();

  mutable TDRPtable _table[118];

  const char *_className;

  bool _exitDeferred;

};

#endif

