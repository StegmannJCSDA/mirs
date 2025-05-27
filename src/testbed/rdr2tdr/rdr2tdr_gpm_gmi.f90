!===============================================================================
! Name:       rdr2tdr_gpm_gmi.f90
!
!
! Type:       Main Program
!
!
! Description:
!        Program to convert raw data of GPM/GMI into MIRS TDRs format.
!        Adapted from NASA GMAO gpm bufr converter.
!
! Modules needed:
!       - Consts
!       - misc
!       - IO_MeasurData
!       - IO_Noise
!       - IO_Misc
!       - USE ErrorHandling
!
! History:
!       11-11-2014      Dr. Tanvir Islam                   
!                       NOAA/NESDIS/STAR & CIRA/CSU       Original Coder
!
!===============================================================================

PROGRAM rdr2tdr_gpm_gmi

  USE Consts
  USE misc
  USE IO_MeasurData
  USE IO_Noise
  USE IO_Misc
  USE ErrorHandling

  USE HDF5

  !-----------------------------------------------------------------------------
  ! Declarations
  !-----------------------------------------------------------------------------
  IMPLICIT NONE

  !----Pointers and other allocatable arrays ----
  CHARACTER(LEN=256), DIMENSION(:), POINTER :: rdrFilesGmi,tdrFilesGmi

  !----Namelist data ---- 
  CHARACTER(LEN=256)                        :: rdrfileList=DEFAULT_VALUE_STR4
  CHARACTER(LEN=256)                        :: rdrfileList2=DEFAULT_VALUE_STR4
  CHARACTER(LEN=256)                        :: pathTDR=DEFAULT_VALUE_STR4
  INTEGER                                   :: norbits2process=DEFAULT_VALUE_INT
  CHARACTER(LEN=256)                        :: logFile=DEFAULT_VALUE_STR4

  !---- Specify global varables ----
  INTEGER                                   :: nfile,ifile
  INTEGER                                   :: iu_list
  INTEGER(4)                                :: nscan
  INTEGER                                   :: npixel 
  CHARACTER(LEN=256)                        :: granule_name

  !----HDF Function declarations----
  INTEGER(HID_T)                            :: file_id, group_id, dset_id, attr_id, space_id, &
       memtype_id, type_id, subgroup_id, subdset_id
  INTEGER                                   :: error
  INTEGER(HSIZE_T),DIMENSION(1)             :: attr_dims
  CHARACTER (LEN=1000)                      :: attr_data
  INTEGER                                   :: hdferr         ! Error code:

  CHARACTER(LEN = 80)                       :: groupname, subgroupname, swathheader
  CHARACTER(LEN=40),DIMENSION(2)            :: groupnames=(/'S1','S2'/)
  CHARACTER(LEN=40),DIMENSION(2)            :: gswathheaders=(/'S1_SwathHeader','S2_SwathHeader'/)

  INTEGER                                   :: GranuleNumber,NumberScansBeforeGranule,&
       NumberScansGranule,NumberScansAfterGranule,MissingData

  INTEGER                                   :: dim1,dim2,dim3
  CHARACTER (LEN = 80)                      :: dsetname

  !---- Parameters section ----
  !----------------------------

  INTEGER, PARAMETER :: NFOV = 221,  NCHAN = 13, NCHAN_LOW = 9, NCHAN_HIGH = 4 , NQC = 13
  INTEGER, PARAMETER :: DEBUG=0

  !----- scan time ---- 
  integer, dimension(:), allocatable        :: years
  integer, dimension(:), allocatable        :: months
  integer, dimension(:), allocatable        :: doms
  integer, dimension(:), allocatable        :: hours
  integer, dimension(:), allocatable        :: minutes
  integer, dimension(:), allocatable        :: seconds
  integer, dimension(:), allocatable        :: milliseconds
  integer, dimension(:), allocatable        :: doys
  !----- tb ---- 
  real, dimension(:,:,:), allocatable       :: Tb, Tb1, Tb2
  !---- qc ----
  integer, dimension(:,:,:), allocatable    :: qc
  integer, dimension(:,:), allocatable      :: qc1, qc2
  !---- lat/lon ----
  real, dimension(:,:), allocatable         :: lons
  real, dimension(:,:), allocatable         :: lats
  !---- angle -----
  real, dimension(:,:), allocatable         :: angles

  !-- not get values inside this code, just use default values
  real, dimension(NFOV)                     :: relAziAngles = DEFAULT_VALUE_REAL

  !---- Output variables section ----
  !----------------------------------

  INTEGER                                   :: iu_out ! iu unit for output TDR files
  CHARACTER(LEN=256)                        :: tdrFileGmi ! output TDR file names
  integer                                   :: iscan, ichan, ifov, jfov

  !---- channel central frequency ----
  real, dimension(NCHAN)                    :: freq  

  !---- polarization: vertical or horizontal ( 4:V, 5:H ) ----
  integer, dimension(NCHAN)                 :: polar

  DATA freq  / 10.65, 10.65, 18.70, 18.70, 23.80, 36.50, 36.50, 89.0, 89.0, 166.0, &
       166.0, 183.31, 183.31 /
  DATA polar  / 4, 5, 4, 5, 4, 4, 5, 4, 5, 4, 5, 4, 4 /

  !---- corresponding utc times into mirs format ( in milli seconds ) ----
  integer(4), dimension(:), allocatable     :: utcs

  !---- tbs ----
  real, dimension(NFOV,NCHAN)               :: tbs

  !---- quality flags ----
  integer(1), dimension(:,:), allocatable   :: qcs

  !--- ascending/descending passing mode ( 0: asc ; 1: des ) ----
  integer, dimension(:), allocatable        :: nodes


  NAMELIST /ContrlRDR2TDR/rdrfileList,pathTDR,norbits2process,logFile


  !-------------------------------------------------------------------------------------
  !   Read control-data from namelist
  !-------------------------------------------------------------------------------------
  READ(*,NML=ContrlRDR2TDR)

  !---- open Log file ----
  CALL OpenLogFile(logFile)

  !----Read the file names of GPM-GMI RDR data and build TDR files names, tdrFilesGmi not used here
  call ReadList(iu_list,trim(rdrfileList),rdrFilesGmi,nfile,tdrFilesGmi,pathTDR,'TDR_')
  IF (nfile .lt. 1 ) CALL ErrHandl(ErrorType,Err_NoFilesFound,'')
  nfile=minval((/nfile,norbits2process/))

  !---- loop the files ----
  fileLoop: DO ifile=1,nfile

     granule_name = trim(rdrFilesGmi(ifile))
     write(*,'(A)') trim(granule_name)

     !-----------------------------------------------------------
     ! Open granule/file for access and read
     !-----------------------------------------------------------
     ! Initialize FORTRAN interface.
     CALL h5open_f(error)

     CALL h5fopen_f (TRIM(granule_name), H5F_ACC_RDWR_F, file_id, error)

     if(error/=0) then  
        print*, 'Error. Cannot open the HDF file. Program stop'
        stop
     endif

     !----------------------------------------------------------------------
     !----- get the file header info-----
     call h5aopen_f(file_id, 'FileHeader', attr_id, error)
     call h5aget_type_f(attr_id, type_id, error)
     attr_dims(1)=0
     call h5aread_f(attr_id, type_id, attr_data, attr_dims, error)
     GranuleNumber = inquire_attr_sub(attr_data, 'GranuleNumber')
     MissingData = inquire_attr_sub(attr_data, 'MissingData')
     print *, 'GranuleNumber, MissingData'
     print *, GranuleNumber, MissingData
     call h5aclose_f(attr_id, error)

     !-----------------------------------------------------------------------
     ! open group1 to pull most information
     groupname = trim(groupnames(1)) 
     print *, '-------'
     print *, 'group:', trim(groupnames(1))
     call h5gopen_f(file_id, trim(groupname), group_id, error)

     !----------------------------------------------------------------------- 
     !get the swath header information for S1
     swathheader = trim(gswathheaders(1))
     call h5aopen_f(group_id, trim(swathheader), attr_id, error)
     call h5aget_type_f(attr_id, type_id, hdferr) 

     attr_dims(1)=1
     call h5aread_f(attr_id,type_id, attr_data, attr_dims, error) 
     NumberScansBeforeGranule = inquire_attr_sub(attr_data, 'NumberScansBeforeGranule')
     NumberScansAfterGranule  = inquire_attr_sub(attr_data, 'NumberScansAfterGranule' )
     !nscan = inquire_attr_sub(attr_data, 'MaximumNumberScansTotal')
     nscan = inquire_attr_sub(attr_data, 'NumberScansGranule')
     npixel = inquire_attr_sub(attr_data, 'NumberPixels' )
     print *, 'NumberScansBeforeGranule, NumberScansAfterGranule, nscan, npixel'
     print *,  NumberScansBeforeGranule, NumberScansAfterGranule, nscan, npixel
     call h5aclose_f(attr_id, error)
     ! Terminate access to the SD interface and close the file.
     call h5fclose_f(file_id, error)
     ! Close FORTRAN interface.
     CALL h5close_f(error) 
     if (nscan < 1 ) then 
        print*, 'Number of scans in the HDF file =', nscan
        print*, 'Error. Data are missing in HDF file. Program stopped.'
        stop
     endif

     !-----------------------------------------------------------------------
     allocate (years(nScan),months(nScan),doms(nScan),hours(nScan),minutes(nScan),seconds(nScan),milliseconds(nScan),doys(nScan))
     groupname='S1'
     subgroupname='ScanTime'
     dim1=nScan;           dim2=1;             dim3=1
     dsetname='Year'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,years)
     dsetname='Month'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,months)
     dsetname='DayOfMonth'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,doms)
     dsetname='Hour'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,hours)
     dsetname='Minute'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,minutes)
     dsetname='Second'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,seconds)
     dsetname='MilliSecond'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,milliseconds)
     dsetname='DayOfYear'
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,doys)

     !-----------------------------------------------------------------------
     allocate (Tb(nchan, npixel, nscan))
     groupname='S1'
     subgroupname=''
     dsetname='Tc'
     dim1=NCHAN_LOW;      dim2=npixel;        dim3=nscan
     allocate (Tb1(dim1,dim2,dim3))
     call  hdf5rd_f(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,Tb1)
     Tb(1:9, :,:) = Tb1(:,:,:)
     deallocate (Tb1)

     groupname='S2'
     dim1=NCHAN_HIGH;      dim2=npixel;        dim3=nscan
     allocate (Tb2(dim1,dim2,dim3))
     call  hdf5rd_f(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,Tb2)
     Tb(10:nchan, :,:) = Tb2(:,:,:)
     deallocate (Tb2)
     !-----------------------------------------------------------------------
     allocate (qc(nchan, npixel, nscan))
     groupname='S1'
     subgroupname=''
     dsetname='Quality'
     dim1=npixel;        dim2=nscan;           dim3=1
     allocate (qc1(dim1,dim2))
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,qc1)
     do ichan=1,9 
        qc(ichan,:,:) = qc1(:,:)
     enddo
     deallocate (qc1)

     groupname='S2'
     allocate (qc2(dim1,dim2))
     call  hdf5rd_i(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,qc2)
     do ichan=10,nchan
        qc(ichan,:,:) = qc2(:,:)
     enddo
     deallocate (qc2)
     Where(qc > 0 .AND. qc < 5)  qc = 0 !---sliu
     !-----------------------------------------------------------------------
     allocate (lats(npixel, nscan), lons(npixel, nscan) )
     groupname='S1'
     subgroupname=''
     dim1=npixel;        dim2=nscan;           dim3=1
     dsetname='Latitude'
     call  hdf5rd_f(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,lats)
     dsetname='Longitude'
     call  hdf5rd_f(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,lons)
     !-----------------------------------------------------------------------
     allocate (angles(npixel, nscan))
     groupname='S1'
     subgroupname=''
     dim1=npixel;        dim2=nscan;           dim3=1
     dsetname='incidenceAngle'
     call  hdf5rd_f(granule_name,groupname,subgroupname,dsetname,dim1,dim2,dim3,angles)

     !-------------------------------------------------------------------------------------
     ! 
     ! output section to write into MIRS internal TDR format
     !
     !-------------------------------------------------------------------------------------

     !---- generate output tdr file names ----
     call insert_path_string( rdrFilesGmi(ifile), TRIM(pathTDR), 'TDR_', tdrFileGmi  )

     !---- wirte header part ----
     call WriteRadHdrScanLMode(tdrFileGmi, iu_out, nscan, NFOV, NQC, NCHAN, freq, polar)

     !--- nodes ----
     allocate( nodes(nscan) )
     nodes(:) = 0
     do iscan = 1, nscan-1
        !---- use the center one ( 110 or 111 ) ----
        if( lats(110,iscan) .gt. lats(110,iscan+1) ) nodes(iscan) = 1
     enddo

     if( nodes(nscan-1) .eq. 1 ) nodes(nscan) = 1

     allocate( utcs( nscan ) )
     allocate( qcs( nqc, nscan ) )


     do iscan = 1, nscan
        !---- mirs internal seconds are saved in milli-seconds of the day
        utcs = ( hours(iscan) * 3600 + minutes(iscan) * 60 + seconds(iscan) ) * 1000 + milliseconds(iscan)

        !---- swich NCHAN,NFOV ====> NFOV,NCHAN order ----
        do ichan = 1, NCHAN
           do ifov  = 1, NFOV
              tbs(ifov,ichan) = tb(ichan,ifov,iscan)
           enddo
        enddo

        qcs(1:NQC,iscan) = qc(1:NQC,1,iscan)

        !---- write scan line data ----
        CALL WriteRadMeasScanLMode(iu_out,NQC,INT(qcs(1:NQC,iscan)),NCHAN,NFOV, &
             angles(1:NFOV,iscan),tbs(1:NFOV,1:NCHAN), &
             lats(1:NFOV,iscan),lons(1:NFOV,iscan),nodes(iscan), &
        utcs(iscan),int(doys(iscan)),int(years(iscan)),relAziAngles(:), &
             angles(1:NFOV,iscan) )

     enddo

     deallocate( years )
     deallocate( months )
     deallocate( doms )
     deallocate( hours )
     deallocate( minutes )
     deallocate( seconds )
     deallocate( milliseconds )
     deallocate( doys )
     deallocate( Tb )
     deallocate( qc )
     deallocate( qcs )
     deallocate( lats )
     deallocate( lons )
     deallocate( angles )
     deallocate( nodes )
     deallocate( utcs )

     CLOSE(iu_out)

  ENDDO fileLoop

  DEALLOCATE(rdrFilesGmi,tdrFilesGmi)
  CALL CloseLogFile()




  !+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Contains

  Function inquire_attr_sub(attr_buffer, attrsub_name) result(attrsub_ival)
    ! find the substring attrsub_name.
    !---------------------------------------------------------------------------
    IMPLICIT NONE
    CHARACTER (LEN =*),INTENT(IN) :: attr_buffer
    character (len=*), INTENT(IN) :: attrsub_name
    integer (KIND=8)              :: attrsub_ival
    character(len=100)            :: attrsub_cval
    integer                       :: str_id0, str_ida, str_id1, str_id2,&
         str_idx, str_idy

    str_id0 = index(attr_buffer, attrsub_name)
    if (str_id0 <= 0) then
       print *,  attrsub_name, ' is not found in '
       print *,  attr_buffer
       attrsub_ival = -999
       print *,  ' attrsub_ival = -999 '
       return
    endif
    str_ida = len(attr_buffer)
    str_id1 = index( attr_buffer(str_id0:str_ida),'=')
    str_id2 = index( attr_buffer(str_id0:str_ida),';')
    str_idx = str_id0 + str_id1
    str_idy = str_id0 + str_id2 - 2
    attrsub_cval = attr_buffer(str_idx:str_idy)
    read(attrsub_cval,'(I10)') attrsub_ival

  END function inquire_attr_sub

  subroutine hdf5rd_i(inputFileName,groupname,subgroupname,dsetname,dim1,&
       dim2,dim3,dataset)
    !
    ! read hdf5 integer dataset

    ! input:  inputFileName,groupname,dsetname,dim1,dim2,dim3
    ! output: dataset

    CHARACTER(LEN=256)                   :: inputFileName
    CHARACTER (LEN = 80),intent(in)      :: groupname, subgroupname, dsetname
    integer, intent(in)                  :: dim1,dim2,dim3
    integer, intent(out)                 :: dataset(dim1,dim2,dim3)
    INTEGER(HSIZE_T),DIMENSION(:), allocatable :: data_dims


    if (dim1 > 1 .and. dim2 > 1 .and. dim3 > 1) then
       allocate( data_dims(3) )
       data_dims(1)=dim1
       data_dims(2)=dim2
       data_dims(3)=dim3
    else if (dim1 > 1 .and. dim2 > 1 .and. dim3 ==1) then
       allocate( data_dims(2) )
       data_dims(1)=dim1
       data_dims(2)=dim2
    else if (dim1 > 1 .and. dim2 ==1 .and. dim3 ==1) then
       allocate( data_dims(1) )
       data_dims(1)=dim1
    else 
       write(6,*) 'dsetname',dsetname
       write(6,*) 'dim1, dim2, dim3 ',dim1,dim2,dim3
       write(6,*) 'dim1, dim2, dim3 must >=1. '
       write(6,*) 'stop at hdf5rd_i'
       return 
    endif

    ! Initialize FORTRAN interface.
    CALL h5open_f(error)
    ! Open file to read VD (table) information
    call h5fopen_f(trim(inputFileName), H5F_ACC_RDONLY_F, file_id, error)
    if(error /=0) call error_message(1, inputFileName)
    call h5gopen_f(file_id, trim(groupname), group_id, error)
    if(error /=0) call error_message(2, groupname)
    if( len(trim(subgroupname)) > 0 ) then
       call h5gopen_f(group_id, trim(subgroupname), subgroup_id, error)
       if(error /=0) call error_message(3, subgroupname)
       call h5dopen_f(subgroup_id, trim(dsetname), dset_id, error)
       if(error /=0) call error_message(4, dsetname) 
       call h5dget_type_f(dset_id, type_id, error)
       !call h5dread_f(dset_id, type_id, dataset, data_dims, error)
       call h5dread_f(dset_id, H5T_NATIVE_INTEGER, dataset, data_dims, error)
       print *, 'read: ', trim(groupname),'/',trim(subgroupname), '/', trim(dsetname)
       call h5dclose_f(dset_id, error)
       call h5gclose_f(subgroup_id, error)
    else
       call h5dopen_f(group_id, trim(dsetname), dset_id, error)
       if(error /=0) call error_message(4, dsetname) 
       call h5dget_type_f(dset_id, type_id, error)
       !call h5dread_f(dset_id, type_id, dataset, data_dims, error)
       call h5dread_f(dset_id, H5T_NATIVE_INTEGER, dataset, data_dims, error)
       print *, 'read: ', trim(groupname), '/', trim(dsetname)
       call h5dclose_f(dset_id, error)
    endif
    call h5gclose_f(group_id, error)
    call h5fclose_f(file_id, error)
    IF( error == -1) THEN
       PRINT *, 'FAILED TO Close: ', inputFileName
       STOP
    ELSE
    END IF
    call h5close_f(error)
    deallocate(data_dims)
  end subroutine hdf5rd_i


  subroutine hdf5rd_f(inputFileName,groupname,subgroupname,dsetname,dim1,dim2,dim3,dataset)
    !
    ! read hdf5 real type dataset

    ! input:  inputFileName,groupname,dsetname,dim1,dim2,dim3
    ! output: dataset

    CHARACTER (LEN = 256),intent(in)     :: inputFileName
    CHARACTER (LEN = 80),intent(in)      :: groupname, subgroupname, dsetname
    integer, intent(in)                  :: dim1,dim2,dim3
    real, intent(out)                    :: dataset(dim1,dim2,dim3)
    INTEGER(HSIZE_T),DIMENSION(:), allocatable :: data_dims


    if (dim1 > 1 .and. dim2 > 1 .and. dim3 > 1) then
       allocate( data_dims(3) )
       data_dims(1)=dim1
       data_dims(2)=dim2
       data_dims(3)=dim3
    else if (dim1 > 1 .and. dim2 > 1 .and. dim3 ==1) then
       allocate( data_dims(2) )
       data_dims(1)=dim1
       data_dims(2)=dim2
    else if (dim1 > 1 .and. dim2 ==1 .and. dim3 ==1) then
       allocate( data_dims(1) )
       data_dims(1)=dim1
    else 
       write(6,*) 'dsetname',dsetname
       write(6,*) 'dim1, dim2, dim3 ',dim1,dim2,dim3
       write(6,*) 'dim1, dim2, dim3 must >=1. '
       write(6,*) 'stop at hdf5rd_f'
       return 
    endif

    ! Initialize FORTRAN interface.
    CALL h5open_f(error)
    ! Open file to read VD (table) information
    call h5fopen_f(trim(inputFileName), H5F_ACC_RDONLY_F, file_id, error)
    if(error /=0) call error_message(1, inputFileName)
    call h5gopen_f(file_id, trim(groupname), group_id, error)
    if(error /=0) call error_message(2, groupname)
    if( len(trim(subgroupname)) > 0 ) then
       call h5gopen_f(group_id, trim(subgroupname), subgroup_id, error)
       if(error /=0) call error_message(3, subgroupname)
       call h5dopen_f(subgroup_id, trim(dsetname), dset_id, error)
       if(error /=0) call error_message(4, dsetname) 
       call h5dget_type_f(dset_id, type_id, error)
       !call h5dread_f(dset_id, type_id, dataset, data_dims, error)
       call h5dread_f(dset_id, H5T_NATIVE_REAL, dataset, data_dims, error)
       print *, 'read: ', trim(groupname),'/',trim(subgroupname), '/', trim(dsetname)
       call h5dclose_f(dset_id, error)
       call h5gclose_f(subgroup_id, error)
    else
       call h5dopen_f(group_id, trim(dsetname), dset_id, error)
       if(error /=0) call error_message(4, dsetname) 
       call h5dget_type_f(dset_id, type_id, error)
       !call h5dread_f(dset_id, type_id, dataset, data_dims, error)
       call h5dread_f(dset_id, H5T_NATIVE_REAL, dataset, data_dims, error)
       print *, 'read: ', trim(groupname),'/', trim(dsetname)
       call h5dclose_f(dset_id, error)
    endif
    call h5gclose_f(group_id, error)
    call h5fclose_f(file_id, error)
    IF( error == -1) THEN
       PRINT *, 'FAILED TO Close: ', inputFileName
       STOP
    END IF
    call h5close_f(error)
    deallocate(data_dims)
  end subroutine hdf5rd_f

  subroutine error_message(errnumber, varname)
    CHARACTER (LEN = 80),intent(in)     :: varname
    integer, intent(in)                 :: errnumber
    if (errnumber ==1) then
       print *, 'file is not found: ', trim(varname) 
    else if (errnumber ==2) then   
       print *, 'group is not found: ', trim(varname) 
    else if (errnumber ==3) then   
       print *, 'subgroup is not found: ', trim(varname) 
    else if (errnumber ==4) then   
       print *, 'dataset is not found: ', trim(varname) 
    endif
    stop
  end subroutine error_message


END PROGRAM rdr2tdr_gpm_gmi
