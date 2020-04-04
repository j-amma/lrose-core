
// Bad flags can be set using values in one data field,
// then applied to another data field.
// So, the flag "mask" survives from one function to another.
// The bad flag mask is like a boundary mask.
// Pass in a bad_flag_mask, set it using a data vector and some criteria,
//  then, pass back the bad_flag_mask for use in another function.


// need a bad flags mask array; will this be like a boundary mask??
// need to be persistent? static?
// Q: how is bad flag mask used?
// editor/se_for_each.c:      seds->bad_flag_mask = flag = seds->bad_flag_mask_array;
//      for flagged-copy ... consider bad_flag_mask and boundary_mask
// editor/se_defrkl.c:    seds->bad_flag_mask = flag = seds->bad_flag_mask_array;
//      for flag-freckles ... consider bad_flag_mask and boundary_mask
//      for flag-glitches ...    "       "
//bool *bad_flag_mask_array;  // unsigned short, originally in Soloii
//bool *bad_flag_mask;        // unsigned short, originally in Soloii
//bool use_bad_flag_mask;     // int, originally in Soloii


/*
 * se_assert_bad_flags
 * se_bad_flags_logic
 * se_clear_bad_flags
 * se_copy_bad_flags
 * se_set_bad_flags
 * 
 */





/* c------------------------------------------------------------------------ */

void se_do_clear_bad_flags_array(bool *bad_flag_mask, size_t nn)
{

  memset(bad_flag_mask, 0, nn * sizeof(bool)); 
  // (nn > 0 ? nn : seds->max_gates) * sizeof(*seds->bad_flag_mask_array));
}
/* c------------------------------------------------------------------------ */

//int se_set_bad_flags(arg, cmds)	/* #set-bad-flags# */
//  int arg;
//  struct ui_command *cmds;
// 
// In params:
// data    pointer to data values
// bad     value to indicate bad value in data

// In/Out params:
// bad_flags_mask send preallocated space, same size as data
//                returns boolean array: 
//                  true if data value meets bad flag criteria AND in inside boundary mask
void se_set_bad_flags(char *where, float scaled_thr1, float scaled_thr2, const float *data, size_t nGates,
		      float bad, size_t dgi_clip_gate, bool *boundary_mask, bool *bad_flag_mask)
{
  //int below;
  //struct ui_command *cmdq=cmds+1; /* point to the first argument */
  //struct solo_edit_stuff *seds, *return_sed_stuff();
  //struct dd_general_info *dgi, *dd_window_dgi();
  //struct dds_structs *dds;
    //char *name, *dst_name, *where;
    //float f_thr1, f_thr2;

  size_t nc; 
  //int nd, nchar, bad, thr_bad, fn, fgg;
    //int gg, ii, jj, kk, nn, scaled_thr1, scaled_thr2, mark, fthr;
    //short *anchor, *ss, *zz; // , *thr=NULL;
    float *zz;
    float *thr;
    bool *bnd, *flag; // was unsigned short


    //name = (cmdq++)->uc_text;
    //nchar = strlen(name);
    //where = (cmdq++)->uc_text;
    //f_thr1 = (cmdq++)->uc_v.us_v_float;
    //if(cmdq->uc_ctype != UTT_END)
    //  f_thr2 = cmdq->uc_v.us_v_float;

    //seds = return_sed_stuff();
    //if(seds->finish_up) {
    //	return(1);
    //}
    //seds->modified = YES;
    // which frame is active; i.e. which frame to apply command
    //dgi = dd_window_dgi(seds->se_frame);
    //dds = dgi->dds;
    nc = dgi_clip_gate+1;
    // fgg = seds_first_good_gate; // not used??
    bnd = boundary_mask;
    flag = bad_flag_mask;
    //seds->bad_flag_mask = flag = seds->bad_flag_mask_array;
    //
    // find the thr field; the data are sent as an argument
    //
    //if((fn = dd_find_field(dgi, name)) < 0) {
    // thr field not found
    //
    // uii_printf("Set bad flags field: %s not found\n", name);
    // seds->punt = YES;
    // return(-1);
    //}
    // fn is an index for the data field
    // thr is a pointer to the data
    //# ifdef NEW_ALLOC_SCHEME
    //thr = (short *)dds->qdat_ptrs[fn];
    //# else
    //thr = (short *)((char *)dds->rdat[fn] + sizeof(struct paramdata_d));
    //# endif
    thr = data;
    //thr_bad = bad; // dds->parm[fn]->bad_data;
    zz = thr +nc;
    //    scaled_thr1 = DD_SCALE(f_thr1, dds->parm[fn]->parameter_scale
    //		  , dds->parm[fn]->parameter_bias);
    //scaled_thr2 = DD_SCALE(f_thr2, dds->parm[fn]->parameter_scale
    //		  , dds->parm[fn]->parameter_bias);
    //bad = dds->parm[fn]->bad_data;
    se_do_clear_bad_flags_array(nc, bad_flag_mask);
   
    /*
     * loop through the data
     */

    if(strncmp(where, "below", 3) == 0) {
	for(; thr < zz; thr++,bnd++,flag++) {
	    if(!(*bnd) || *thr == bad)
	      *flag = false; // continue;
	    if(*thr < scaled_thr1) {
		*flag = true;
	    }
	}
    }
    else if(strncmp(where, "above", 3) == 0) {
	for(; thr < zz; thr++,bnd++,flag++) {
	    if(!(*bnd) || *thr == bad)
	      *flag = false; // continue;
	    if(*thr > scaled_thr1) {
		*flag = true;
	    }
	}
    }
    else {			/* between */
	for(; thr < zz; thr++,bnd++,flag++) {
	    if(!(*bnd) || *thr == bad)
	      *flag = false; // continue;
	    if(*thr >= scaled_thr1 && *thr <= scaled_thr2) {
		*flag = true;
	    }
	}
    }
    //    return(fn);
}  
/* c------------------------------------------------------------------------ */


/* c------------------------------------------------------------------------ */

/* #assert-bad-flags# */

void se_assert_bad_flags(const float *data, float *newData, size_t nGates,
			 float bad, size_t dgi_clip_gate,
			 bool *boundary_mask, bool *bad_flag_mask)
{
  //size_t  nc; //  nd, fn;
  //int ii, nn, mark;
  //float *ss, *zz, *orig;
    bool *bnd, *flag;

    //nc = clip_gate+1;
    bnd = boundary_mask;
    flag = bad_flag_mask;
    //orig = data + nc;
    //ss = newData;
    //nc = dgi_clip_gate;
    //zz = ss + nc;
    
    /*
     * loop through the data
     */

    for(; ss < zz; ss++,bnd++,flag++,orig++) {
      if(*bnd && *flag)
	*ss = bad;
      else 
	*ss = *orig;
    }

    /* or using index ...
    size_t endIndex = dgi_clip_gate;
    if (endIndex > nGates)
      endIndex = nGates;
    for (size_t index = 0; index < endIndex; index++) {
      if (bnd[index] && flag[index])
	newData[index] = bad;
      else
	newData[index] = data[index];

    }
    */
}  
/* c------------------------------------------------------------------------ */

// add or multiply each flagged data value by a constant
void se_flagged_add(float f_const, bool multiply, const float *data, float *newData, size_t nGates,
			 float bad, size_t dgi_clip_gate,
			 bool *boundary_mask, bool *bad_flag_mask)
// #flagged-add# 
{
  // #flagged-add#
  // #flagged-multiply#
  //
    char *name, *where;

    size_t nc;
    //  bad, fn;
    int gg, ii, jj, kk, nn, scaled_const, mark;
    float *ss, *zz, *orig;
    bool *bnd, *flag;

    //f_const = (cmdq++)->uc_v.us_v_float;
    //name = (cmdq++)->uc_text;

    nc = dgi_clip_gate+1;
    bnd = boundary_mask;
    flag = bad_flag_mask;
    ss = newData;
    zz = ss +nc;
    orig = data;
 
    //
    // loop through the data
    //

    if(multiply) { // strstr(cmds->uc_text, "d-m")) { // multiply 
      for(; ss < zz; ss++,bnd++,flag++,orig++) {
	if(*bnd && *ss != bad && *flag) {
	  *ss *= f_const;
	} else
	  *ss = *orig;
      }
    }
    else {			// add 
      for(; ss < zz; ss++,bnd++,flag++,orig++) {
	if(*bnd && *ss != bad && *flag) {
	  *ss += f_const;
	} else
	  *ss = *orig;
      }
    }
}
/* c------------------------------------------------------------------------ */

// sets bad_flag_mask In/out parameter
void se_bad_flags_logic(float scaled_thr1, float scaled_thr2, char *where, 
			char *logical_operator, const float *data, size_t nGates,
			 float bad, size_t dgi_clip_gate,
			 bool *boundary_mask, bool *bad_flag_mask)
{
  //
  // #and-bad-flags#
  // #or-bad-flags#
  // #xor-bad-flags#
  //

    size_t nc; // , nchar, bad, fn;
    int gg, ii, jj, kk, nn; // , scaled_thr1, scaled_thr2, mark;
    float *ss, *zz, *thr=NULL;
    bool *bnd, *flag;

    nc = dgi_clip_gate+1;
    bnd = boundary_mask;
    flag = bad_flag_mask;
    thr = data;

    zz = thr +nc;

    /*
     * loop through the data
     */

    if(strncmp(where, "below", 3) == 0) {
	if(strncmp(logical_operator, "and", 3) == 0) {
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr == bad) {
		    *flag = false;
		}
		else {
		    *flag &= *thr < scaled_thr1;
		}
	    }
	}
	else if(strncmp(logical_operator, "xor", 3) == 0) {
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr != bad)
		      *flag ^= *thr < scaled_thr1;
	    }
	}
	else {			/* or */
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr != bad)
		      *flag |= *thr < scaled_thr1;
	    }
	}
    }
    else if(strncmp(where, "above", 3) == 0) {
	if(strncmp(logical_operator, "and", 3) == 0) {
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr == bad) {
		    *flag = false;
		}
		else {
		    *flag &= *thr > scaled_thr1;
		}
	    }
	}
	else if(strncmp(logical_operator, "xor", 3) == 0) {
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr != bad)
		      *flag ^= *thr > scaled_thr1;
	    }
	}
	else {			/* or */
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr != bad)
		      *flag |= *thr > scaled_thr1;
	    }
	}
    }
    else {			/* between */
	if(strncmp(logical_operator, "and", 3) == 0) {
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr == bad) {
		    *flag = false;
		}
		else {
		    *flag &= *thr >= scaled_thr1 && *thr <= scaled_thr2;
		}
	    }
	}
	else if(strncmp(logical_operator, "xor", 3) == 0) {
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr != bad) {
		    *flag ^= *thr >= scaled_thr1 && *thr <= scaled_thr2;
		}
	    }
	}v
	else {			/* or */
	    for(; thr < zz; thr++,bnd++,flag++) {
		if(!(*bnd))
		      continue;
		if(*thr != bad) {
		    *flag |= *thr >= scaled_thr1 && *thr <= scaled_thr2;
		}
	    }
	}
    }
}
/* c------------------------------------------------------------------------ */

void se_clear_bad_flags(bool complement, size_t nGates,
			bool *bad_flag_mask)
{
  /* #clear-bad-flags#       complement == false
   * #complement-bad-flags#  complement == true
   */
  size_t nn;
  bool *flag;


    if(complement) {
      flag = bad_flag_mask;
      nn = nGates;
      for(; nn--; flag++) {
	*flag = *flag ? 0 : 1;
      }
    }
    else {
      se_do_clear_bad_flags_array(flag, nGates);
    }
}  
/* c------------------------------------------------------------------------ */

// bad_flag_mask In/Out parameter; flags outside boundary are NOT changed.
//
void se_copy_bad_flags(const float *data, size_t nGates,
			 float bad, size_t dgi_clip_gate,
			 bool *boundary_mask, bool *bad_flag_mask)
// #copy-bad-flags# 
{
    /* creates a bad flag mask corresponding to the bad flagged
     * gates in the test field
     */

    char *name, *where;

    size_t nc;
    //  nd, nchar, bad, thr_bad, fn, fgg;
    int gg, ii, jj, kk, nn, scaled_thr1, scaled_thr2, mark;
    short *anchor, *ss, *zz, *thr=NULL;
    bool *bnd, *flag;


    nc = dgi_clip_gate+1;
    fgg = seds->first_good_gate;
    bnd = boundary_mask;
    flag = bad_flag_mask;
    /*
     * find the thr field
     */
    thr = data; // ??? (short *)((char *)dds->rdat[fn] + sizeof(struct paramdata_d));

    thr_bad = bad; // dds->parm[fn]->bad_data;
    zz = thr +nc;
    //bad = dds->parm[fn]->bad_data;

    /*
     * loop through the data
     */

    for(; thr < zz; thr++,bnd++,flag++) {
	if(!(*bnd))
	      continue;
	*flag = *thr == bad ? true : false;
    }
}  



/* c------------------------------------------------------------------------ */

void se_flag_glitches(const float *data, float *newData, size_t nGates,
			 float bad, size_t dgi_clip_gate,
			 bool *boundary_mask, bool *bad_flag_mask) 
// #flag-glitches# 
{
  /* routine to remove discountinuities (freckles) from the data                                    
   * such as birds and radio towers by comparing a particular                                       
   * data point to a running average that is ahead of the point of interest                         
   * but switches to a trailing average once enough good points                                     
   * have been encountered                                                                          
   */
  struct ui_command *cmdq=cmds+1; /* point to the first argument */
  int fn, ii, jj, kk, mm, nn, mark, navg;
  int nc, ndx_ss, bad, scaled_thr;
  int ndx_qend, ndx_qctr, min_bins, good_bins, half, sum, ival;
  double rcp_ngts, davg, diff;
  char *name;
  static int que_size = 0;
  static int *que, *qctr;

  struct solo_edit_stuff *seds, *return_sed_stuff();
  struct dd_general_info *dgi, *dd_window_dgi();
  struct dds_structs *dds;

  short *ss;
  unsigned short *bnd, *flag;
  /* boundary mask is set to 1 if the corresponding cell satisfies                                  
   * conditions of the boundaries                                                                   
   */

  seds = return_sed_stuff();  /* solo editing struct */
  if(seds->finish_up) {
    return(1);
  }
  seds->modified = YES;
  bnd = seds->boundary_mask;
  seds->bad_flag_mask = flag = seds->bad_flag_mask_array;
  dgi = dd_window_dgi(seds->se_frame);
  dds = dgi->dds;
  name = (cmdq++)->uc_text;
  mm = strlen(name);

  if((fn = dd_find_field(dgi, name)) < 0) {
    uii_printf("Field to be deglitched: %s not found\n", name);
    seds->punt = YES;
    return(-1);
  }
# ifdef NEW_ALLOC_SCHEME
  ss = (short *)dds->qdat_ptrs[fn];
# else
  ss = (short *)((char *)dds->rdat[fn] +sizeof(struct paramdata_d));
# endif
  scaled_thr = DD_SCALE(seds->deglitch_threshold
			, dds->parm[fn]->parameter_scale
			, dds->parm[fn]->parameter_bias);
  bad = dds->parm[fn]->bad_data;
  if( seds->deglitch_radius < 1 )
    { seds->deglitch_radius = 3; }
  navg = seds->deglitch_radius *2 +1;
  half = navg/2;

  if( seds->deglitch_min_bins > 0 ) {
    if( seds->deglitch_min_bins > navg )
      { seds->deglitch_min_bins = navg; }
    min_bins = seds->deglitch_min_bins;
  }
  else
    { seds->deglitch_min_bins = min_bins = navg; }

  if(seds->process_ray_count == 1) {
    /*                                                                                            
     * set up                                                                                     
     */
    if( navg > que_size ) {
      if( que_size )
	{ free( que ); }
      que = (int *)malloc( navg * sizeof( int ));
      que_size = navg;
    }
  }
  nc = dgi->clip_gate +1;

  for( ndx_ss = 0; ndx_ss < nc; ) {
    /*                                                                                             
     * move the cell index to the first                                                            
     * gate inside the next boundary                                                               
     */
    for(; ndx_ss < nc && !(*(bnd + ndx_ss)); ndx_ss++ );
    /*                                                                                            
     * set up the queue                                                                           
     */
    ndx_qend = 0;
    good_bins = 0;
    sum = 0;

    /* and start looking for the good gates count to equal or                                     
     * exceed the min_bins and the center bin not be bad                                          
     */

    for(mm = 0; ndx_ss < nc && *(bnd+ndx_ss); ndx_ss++ ) {
      if( ++mm > navg ) {  /* after the que is full */
	ival = *(que + ndx_qend);
	if( ival != bad )
	  { good_bins--; sum -= ival; }
      }
      ival = *(ss + ndx_ss); /* raw data value */
      *(que + ndx_qend) = ival;
      if( ival != bad )
	{ sum += ival; good_bins++; }
      else
	{ mark = 0; }

      ndx_qctr = (ndx_qend - half + navg ) % que_size;
      qctr = que + ndx_qctr;

      if( good_bins >= min_bins && *qctr != bad ) {
	/*                                                                                      
	 * do a test                                                                            
	 */
	davg = (double)(sum - *qctr)/(double)(good_bins -1);
	if(( diff = FABS( davg - *qctr)) > scaled_thr ) {
	  sum -= *qctr;
	  good_bins--;
	  *qctr = bad;
	  ii = ndx_ss - half;
	  *(flag + ndx_ss - half) = 1; /* flag this gate */
	}
      }
      ndx_qend = ++ndx_qend % que_size;
    }
  }
}
/* c------------------------------------------------------------------------ */
