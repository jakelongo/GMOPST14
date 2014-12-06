#!/bin/bash

DATASETURL=http://data.bris.ac.uk/datasets/11ja3p9r3iofn14v65lq51mzvo/GMOPST14

function downloadDevice {
	wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/scope.sav
	wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/setup.txt
  downloadData $1 aes aes
  downloadData $1 NullAes aes
  downloadData $1 beg beg
  downloadData $1 end end
}

function downloadData {
	for i in {0..299}; do
		wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/DATA/$2/d000/$3`printf _%04d $i`.dwfm
	done
	wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/DATA/$2_cip.txt
  wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/DATA/$2_mes.txt
  wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/DATA/$2_traceList.txt
  wget -nH -x --cut-dirs=2 -P datasets $DATASETURL/$1/DATA/$2_key.txt
}

downloadDevice ARM
downloadDevice SASEBO_R
downloadDevice SASEBO_GII

