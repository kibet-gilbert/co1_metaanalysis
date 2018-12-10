#!/bin/bash
# this script will run the awk script on all its arguments

AWK_EXEC=$( which gawk )
AWK_SCRIPT=~/bioinformatics/github/co1_metaanalysis/Code/buildfasta_jb.awk

if [ $# -eq 0 ]
then
	echo "Usage: $0 file1.tsv [file2.tsv file3.tsv ...]"
	exit 1
fi

for input_filename in "$@"
do
	echo "Let us proceed with file ${input_filename}..."
	output_filename=${input_filename%.*}.fasta
	${AWK_EXEC} -f ${AWK_SCRIPT} ${input_filename} > ${output_filename}
done
