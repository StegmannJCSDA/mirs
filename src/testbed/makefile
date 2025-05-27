#$ID:$
SHELL = /bin/sh

SUBS =  biasGenerAndMonit \
	chopp \
	fm \
	ims \
	ffColloc \
	mergeEDR \
	mergeNEDTofDiffInstr \
	nwp \
	rdr2tdr \
	retrRegress \
	sweClimo \
	sfr \
	sfr2dep \
	tdr2sdr \
   vipp \
   grid \
	mirs2nc \
	bias_NeuralNetwork
        #convExternalFormats \


all:
	- PWD=`pwd`;\
	for i in ${SUBS}; do \
		echo '<<<<' $$i '>>>>'; \
		cd ${PWD}/$$i; \
		make $@; \
	done

clean:
	- PWD=`pwd`;\
	for i in ${SUBS}; do \
		echo '<<<<' $$i '>>>>'; \
		cd ${PWD}/$$i; \
		make $@; \
	done
