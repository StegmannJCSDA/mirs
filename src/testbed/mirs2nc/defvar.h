/****************************************************************
 *  Define and implement templates: dfvar, put_vatt and put_gatt
 *
 *         Called by: gnrt_nc_img.cpp, gnrt_nc_snd.cpp
 ***************************************************************/

template <typename T>
void  dfvar(int ncid, const char *name, T xtype, int ndims, const int *dimidsp, int *varidp);

template <typename T, typename U>
void  put_vatt(int ncid, int vid, const char *name, T xtype, size_t len, const U *varidp);

template <typename T, typename U>
void  put_gatt(int ncid, const char *name, T xtype, size_t len, const U *varidp);

/* Output variables of SND and ING to NC file */
template <typename T>
void put_nc_snd_img( int ncid, const char* name, const size_t *start, const size_t *count, const T *op ); 


/*--------------------------------------------------------
 * Define veriable, xtype can be int, float, short etc.
 *------------------------------------------------------*/
template <typename T>
void  dfvar(int ncid, const char *name, T xtype, int ndims, const int *dimidsp, int *varidp)
{
  int stat, errid = 15;
  stat = nc_def_var(ncid, name, xtype, ndims, dimidsp, varidp);
  check_err(stat,__LINE__,__FILE__,errid);
}

/*------------------------------------------------------------------------------
* Write out variable attribute, xtype and varidp can be int, float, short etc.
*------------------------------------------------------------------------------*/
template <typename T, typename U>
void  put_vatt(int ncid, int vid, const char *name, T xtype, size_t len, const U *varidp)
{
  int stat, errid = 15;
  stat = nc_put_att(ncid, vid, name, xtype, len, varidp);
  check_err(stat,__LINE__,__FILE__,errid);
}

/*------------------------------------------------------------------------------
* Write out global attribute, xtype and varidp can be int, float, short etc.
*------------------------------------------------------------------------------*/
template <typename T, typename U>
void  put_gatt(int ncid, const char *name, T xtype, size_t len, const U *varidp)
{
  int stat, errid = 15;
  stat = nc_put_att(ncid, NC_GLOBAL, name, xtype, len, varidp);
  check_err(stat,__LINE__,__FILE__,errid);
}

/*------------------------------
 * Write data in op to NC file
 *-----------------------------*/
template <typename T>
void put_nc_snd_img( int ncid, const char* name, const size_t *start, const size_t *count, const T *op )
{
  int  stat;     /* return status */
  int  errid = 16;
  int  varid;
  if ( (stat = nc_inq_varid(ncid, name, &varid)) )
    check_err(stat,__LINE__,__FILE__,errid);
  if ( (stat = nc_put_vara(ncid, varid, start, count, op)))
    check_err(stat,__LINE__,__FILE__,errid);
  if( op && strcmp (name, "SurfP") !=0 ) //NC files of IMG need SurfP too
    delete[] op;
}

