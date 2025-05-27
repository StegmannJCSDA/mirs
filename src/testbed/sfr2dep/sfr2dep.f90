!===============================================================================
! Name:       sfr2dep.f90
!
!
! Type:         Main Program
!
!
! Description:
!       Read SFR files and transforms SFRs into DEPs.
!
! Modules needed:
!       - Consts
!       - IO_DEP
!       - IO_SFR
!       - IO_Misc
!       - ErrorHandling
!
!
! History:
!       10-22-2014      Dr. Tanvir Islam, 
!                       NOAA/NESDIS/STAR & CIRA/CSU    Original Coder
!
!===============================================================================

Program sfr2dep

  USE Consts
  USE IO_DEP
  USE IO_SFR
  USE IO_Misc
  USE ErrorHandling
  USE QCchecking

  IMPLICIT NONE

  !---Different parameters
  INTEGER, PARAMETER :: LEN_FILE=250

  !---Pointers
  CHARACTER(LEN=LEN_FILE), DIMENSION(:), POINTER  :: depFiles1,sfrFiles1,depFiles2,sfrFiles2
  !---Namelist data 
  CHARACTER(LEN=LEN_FILE) :: depFilesList=DEFAULT_VALUE_STR4
  CHARACTER(LEN=LEN_FILE) :: sfrFilesList=DEFAULT_VALUE_STR4
  CHARACTER(LEN=LEN_FILE) :: depDir=DEFAULT_VALUE_STR4
  CHARACTER(LEN=LEN_FILE) :: sfrDir=DEFAULT_VALUE_STR4
  CHARACTER(LEN=LEN_FILE) :: LogFile=DEFAULT_VALUE_STR4
  INTEGER                 :: norbits2process=DEFAULT_VALUE_INT
  INTEGER                 :: nprofs2process=DEFAULT_VALUE_INT

  !---Single variables
  INTEGER            :: iu_listdep,iu_listsfr,nFiles,ifile,nprf,iuDEP1,iuDEP2,ierr,iprof,iuSFR

  !---- Structures ----
  TYPE(DEP_type)     :: Dep
  TYPE(SFR_type2)    :: Sfr

  !---- SFR variables ----
  INTEGER                                          :: iscanLine,iposline
  REAL,               DIMENSION(:),    ALLOCATABLE :: blat,blon,jday,sec,sfrr,iscan,ipos, &
                                                      sfrprob
  INTEGER,DIMENSION(:),ALLOCATABLE                :: sfrqc0,sfrqc1

  NAMELIST /ContrlSfr2dep/ depFilesList,sfrFilesList,depDir,&
                           LogFile,norbits2process,nprofs2process

  !-----------------------------------------------------
  !     Read control-data from namelist
  !-----------------------------------------------------
  READ(*,NML=ContrlSfr2dep)

  !---Prepare Log file
  CALL OpenLogFile(Logfile)

  !---Read the file names of dep and sfr data and build output files names
  CALL ReadList(iu_listdep,trim(depFilesList),depFiles1,nFiles,depFiles2,depDir,'DEP_')
  CALL ReadList(iu_listsfr,trim(sfrFilesList),sfrFiles1,nFiles,sfrFiles2,sfrDir,'FMSDR_')
  IF (nfiles .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'') 
  nfiles=minval((/norbits2process,nfiles/))

  !-----------------------------------------------------
  !     Loop over the files
  !-----------------------------------------------------
  FilesLoop: DO ifile=1,nFiles
     write(*,*) 'ifile=',ifile
     write(*,'(A)') 'SFR='//TRIM( sfrFiles1(ifile) )
     write(*,'(A)') 'DEP1='//TRIM( depFiles1(ifile) )
     write(*,'(A)') 'DEP2='//TRIM( depFiles2(ifile) )
     write(*,*)

     !---Read header of DEP1 file
     CALL ReadHdrDEP(iuDEP1,depFiles1(ifile),DEP,nprf)

     !---Read header of SFR file
     CALL ReadHdrSFR(iuSFR,sfrFiles1(ifile))

     !---Read SFR content in scan/pos mode and stores into profiles mode
     ALLOCATE (blat(DEP%nScanLines*DEP%nPosScan),blon(DEP%nScanLines*DEP%nPosScan),&
          jday(DEP%nScanLines*DEP%nPosScan),sec(DEP%nScanLines*DEP%nPosScan),sfrr(DEP%nScanLines*DEP%nPosScan),&
          iscan(DEP%nScanLines*DEP%nPosScan),ipos(DEP%nScanLines*DEP%nPosScan), &
          sfrProb(DEP%nScanLines*DEP%nPosScan),sfrQC0(DEP%nScanLines*DEP%nPosScan),sfrQC1(DEP%nScanLines*DEP%nPosScan) )

     iprof = 1
     ScanLinesLoop: DO iscanLine=1,DEP%nScanLines
        PosLinesLoop: DO iposLine=1,DEP%nPosScan
           CALL ReadSFR2(iuSFR,SFR,ierr)
           blat(iprof)     = SFR%blat
           blon(iprof)     = SFR%blon
           jday(iprof)     = SFR%jday
           sec(iprof)      = SFR%sec
           sfrr(iprof)     = SFR%sfrr
           sfrprob(iprof)  = SFR%prob
           sfrqc0(iprof)   = SFR%flag0
           sfrqc1(iprof)   = SFR%flag1
           iscan(iprof)    = iscanLine
           ipos(iprof)     = iposLine
           iprof = iprof+1
        ENDDO PosLinesLoop
     ENDDO ScanLinesLoop

     !---Output header of DEP2 (derived products)
     CALL WriteHdrDEP(iuDEP2,depFiles2(ifile),DEP,nprf)

     !---Loop over the profiles within the DEP file
     ProfLoop: DO iprof=1,nPrf
        CALL ReadDEP(iuDEP1,DEP,ierr)
        IF (DEP%iscanLine .eq. iscan(iprof) .and. DEP%iscanPos .eq. ipos(iprof)) THEN
           DEP%SFR = sfrr(iprof)
           IF (DEP%algSn >= 4150) then
              DEP%Prob_SF  = sfrprob(iprof)
              CALL setQCSfrDep(DEP, sfrqc0(iprof),sfrqc1(iprof))
           ENDIf
           !write(*,*) iscan(iprof), Dep%iscanLine, blat(iprof), DEP%lat
        ELSE
           write(*,*) "Error in sfr2dep_atms, sfr and dep files differ in records"
           CALL ErrHandl(ErrorType,Err_NoMatching,'') 
        ENDIF
        !---Output DEP
        CALL WriteDEP(iuDEP2,DEP)
     ENDDO ProfLoop

     !---Release memory
     DEALLOCATE(blat,blon,jday,sec,sfrr,iscan,ipos)
     IF (ALLOCATED(sfrProb)) DEALLOCATE(sfrProb)
     IF (ALLOCATED(sfrQC0))  DEALLOCATE(sfrQC0)
     IF (ALLOCATED(sfrQC1))  DEALLOCATE(sfrQC1)

     !---Close the DEP file
     CLOSE (iuDEP1)
     CLOSE (iuDEP2)

     !---Close the DEP file
     CLOSE (iuSFR)

  ENDDO FilesLoop

  DEALLOCATE(depFiles1)
  DEALLOCATE(depFiles2)
  DEALLOCATE(sfrFiles1)
  DEALLOCATE(sfrFiles2)

  CALL CloseLogFile()

End Program sfr2dep
