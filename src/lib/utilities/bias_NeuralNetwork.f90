!======================================================================================
! Name: bias_NeuralNetwork
! Type: F90 Module
! Descripption: Module that contains the necessary routines to calculate 
!               TB bias using Nerual Network method
! Moudles needed:
!     - Consts
!     - utils
! Subroutines and functions contained:
!     - TBbias_NN_cal
!     - layer_output
!     - relu
!     - read_NN_model
!     - save_NN_output
!     - TPW_NN
!     - Tskin_NN
! History:
!     2019 Yan Zhou CISESS-ESSIC @ University of Maryland
!
!======================================================================================
module bias_NeuralNetwork 
  USE Consts
  USE utils

  IMPLICIT NONE
  PRIVATE :: layer_output
  !----Publicly available subroutine(s)
  PUBLIC :: TBbias_NN_cal, TPW_NN, Tskin_NN
  PUBLIC :: read_NN_model
  PUBLIC :: ReLU
  !---INTRINSIC functions used in this module
  INTRINSIC :: MATMUL, COS


  REAL,PARAMETER                                                  :: minValidValue  = 0.0
!
!======================================================================================
!

CONTAINS

  subroutine TBbias_NN_cal(fmTB,nChan,angle,lat,iSfc,TBbias_NNModelFile,TPW_NNModelFile,Tskin_NNModelFile, &
                           TBbias_NN_out,TPW, Tskin, clw)
     IMPLICIT NONE

     CHARACTER(LEN=*)                   :: TBbias_NNModelFile,TPW_NNModelFile,Tskin_NNModelFile

     REAL,    DIMENSION(:), pointer        :: data_in
     REAL,    DIMENSION(:)                 :: fmTB
     REAL,    DIMENSION(:)                 :: TBbias_NN_out
     REAL                                  :: TPW, Tskin
     REAL                                  :: angle, lat, clw
     INTEGER                               :: nChan,iSfc
     INTEGER                               :: ichan,i
     INTEGER                               :: nLayers,nInput,nNode1,nNode2,nOutput

     real, DIMENSION(:), pointer        :: layer_1_bias, output_1
     real, DIMENSION(:), pointer        :: layer_2_bias, output_2
     real, DIMENSION(:), pointer        :: layer_3_bias, output_3

     real, DIMENSION(:,:), pointer      :: layer_1_weights
     real, DIMENSION(:,:), pointer      :: layer_2_weights
     real, DIMENSION(:,:), pointer      :: layer_3_weights
  
     real, DIMENSION(:), pointer        :: train_mean,train_stdv


     call read_NN_model(TBbias_NNModelFile,nLayers,nInput,nNode1,nNode2,nOutput, &
                        layer_1_bias,layer_2_bias,layer_3_bias,&
                        layer_1_weights,layer_2_weights,layer_3_weights,train_mean,train_stdv)      ! TBbias_NNModelFile: nLayers,nInput,nNode1,nNode2,nOutput = 3,27,200,200,22



     ALLOCATE(output_1(nNode1),output_2(nNode2),output_3(nOutput))
     ALLOCATE(data_in(nInput))

     IF (iSfc .eq. OC_TYP) THEN
        CALL TPW_NN(fmTB,nChan,angle,lat,iSfc,TPW_NNModelFile,TPW)
        IF (TPW > DEFAULT_VALUE_REAL .and. TPW < minValidValue) TPW = minValidValue

        CALL Tskin_NN(fmTB,nChan,angle,lat,iSfc,Tskin_NNModelFile,Tskin)
        IF (Tskin > DEFAULT_VALUE_REAL .and. Tskin < minValidValue) Tskin = minValidValue

        CALL regr_clw_atms(fmTB,angle,lat,clw);  
        IF (clw > DEFAULT_VALUE_REAL .and. clw < minValidValue) clw = minValidValue

       IF ((nChan+5) .NE. nInput) THEN
          write(*,*) "STOPPPED!"
          write(*,*) "NN valid for ATMS only! No. of input channel need to be 22!"
          write(*,*) "Input No. of channel is: ", nchan
          STOP
       ENDIF

       IF (TPW>DEFAULT_VALUE_REAL .and. Tskin>DEFAULT_VALUE_REAL .and. clw>DEFAULT_VALUE_REAL) THEN
        data_in(1)=lat
        data_in(2)=cos(angle)
        data_in(3:24) = fmTB(1:nChan)
        data_in(25)=clw
        data_in(26)=TPW    
        data_in(27)=Tskin  

        do i = 1,nInput
           data_in(i) = (data_in(i) - train_mean(i))/train_stdv(i)
        end do

        ! Input to 1st hidden layer
        CALL layer_output(data_in,layer_1_weights,layer_1_bias, output_1)
        output_1=ReLU(output_1)

        ! 1st to 2nd hidden layer
        CALL layer_output(output_1,layer_2_weights,layer_2_bias, output_2)
        output_2=ReLU(output_2)

        ! 2nd hidden layer to output
        CALL layer_output(output_2,layer_3_weights,layer_3_bias, output_3)
       ELSE
         output_3=DEFAULT_VALUE_REAL
       END IF

     ELSE
          output_3=DEFAULT_VALUE_REAL
     END IF ! Choose surface type

     DO ichan=1,nChan
        IF (output_3(ichan) .ne.  DEFAULT_VALUE_REAL) THEN
          TBbias_NN_out(ichan)=output_3(ichan)
        ELSE
          TBbias_NN_out(ichan) = minValidValue
        END IF
     END DO


     ! Release memory allocated in current subroutine
     DEALLOCATE(output_1,output_2,output_3)
     DEALLOCATE(data_in)
     ! Release memory allocated in read_NN_model 
     DEALLOCATE(layer_1_bias,layer_2_bias,layer_3_bias,layer_1_weights,layer_2_weights,layer_3_weights)
     DEALLOCATE(train_mean, train_stdv)

  end subroutine TBbias_NN_cal


  subroutine layer_output(input, weight, bias, output)
     IMPLICIT NONE
     REAL,    DIMENSION(:), pointer             :: input, bias, output
     REAL,    DIMENSION(:,:), pointer           :: weight

     output=matmul(weight,input) + bias

  end subroutine layer_output

  pure function relu(x) result(res)
    !! REctified Linear Unit (RELU) activation function.
    real, intent(in) :: x(:)
    real:: res(size(x))
    res = max(0., x)
  end function relu

  subroutine read_NN_model(NNModelFile,nLayers,nInput,nNode1,nNode2,nOutput,layer_1_bias,layer_2_bias,layer_3_bias,&
                           layer_1_weights,layer_2_weights,layer_3_weights,train_mean, train_stdv)
     USE HDF5 ! This module contains all necessary modules
     IMPLICIT NONE
     CHARACTER(LEN=*)                   :: NNModelFile

     INTEGER(HID_T) :: file_id       ! File identifier
     INTEGER(HID_T) :: dset_id       ! Dataset identifier
     INTEGER(HID_T) :: dspace_id     ! Dataspace identifier 

     INTEGER     ::   error ! Error flag
     INTEGER     ::   i,j
     INTEGER     ::   nLayers,nInput,nNode1,nNode2,nOutput

     real, DIMENSION(:), pointer        :: layer_1_bias
     real, DIMENSION(:), pointer        :: layer_2_bias
     real, DIMENSION(:), pointer        :: layer_3_bias

     real, DIMENSION(:,:), pointer      :: layer_1_weights
     real, DIMENSION(:,:), pointer      :: layer_2_weights
     real, DIMENSION(:,:), pointer      :: layer_3_weights

     real, DIMENSION(:), pointer        :: train_mean, train_stdv 

     INTEGER(HSIZE_T), DIMENSION(1) :: data_dims1, maxdims1
     INTEGER(HSIZE_T), DIMENSION(2) :: data_dims2, maxdims2


     nLayers=3  ! TBC

     !
     ! Initialize FORTRAN interface.
     !
     CALL h5open_f(error)

     !
     ! Open an existing file.
     !
     CALL h5fopen_f (NNModelFile, H5F_ACC_RDWR_F, file_id, error)


     !!! layer_1_bias
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'layer_1_bias', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims1, maxdims1, error)
     !CALL H5Sget_simple_extent_ndims_f(dspace_id, rank, error)
   
     !
     ! Read the dataset.
     !
     nNode1=data_dims1(1)
     ALLOCATE(layer_1_bias(data_dims1(1)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, layer_1_bias, data_dims1, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! layer_2_bias
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'layer_2_bias', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims1, maxdims1, error)

     !
     ! Read the dataset.
     !
     nNode2=data_dims1(1)
     ALLOCATE(layer_2_bias(data_dims1(1)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, layer_2_bias, data_dims1, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! layer_3_bias
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'layer_3_bias', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims1, maxdims1, error)

     !
     ! Read the dataset.
     !
     nOutput=data_dims1(1)
     ALLOCATE(layer_3_bias(data_dims1(1)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, layer_3_bias, data_dims1, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! layer_1_weights
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'layer_1_weights', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims2, maxdims2, error)

     !
     ! Read the dataset.
     !
     nInput=data_dims2(2)
     ALLOCATE(layer_1_weights(data_dims2(1),data_dims2(2)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, layer_1_weights, data_dims2, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! layer_2_weights
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'layer_2_weights', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims2, maxdims2, error)

     !
     ! Read the dataset.
     !
     ALLOCATE(layer_2_weights(data_dims2(1),data_dims2(2)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, layer_2_weights, data_dims2, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! layer_3_weights
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'layer_3_weights', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims2, maxdims2, error)

     !
     ! Read the dataset.
     !
     ALLOCATE(layer_3_weights(data_dims2(1),data_dims2(2)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, layer_3_weights, data_dims2, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! train_mean 
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'train_mean', dset_id, error)

     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims1, maxdims1, error)

     !
     ! Read the dataset.
     !
     ALLOCATE(train_mean(data_dims1(1)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, train_mean, data_dims1, error)

     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)


     !!! train_stdv
     ! Open an existing dataset.
     !
     CALL h5dopen_f(file_id, 'train_stdv', dset_id, error)
     
     ! Get dataspace dims
     CALL H5Dget_space_f(dset_id,dspace_id, error)    ! dataspace identifier 
     CALL h5sget_simple_extent_dims_f(dspace_id, data_dims1, maxdims1, error)

     !
     ! Read the dataset.
     !
     ALLOCATE(train_stdv(data_dims1(1)))
     CALL h5dread_f(dset_id,H5T_NATIVE_REAL, train_stdv, data_dims1, error)
     
     !
     ! Close the dataset.
     !
     CALL h5dclose_f(dset_id, error)

     !
     ! Close the file.
     !
     CALL h5fclose_f(file_id, error)

     !
     ! Close FORTRAN interface.
     !
     CALL h5close_f(error)

     RETURN 
  end subroutine read_NN_model


  subroutine TPW_NN(fmTB,nChan,angle,lat,iSfc,TPW_NNModelFile,TPW)
     IMPLICIT NONE

     CHARACTER(LEN=*)                      :: TPW_NNModelFile

     REAL,    DIMENSION(:)                 :: fmTB
     REAL,    DIMENSION(:), pointer        :: data_in
     REAL                                  :: TPW
     REAL                                  :: angle, lat
     INTEGER                               :: nChan,iSfc
     INTEGER                               :: ichan,i
     INTEGER                               :: nLayers,nInput,nNode1,nNode2,nOutput

     real, DIMENSION(:), pointer        :: layer_1_bias, output_1
     real, DIMENSION(:), pointer        :: layer_2_bias, output_2
     real, DIMENSION(:), pointer        :: layer_3_bias, output_3

     real, DIMENSION(:,:), pointer      :: layer_1_weights
     real, DIMENSION(:,:), pointer      :: layer_2_weights
     real, DIMENSION(:,:), pointer      :: layer_3_weights

     real, DIMENSION(:), pointer        :: train_mean,train_stdv

     call read_NN_model(TPW_NNModelFile,nLayers,nInput,nNode1,nNode2,nOutput, &
                        layer_1_bias,layer_2_bias,layer_3_bias,&
                        layer_1_weights,layer_2_weights,layer_3_weights,train_mean,train_stdv)

     ALLOCATE(output_1(nNode1),output_2(nNode2),output_3(nOutput))
     ALLOCATE(data_in(nInput))

     IF ((nChan+2) .NE. nInput) THEN
        write(*,*) "STOPPPED!"
        write(*,*) "TPW_NN valid for ATMS only! No. of input channel need to be 22!"
        write(*,*) "Input No. of channel is: ", nchan
        STOP
     ENDIF

     IF (iSfc .eq. OC_TYP) THEN
        data_in(1)=lat
        data_in(2)=cos(angle)
        data_in(3:nInput) = fmTB(1:nChan)

        do i = 1,nInput
           data_in(i) = (data_in(i) - train_mean(i))/train_stdv(i)
        end do

        ! Input to 1st hidden layer
        CALL layer_output(data_in,layer_1_weights,layer_1_bias, output_1)
        output_1=ReLU(output_1)

        ! 1st to 2nd hidden layer
        CALL layer_output(output_1,layer_2_weights,layer_2_bias, output_2)
        output_2=ReLU(output_2)

        ! 2nd hidden layer to output
        CALL layer_output(output_2,layer_3_weights,layer_3_bias, output_3)
       
        do i = 1,nOutput 
           IF(output_3(i).lt. minValidValue) output_3(i)=minValidValue
        end do
     ELSE
        output_3 = DEFAULT_VALUE_REAL 
     END IF ! Choose surface type

     TPW=output_3(1)

     ! Release memory allocated in current subroutine
     DEALLOCATE(output_1,output_2,output_3)
     DEALLOCATE(data_in)
     ! Release memory allocated in read_NN_model 
     DEALLOCATE(layer_1_bias,layer_2_bias,layer_3_bias,layer_1_weights,layer_2_weights,layer_3_weights)
     DEALLOCATE(train_mean, train_stdv)
    
  end subroutine TPW_NN


  subroutine Tskin_NN(fmTB,nChan,angle,lat,iSfc,Tskin_NNModelFile,Tskin)
     IMPLICIT NONE

     CHARACTER(LEN=*)                      :: Tskin_NNModelFile

     REAL,    DIMENSION(:)                 :: fmTB
     REAL,    DIMENSION(:), pointer        :: data_in
     REAL                                  :: Tskin 
     REAL                                  :: angle, lat
     INTEGER                               :: nChan,iSfc
     INTEGER                               :: ichan,i
     INTEGER                               :: nLayers,nInput,nNode1,nNode2,nOutput

     real, DIMENSION(:), pointer        :: layer_1_bias, output_1
     real, DIMENSION(:), pointer        :: layer_2_bias, output_2
     real, DIMENSION(:), pointer        :: layer_3_bias, output_3

     real, DIMENSION(:,:), pointer      :: layer_1_weights
     real, DIMENSION(:,:), pointer      :: layer_2_weights
     real, DIMENSION(:,:), pointer      :: layer_3_weights

     real, DIMENSION(:), pointer        :: train_mean,train_stdv

     call read_NN_model(Tskin_NNModelFile,nLayers,nInput,nNode1,nNode2,nOutput, &
                        layer_1_bias,layer_2_bias,layer_3_bias,&
                        layer_1_weights,layer_2_weights,layer_3_weights,train_mean,train_stdv)

     ALLOCATE(output_1(nNode1),output_2(nNode2),output_3(nOutput))
     ALLOCATE(data_in(nInput))

     IF (nInput .NE. 6) THEN
        write(*,*) "STOPPPED!"
        write(*,*) "Tskin_NN valid for ATMS only! No. of input channel need to be 4!"
        write(*,*) "Input No. of channel is: ", nchan
        STOP
     ENDIF

     IF (iSfc .eq. OC_TYP) THEN
        data_in(1)=lat
        data_in(2)=cos(angle)
        data_in(3:nInput) = fmTB(1:4)

        do i = 1,nInput
           data_in(i) = (data_in(i) - train_mean(i))/train_stdv(i)
        end do

        ! Input to 1st hidden layer
        CALL layer_output(data_in,layer_1_weights,layer_1_bias, output_1)
        output_1=ReLU(output_1)

        ! 1st to 2nd hidden layer
        CALL layer_output(output_1,layer_2_weights,layer_2_bias, output_2)
        output_2=ReLU(output_2)

        ! 2nd hidden layer to output
        CALL layer_output(output_2,layer_3_weights,layer_3_bias, output_3)

        do i = 1,nOutput
           IF(output_3(i).lt. minValidValue) output_3(i)=minValidValue
        end do
     ELSE
        output_3 = DEFAULT_VALUE_REAL 
     END IF ! Choose surface type

     Tskin=output_3(1)

     ! Release memory allocated in current subroutine
     DEALLOCATE(output_1,output_2,output_3)
     DEALLOCATE(data_in)
     ! Release memory allocated in read_NN_model 
     DEALLOCATE(layer_1_bias,layer_2_bias,layer_3_bias,layer_1_weights,layer_2_weights,layer_3_weights)
     DEALLOCATE(train_mean, train_stdv)

  end subroutine Tskin_NN

end module bias_NeuralNetwork 
