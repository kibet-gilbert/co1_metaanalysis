#!/bin/bash
# this script performs may functions including: run awk script to generate .fasta format files, delete repeated seqs, trim seqs ...

AWK_EXEC=$( which gawk )
PYTHON_EXEC=$( which python )
RSCRIPT_EXEC=$( which Rscript )
co1_path=~/bioinformatics/github/co1_metaanalysis/
AWK_SCRIPT=${co1_path}code/buildfasta_jb.awk
xml_to_tsv=${co1_path}code/xml_to_tsv.py
data_cleanup=${co1_path}code/data_cleanup.R
rgbifdata=${co1_path}code/gbifdata_retrival.R
country_codes=${co1_path}code/country_codes-ISO-3166-1
rgbifdata_eval=${co1_path}code/gbifdata_eval.R
inputdata_path=${co1_path}data/input/
gbif_dest=${inputdata_path}gbif

usage() { #checks if the positional arguments (input files) for execution of the script are defined
	set -E
	trap '[ "$?" -ne 10 ] || return 10' ERR
        if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[1]} file1.*[file2.* file3.* ...]"
                return 10
        fi
}

realpath() { #
	${PYTHON_EXEC} -c "import os,sys; print(os.path.realpath(sys.argv[1]))" $1
}


rename() { #Takes input file name and extracts the prefix of the name ("prefix"), The suffix (".suffix"), Absolute path to the file and the name of the Directory within which the file is found.
        input_filename=`basename -- ${i}`
        output_filename=${input_filename%.*}
	filename_ext=${input_filename##*.}
	src_dir_path=`dirname $(realpath ${i})`
	src_dir=${src_dir_path##*/}
	#echo -e "input is $i \ninput_filename is $input_filename \noutput_filename is $output_filename \nfilename_ext is $filename_ext \nsrc_dir_path is $src_dir_path \nsrc_dir is $src_dir"
}

#===============================================================================================================================================================
# Functions to support bolddata_retrival for BOLD systems v5:

# Functionn to extract data from JSON response
extract_data() {
	local json_response="$1"
	taxa=$(echo "$json_response" | jq -r '.successful_terms[0].matched' | cut -d':' -f2)
	taxaName=$(echo "$json_response" | jq -r '.successful_terms[0].matched' | cut -d':' -f3)
	geo_loc=$(echo "$json_response" | jq -r '.successful_terms[1].matched' | cut -d':' -f3)
	echo -e "\ttaxa:${taxa};taxaName:${taxaName};geo_loc:${geo_loc}"
}

# ********************************************************************
bolddata_retrival() { #
	# This fuction retrives data belonging to a list of country names given.
	### input can be a file containing names of select countries or idividual country names

	if [[ ( $# -eq 0 ) || ! ( `echo $1` =~ -.*$ ) ]]
	then
		echo "Input error..."
		echo "function usage: ${FUNCNAME[0]} [-i <Taxonomy>] [-a] [-c <name of country>] [-f <a file with list of countries and named *countries*>]"
		return 1
	fi

	unset taxon_nam
	regexp='^[a-zA-Z0-9/_\-\ ]+$'
	local OPTIND=1
	unset countries
	unset country_geo
	unset shortDate

	while getopts 't:ac:f:' key
	do
		case "${key}" in
			t)
				if [[ ( ! -f $OPTARG ) && ( "$OPTARG" =~ $regexp ) ]]
				then
					local taxon_nam=${OPTARG}
					#echo $taxon_nam
				else		
					until [[ "$taxon_nam" =~ $regexp ]]
					do
						read -p "Please enter taxon name to be searched, ensure the spelling is right otherwise you will download everything from the BOLD database. To ensure that you are downloading the right dataset/taxon first go to 'http://v4.boldsystems.org/index.php/Public_BINSearch?searchtype=records' and search the tax(on|a) of choice as explained:: " taxon_nam
					done
				fi
				;;
			f)
				if [ ! -f $OPTARG ]
				then
					echo "input error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename $OPTARG` =~ ^.*countries.*$ ) ]]
				then
					#countries+=("$(while IFS="\n" read -r line || [[ "$line" ]]; do geography+=("`echo $line | sed 's/ /%20/g'`"); done < $OPTARG)")
					while IFS= read -r line || [[ -n "$line" ]]
					do countries+=("`echo $line | sed 's/ /%20/g'`")
						country_geo+=("`echo -e "'$line'"`")
					done < $OPTARG
					local countryNO=${#countries[@]}
				else
					echo "input file error in `basename $OPTARG`: input file should be named '.*countries.*'"
				fi
				;;
			c)
				countries+=(`echo $OPTARG | sed 's/ /%20/g'`)
				local countryNO=${#countries[@]}
				;;
			a)
				countries=("all")
				local countryNO="all"
				;;
			?)
				echo "Input error..."
				echo "function usage: ${FUNCNAME[0]} [-i <Taxonomy>] [-a] [-c <name of country>] [-f <a file with list of countries>]"
				return 1
				;;
		esac
	done

	echo -e "\n\tDownloading $taxon_nam data of countries named in ${countryNO} countr[y|ies]: ${country_geo[@]} from www.boldsystems.org V5"
	
	local taxon_name=`echo $taxon_nam | sed 's/ /%20/g'`
	local shortDate=$(date +"%Y%m%d")
	
	local wgetoutput_dir=${inputdata_path}bold_data/${shortDate}_${taxon_name}
	until [[ -d ${wgetoutput_dir} ]]
	do
		echo "Creating output directory '${wgetoutput_dir}'"
		mkdir ${wgetoutput_dir}
	done
	
	IFS=$'\n'
	if [[ ! ( `echo ${countries[0]}` =~ "all" ) ]]
	then
		for i in ${countries[@]}
		do
			local country=`echo ${i} | sed "s/%20/_/g; s/'//"g`
			echo -e "\t${country}"

			# Run search query through a preprocessor first:
			query_url="https://portal.boldsystems.org/api/query/preprocessor?query=tax:${taxon_name};geo:${i}"
			json_response=$(wget -qO- "$query_url")
			# Extract data from JSON response:
			extract_data "$json_response"
			
			# Second URL to download data:
			download_summary_url="https://portal.boldsystems.org/api/summary?query=tax:${taxa}:${taxaName};geo:country/ocean:${geo_loc}&fields=specimens"
			download_queryID_url="https://portal.boldsystems.org/api/query?query=tax:${taxa}:${taxaName};geo:country/ocean:${geo_loc}&extent=full"
			dataQuery_response=$(wget -qO- "$download_queryID_url")
	 		query_id=$(echo "$dataQuery_response" | jq -r '.query_id')		
			echo -e "\tquery_id:${query_id}"
			download_data_url="https://portal.boldsystems.org/api/documents/${query_id}/download?format=tsv"
			download_dataDWC_url="https://portal.boldsystems.org/api/documents/${query_id}/download?format=dwc"

			# Download data from URLs:download_summary_url
			wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxon_name}"_"${country}`date +"-%d%m%Y"`"_summary.json -a ${wgetoutput_dir}/${taxon_nam}_wget_log ${download_summary_url}
			# Download data from URLs:download_data_url
			wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxon_name}"_"${country}`date +"-%d%m%Y"`"_data.tsv -a ${wgetoutput_dir}/${taxon_nam}_wget_log ${download_data_url}
			# Download data from URLs:download_dataDWC_url
			#wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxon_name}"_"${country}`date +"-%d%m%Y"`"_dataDWC.tsv -a ${wgetoutput_dir}/${taxon_nam}_wget_log ${download_dataDWC_url}

		done
	elif [[ ( `echo ${countries[0]}` =~ "all" ) ]]
	then
		# Run search query through a preprocessor first:
		query_url="https://portal.boldsystems.org/api/query/preprocessor?query=tax:${taxon_name}"
		json_response=$(wget -qO- "$query_url")
		# Extract data from JSON response:
		extract_data "$json_response"
		
		# Second URL to download data:
		download_summary_url="https://portal.boldsystems.org/api/summary?query=tax:${taxa}:${taxaName}&fields=specimens"
		download_queryID_url="https://portal.boldsystems.org/api/query?query=tax:${taxa}:${taxaName}&extent=full"
		dataQuery_response=$(wget -qO- "$download_queryID_url")
	 	query_id=$(echo "$dataQuery_response" | jq -r '.query_id')
		echo -e "\tquery_id:${query_id}"
		download_data_url="https://portal.boldsystems.org/api/documents/${query_id}/download?format=tsv"
		download_dataDWC_url="https://portal.boldsystems.org/api/documents/${query_id}/download?format=dwc"
	
		# Download data from URLs:download_summary_url
		wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxaName}"_all"`date +"-%d%m%Y"`"_summary.json -a ${wgetoutput_dir}/${taxon_nam}_wget_log ${download_summary_url}
		# Download data from URLs:download_data_url
		wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxaName}"_all"`date +"-%d%m%Y"`"_data.tsv -a ${wgetoutput_dir}/${taxon_nam}_wget_log ${download_data_url}
		# Download data from URLs:download_dataDWC_url
		#wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxaName}"_all"`date +"-%d%m%Y"`"_dataDWC.tsv -a ${wgetoutput_dir}/${taxon_nam}_wget_log ${download_dataDWC_url}
	fi
}

#===============================================================================================================================================================
boldxml2tsv() { #This function generates .tsv files from .xml files using python script and Beautifulsoup4 and pandas package

	usage $@
	
	TAB=$(printf '\t')
	
	echo -e "\n\tGenerating .tsv files from .xml downloads..."

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo -e "\n\t!!!input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(xml)$ ) && ( `basename -- "$i"` =~ .*_summary.xml$ ) ]]
		then
			continue
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(xml)$ ) && ( `basename -- "$i"` != .*_summary.xml$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename 
			echo -e "\n\t'${input_filename}'...STARTing: `date +"-%H.%M.%S-%Y%m%d"`"
			sed 's/class/Class/g' "$i" | sed "s/$TAB/,/g" > ${input_src}/input.xml
			${PYTHON_EXEC} ${xml_to_tsv} ${input_src}/input.xml
			if [ -f output.tsv ]; then
				mv output.tsv ${input_src}/${output_filename}.tsv
				rm ${input_src}/input.xml
				if [ $? -eq 0 ]; then
					echo -e "\b...DONE. The output file has been stored in ${input_src}/${output_filename}.tsv"
				fi
			else
				echo "output.tsv does not exist. The input might be empty."
			fi
		else
			echo -e "\n\tinput file error in `basename -- $i`: input file should be a .xml file format"
			continue
		fi
	done
}

#===============================================================================================================================================================
#bolddata_tsv2fasta

boldtsv2fasta() { #This function generates .fasta files from cleaned-up .tsv files using an awk script

	usage $@

	echo "Generating FASTA files from BOLD BCDM TSV files"

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename
			echo -e "\n\tProcessing '${input_filename}'..."
			${AWK_EXEC} -f ${AWK_SCRIPT} "$i" > ${input_src}/${output_filename}.fasta
		else
			echo "input file error in `basename -- $i`: input file should be a .tsv file format"
			continue
		fi
	done
}

#	**************************************************************

boldtsv_cleanup() { # This function takes an 80 column .TSV output of boldxml2tsv function above and add 
	usage $@

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv)$ ) ]]
		then
			rename
			input_src=`dirname "$( realpath "${i}" )"`
			echo -e "\nProcessing file '${input_filename}'..."
			${RSCRIPT_EXEC} --vanilla ${data_cleanup} $i |& tee -a ${input_src}/${output_filename}_cleanup

			if [ $? -eq 0 ]
                        then
                                echo -e "\n\tDONE. Output file stored in ${input_src}/${output_filename}_[all_data|Over499_data|500to700_data|650to660_data|Over700_data|Under500_data].tsv"
			fi
			#Generating a file of Genbank and BOLD ids.
			${AWK_EXEC} 'BEGIN{FS="\t";OFS="\t"}; FNR>=2{if ($71 !~ /^NA$/) print $1,$71}' ${input_src}/${output_filename}_all_data.tsv ${input_src}/${output_filename}_NonCOI_data.tsv >> ${input_src}/${output_filename}_all_data.genbank_ids
			if [ $? -eq 0 ]
			then
				echo -e "\n\tGenBank accession numbers have been stored in ${input_src}/${output_filename}_all_data.genbank_ids"
                        fi

		else
			echo "input file error in `basename -- $i`: input file should be a .tsv file format"
			continue
		fi
	done
}

#===============================================================================================================================================================

append_tsvfile() { # this function tests if the .tsv file has content and if it does it appends it to a cummulative file of all input records. This function is applied in the function below: clean_sort_tsv()

	if [ `grep -v "processid" ${input} | wc -l` -ge 1 ]
	then
		echo -e "Transfering contents in '${input}'"
		awk ' FNR == 1 { if ( $0 ~ /^.*processid/) {next}} {print $0}' ${input} >> ${output}
		rm ${input}
	else
		echo -e "\n `basename -- $input` has no content besides the header!!!"
		rm ${input}
	fi
}

boldtsv_assemble() { #aggregate together records from separate files into one file.
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.tsv file2.tsv [file3.tsv ...]"
		return 1
	fi

	echo "cleaning up and sorting .tsv files "

	i=${1}
	rename
	inputdata_path=${src_dir_path}
	output_file=${src_dir}
	output_files=("${inputdata_path}/${output_file}_500to700_data.tsv" "${inputdata_path}/${output_file}_650to660_data.tsv" "${inputdata_path}/${output_file}_all_data.tsv" "${inputdata_path}/${output_file}_Over499_data.tsv" "${inputdata_path}/${output_file}_Over700_data.tsv" "${inputdata_path}/${output_file}_Under500_data.tsv" "${inputdata_path}/${output_file}_NonCOI_data.tsv")

	for i in "$@"
	do
                if [ ! -f $i ]
		then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*[0-9]{8}\.(tsv)$ ) ]]
                then
                        rename
                        echo -e "\nLet us proceed with file '${input_filename}'..."
                        ${RSCRIPT_EXEC} --vanilla ${data_cleanup} $i |& tee -a ${input_src}/${output_file}_cleanup

			if [ $? -eq 0 ]
                        then
                                echo -e "\nCreating temporary output files as ${output_filename}_[all_data|Over499_data|500to700_data|650to660_data|Over700_data|Under500_data].tsv"
			fi
			
			#Generating a file of Genbank and BOLD ids.
			${AWK_EXEC} 'BEGIN{FS="\t";OFS="\t"}; FNR>=2{if ($71 !~ /^NA$/) print $1,$71}' ${input_src}/${output_filename}_all_data.tsv ${input_src}/${output_filename}_NonCOI_data.tsv >> ${input_src}/${output_file}_all_data.genbank_ids
			if [ $? -eq 0 ]
			then
				echo -e "GenBank accession numbers have been stored in ${input_src}/${output_filename}_all_data.genbank_ids"
                        fi


			for i in ${output_files[@]}
			do
				if [[ ( ! -f $i ) || ( `grep -A 100 "processid" ${i} | wc -l` -eq 0 ) ]]
				then
					grep "processid" $1 > $i && echo -e "\nInput file $i is set"
				else
					continue
				fi
			done
			input=${output_filename}_500to700_data.tsv
			output=${output_files[0]}
			append_tsvfile
			
			input=${output_filename}_650to660_data.tsv
			output=${output_files[1]}
			append_tsvfile

			input=${output_filename}_all_data.tsv 
			output=${output_files[2]}
			append_tsvfile
			
			input=${output_filename}_Over499_data.tsv
			output=${output_files[3]}
			append_tsvfile

			input=${output_filename}_Over700_data.tsv
			output=${output_files[4]}
			append_tsvfile

			input=${output_filename}_Under500_data.tsv
			output=${output_files[5]}
			append_tsvfile

			input=${output_filename}_NonCOI_data.tsv
			output=${output_files[6]}
			append_tsvfile
		 else
                        echo "input file error in `basename -- $i`: input file should be a [0-9]{8}.tsv file format"
                        continue
                fi
        done
}

clean_sort_tsv() { #This function cleans the .tsv files, sort the records into differnt files based on the sequence length and finally appends this files to a cummulative files of diffent input files

        usage $@

        echo "cleaningup and sorting .tsv files "

	output_files_africa=("${inputdata_path}clean_africa/afroCOI_500to700_data.tsv" "${inputdata_path}clean_africa/afroCOI_650to660_data.tsv" "${inputdata_path}clean_africa/afroCOI_all_data.tsv" "${inputdata_path}clean_africa/afroCOI_Over499_data.tsv" "${inputdata_path}clean_africa/afroCOI_Over700_data.tsv" "${inputdata_path}clean_africa/afroCOI_Under500_data.tsv")

	output_files_eafrica=("${inputdata_path}clean_eafrica/eafroCOI_500to700_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_650to660_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_all_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_Over499_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_Over700_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_Under500_data.tsv")


        for i in "$@"
        do
                if [ ! -f $i ]
		then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv)$ ) ]]
                then
                        rename
			input_src=`dirname "$( realpath "${i}" )"`
			outfilename=`echo "${input_src}/${output_filename}"`
                        echo -e "\nLet us proceed with file '${input_filename}'..."
                        ${RSCRIPT_EXEC} --vanilla ${data_cleanup} $i

			case $output_filename in
				Algeria|Madagascar|Angola|Malawi|Benin|Mali|Botswana|Mauritania|Burkina_Faso|Mauritius|Morocco|Cameroon|Mozambique|Cape_Verde|Namibia|Central_African_Republic|Nigeria|Chad|Niger|Comoros|Republic_of_the_Congo|Cote_d_Ivoire|Reunion|Democratic_republic_of_the_Congo|Djibouti|Sao_Tome_and_Principe|Egypt|Senegal|Equatorial_Guinea|Seychelles|Eritrea|Sierra_Leone|Somalia|Gabon|South_Africa|Gambia|Ghana|Sudan|Guinea-Bissau|Swaziland|Guinea|Togo|Tunisia|Lesotho|Liberia|Zambia|Libya|Zimbabwe)
					for i in ${output_files_africa[@]}
					do
						grep "processid" $1 > $i && echo -e "\nInput file $i is set"
					done

					input=${outfilename}_500to700_data.tsv
					output=${output_files_africa[0]}
					append_tsvfile

					input=${outfilename}_650to660_data.tsv
					output=${output_files_africa[1]}
					append_tsvfile

					input=${outfilename}_all_data.tsv 
					output=${output_files_africa[2]}
					append_tsvfile

					input=${outfilename}_Over499_data.tsv
					output=${output_files_africa[3]}
					append_tsvfile
					
					input=${outfilename}_Over700_data.tsv
					output=${output_files_africa[4]}
					append_tsvfile
					
					input=${outfilename}_Under500_data.tsv
					output=${output_files_africa[5]}
					append_tsvfile
					;;
				Kenya|Tanzania|Uganda|Rwanda|Burundi|South_Sudan|Ethiopia)
					for i in ${output_files_eafrica[@]}
					do
						grep "processid" $1 > $i && echo -e "\nInput file $i is set"
					done

					input=${outfilename}_500to700_data.tsv
					output=${output_files_eafrica[0]}
					append_tsvfile
					
					input=${outfilename}_650to660_data.tsv
					output=${output_files_eafrica[1]}
					append_tsvfile
					
					input=${outfilename}_all_data.tsv
					output=${output_files_eafrica[2]}
					append_tsvfile
					
					input=${outfilename}_Over499_data.tsv
					output=${output_files_eafrica[3]}
					append_tsvfile
					
					input=${outfilename}_Over700_data.tsv
					output=${output_files_eafrica[4]}
					append_tsvfile
					
					input=${outfilename}_Under500_data.tsv
					output=${output_files_eafrica[5]}
					append_tsvfile
					
					;;
				*)
					echo -e "The file $output_filename \b.tsv is not in the list of African countries or is not in the right format."
					;;
			esac
		 else
                        echo "input file error in `basename -- $i`: input file should be a .tsv file format"
                        continue
                fi
        done
}

#===============================================================================================================================================================
#still under development
gbifdata_retrival(){ # This function retrieves occurence data from gbif.org (Global Biodiversity Information Facility). It is based on first building a json file which it uses to interact with the gbif API and downloads the records.

	if [[ ( $# -eq 0 ) && ( $@ =~ ^[A_Z][a-z-]+$ ) ]]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} Scientific_name1[Scientific_name2 Scientific_name3...]"
                return 1
        fi

	echo -e "\n\tProceeding with gbif data downloading..."
	echo -e "\n\tPlease ensure that the search terms refer to the desired full scientific names. \n\tIf so proceed and key in your valid GBIF account details below::"

	unset gbif_user
	unset gbif_email
	unset gbif_pwd
	regexp='^[a-zA-Z0-9/_-\ \|,\.]+$'
	regexmail='^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$'
	regexpwd='^[a-zA-Z0-9#$+*@%^=]+$'
	regextaxon='^kingdom|phylum|class|order|family|genus$'
	until [[ "$gbif_user" =~ $regexp ]]
	do
		read -p "Please enter your GBIF username: " gbif_user
	done

	until [[ "$gbif_email" =~ $regexmail ]]
	do
		read -p "Please enter your GBIF email: " gbif_email
	done

	until [[ "$gbif_pwd" =~ $regexpwd ]]
	do
		read -p "Please enter your GBIF password: " gbif_pwd
	done
	echo "$gbif_pwd"

	for i in "$@"
	do
		unset taxon
		echo -e "\tFor ${i} please enter its taxonomic class (${regextaxon})"
		until [[ "$taxon" =~ $regextaxon ]]
		do
			read -p "Please enter the taxonomic classification of ${i}: " taxon
		done

		${RSCRIPT_EXEC} --vanilla ${rgbifdata} ${i} ${taxon} ${gbif_user} ${gbif_email} ${gbif_pwd} |& tee -a ${gbif_dest}/${i}_download.log && mv ${gbif_dest}/${i}_download.log ${gbif_dest}/${i}*/${i}_download.log

		if [ $? -eq 0 ]
		then
			echo -e "\n\tDONE. The dowloads have stored in ${gbif_dest}/${i}"
		fi

	done
}


#===============================================================================================================================================================

gbifdata_eval(){ #This function evaluates the gbif data downloads and outputs among others genbank accession numbers.
	if [[ ( $# -eq 0 ) && ( $@ =~ ^[A_Z][a-z-]+$ ) ]]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} Scientific_name1[Scientific_name2 Scientific_name3...]"
		return 1
	fi

	for i in "$@"
	do
		input_src=`dirname "$( realpath "${gbif_dest}/${i}*/*.txt" )"`

		if [ -d ${input_src} ]
		then
			${RSCRIPT_EXEC} --vanilla ${rgbifdata_eval} ${i} |& tee -a ${input_src}/${i}_evaluation.log
			if [ $? -eq 0 ]
			then
				echo -e "\n\tDONE. The dowloads have been successfully evaluated and output stored in ${input_src}/*"
			fi
		else
			echo -e "\n\tERROR!!! Input directory ${input_src} does not exist"
		fi

        done

	echo -e "\n\tWe will Proceed and generate FASTA format headers from the downloaded records with associated sequences"
	$AWK_EXEC 'BEGIN{FS="\t"; OFS="\t"}{
		if (FNR==NR) {a[FNR]=$3;b[FNR]=$1}
		else{
			for (x=1; x<=length(a); ++x);{
			if (a[x] == $10) {$10=b[x]}
			};
			print $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14}' ${country_codes} ${input_src}/${i}_gbifheaders.tsv > ${input_src}/${i}_gbifheaders_edited.tsv && mv ${input_src}/${i}_gbifheaders_edited.tsv ${input_src}/${i}_gbifheaders.tsv
	$AWK_EXEC 'BEGIN{FS="\t"; OFS="|"} NR == 1 {
		next} {gsub(/\|/, ";")} {
		if ($3 == "NA") {genbank=""} 
		else {genbank="("$3")"}} {
	       	print ">"$2 genbank,$4,$5,$6,"fam-"$7,"subfam-NA","tri-NA","gs-"$8,"sp-"$9,"country-"$10,"exactsite-"$11, "lat_"$12, "lon_"$13, "elev-"$14}' ${input_src}/${i}_gbifheaders.tsv > ${input_src}/${i}.headers

}

#===============================================================================================================================================================
genbankdata_retrival(){ # This function will retrieve nucleotide sequence data from GenBank based on a provided file of accession numbers.
	# epost -db nuccore -input ../gbif/psychodidae-9164/psychodidae_genbankAccessionNumbers.txt | efetch -format gb -mode xml > output.genbank

	if [[ ( ! "$*" =~ ^-i[[:space:]][[:alnum:]\._]*[[:space:]]-d[[:space:]][[:lower:]]*$ ) || ( $# -lt 4 ) ]]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} -i <uniq_ids_file> -d <target_database>"
		return 1
	fi

	unset ids
	unset db

        local OPTIND=1
	while getopts 'i:d:' key
	do
		case "${key}" in
			i)
				if [ ! -f $OPTARG ]
				then
					echo -e "\tinput error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename -- $OPTARG` =~ .*(\.ids|genbankAccessionNumbers\.txt)$ ) ]]
				then
					ids=$OPTARG
				fi
				;;
			d)
				if [[ ! $OPTARG =~ ^(nuccore|popset|pubmed|taxonomy|gene|genome|nucest|books|protein|snp|sra|probe|nucgss)$ ]]
				then
					echo -e "\tinput error: the value $OPTARG is not an integer"
				elif [[ $OPTARG =~ ^(nuccore|popset|pubmed|taxonomy|gene|genome|nucest|books|protein|snp|sra|probe|nucgss)$ ]]
				then
					db=$OPTARG
				fi
				;;
			?)
				echo "Please check, Input error..."
				echo "Usage: ${FUNCNAME[0]} -i <uniq_ids_file> -d <target_database>"
				return 1
				;;
		esac
	done

	for i in "$ids"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*(\.ids|genbankAccessionNumbers\.txt)$ ) ]]
		then
			rename
			input_src=`dirname "$( realpath "${i}" )"`
			genbankouput_dir=${inputdata_path}genbank/${src_dir}
			until [[ -d ${genbankouput_dir} ]]
			do
				echo "Creating output directory '${genbankouput_dir}'"
				mkdir ${genbankouput_dir}
			done
			unset options

			echo -e "\tWhich type of the following data formats and modes do you wish to download?"
			PS3='Select a choice from the following options, Enter [1] or [2] or [3] or [4] or [5]: '
			options=("FASTA format in plain text mode" "FASTA format in XML mode" "GenBank (.gb) format in plaitext mode" "GenBank (.gb) format in XML mode" "Quit")
			select opt in "${options[@]}"
                        do
				unset format
				unset mode
				case $opt in
					"FASTA format in plain text mode")
						format="fasta"
						mode="text"
						break
						;;
					"FASTA format in XML mode")
						format="fasta"
						mode="xml"
						break
						;;
					"GenBank (.gb) format in plaitext mode")
						format="gb"
						mode="text"
						break
						;;
					"GenBank (.gb) format in XML mode")
						format="gb"
						mode="xml"
						break
						;;
					"Quit")
                                                break
                                                ;;
                                        *) echo "INVALID choice $REPLY"
                                                ;;
				esac
			done

			epost -db $db -input ${i} | efetch -format ${format} -mode ${mode} > ${genbankouput_dir}/${output_filename}.${format}.${mode}

			if [ $? -eq 0 ]
			then
				echo -e "\n\tDONE. The output file has been stored in ${genbankouput_dir}/${output_filename}.${format}.${mode}"
			fi
		else
			echo -e "\n\tinput file error in `basename -- $i`: input file should be a .txt format with name '[myfilename]genbankAccessionNumbers.txt'"
			continue
		fi
	done
}

#===============================================================================================================================================================
gbtsv2fasta(){ # This is part of the function gbxml2tsv; takes its output and convert it to a FASTA file
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.gb.xml[file2.gb.xml file3.gb.xml ...]"
		return 1
	fi

	for i in "$@"
	do
		echo -e "\n\tProceeding with converting `basename -- $i` to fasta..."
		rename
		input_src=`dirname "$( realpath "${i}" )"`

		$AWK_EXEC 'BEGIN{FS="\t"; OFS="|"} NR == 1 {next} {
		gsub(/\|/, ";")} {
		gsub(/[[:space:]]*/,"",$7); split($7,a,";"); {
			if (a[4] ~ /^unclassified.*$/) {b = "NA"}
			else if (a[4] ~ /^[[:alnum:]]+$/) {b = a[4]}
			else {b = "NA"}
			if (a[6] ~ /^unclassified.*$/) {c = "NA"}
                        else if (a[6] ~ /^[[:alnum:]]+$/) {c = a[6]}
                        else {c = "NA"}
			if (a[10] ~ /^unclassified.*$/) {d = "NA"}
                        else if (a[10] ~ /^[[:alnum:]]+$/) {d = a[10]}
                        else {d = "NA"}
			if (a[13] ~ /^unclassified.*$/) {e = "NA"}
                        else if (a[13] ~ /^[[:alnum:]]+$/) {e = a[13]}
                        else {e = "NA"}
			if (a[14] ~ /^unclassified.*$/) {k = "NA"}
			else if (a[14] ~ /^[[:alnum:][:space:]]+$/) {f = a[14]}
			else {f = "NA"}
			if (a[15] ~ /^unclassified.*$/) {k = "NA"} 
			else if (a[15] ~ /^[[:alnum:][:space:]]+$/) {k = a[15]}
			else {k = "NA"}
			}
		} {
		if ($9 ~ /^[[:alpha:] '\'']*:.*/) {split($9,l,":"); m = l[1]; n = l[2]; gsub(/^[[:space:]]*/,"",n) }
		else if ($9 ~ /^[[:alpha:]]*$/) {m = $9; n = "NA"}
		else {m = "NA"; n = "NA"}
		} {
		if ($10 ~ /^-?[[:digit:]]{,3}\.[[:digit:]]{,12}/) {p = $10}
	       	else {p = "NA"}
		} {
		if ($11 ~ /^-?[[:digit:]]{0,3}\.[[:digit:]]{0,12}/) {q = $11}
	       	else {q = "NA"}
		}; {
		print ">"$3"("$1")",b,c,d,"fam-"e,"subfam-NA","tri-NA","gs-"f,"sp-"$6,"subsp-NA","country-"m,"exactsite-"n, "lat_"p, "lon_"q, "elev-NA", "l-"$4 "\n" $8 }' ${i} > ${input_src}/${output_filename}.fasta
		sed -i 's/country-|/country-NA|/g' ${input_src}/${output_filename}.fasta
		if [ $? -eq 0 ]
		then
			echo -e "\tDONE. The output file has been stored in `basename -- ${input_src}/${output_filename}.fasta`"
		fi
	done

}


gbxml2tsv(){ # This function will convert a genbank XML file downloaded in gb formatr and convert it to a tab separated fortmat tsv.
	#element=$(xtract -input psychodidae_genbankAccessionNumbers.gb.xml -outline | awk '!seen[$0]++' | sed 's/GBSet//g' | xargs)
	#echo ${element} | tr -s '[:blank:]' '\t' > test.gbifAssociatedSequences.gb.tsv && xtract -input test.gbifAssociatedSequences.gb.xml -pattern GBSeq -element ${element} >> test.gbifAssociatedSequences.gb.tsv

	set -E
	trap ' return 1 ' ERR

 	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.gb.xml[file2.gb.xml file3.gb.xml ...]"
		return 1
	fi

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.gb\.xml$ ) ]]
                then
                        rename
                        input_src=`dirname "$( realpath "${i}" )"`
			output=${input_src}/${output_filename}
			unset element

			echo -e "\n\tProceeding with converting `basename -- $i` to .tsv and FASTA..."
			#element=$(xtract -input ${i} -outline | awk '!seen[$0]++' | sed 's/GBSet//g; s/\bGBSeq\b//g; s/\bGBSeq_other-seqids\b//g; s/\b/GBSeq_feature-table\b//g;' | xargs)
			element="GBSeq_locus GBSeq_accession-version BOLDProcessid GBSeq_length GBSeq_definition GBSeq_organism GBSeq_taxonomy GBSeq_sequence country lat lon collection_date collected_by"
			element1=`echo ${element} | tr -s '[:blank:]' '\t'`
			echo ${element1} > ${output}_raw.tsv
			xtract -input ${i} -pattern GBSeq -def "NA" -element GBSeq_locus GBSeq_accession-version GBSeq_length GBSeq_definition GBSeq_organism GBSeq_taxonomy \
			       	-upper GBSeq_sequence \
				-block GBSeq_other-seqids -sep "::" -element GBSeqid \
				-block GBQualifier -if GBQualifier_name -equals country -def "NA" -element GBQualifier_value \
				-block GBQualifier -if GBQualifier_name -equals lat_lon -def "NA" -element GBQualifier_value \
				-block GBQualifier -if GBQualifier_name -equals collection_date -def "NA" -element GBQualifier_value \
				-block GBQualifier -if GBQualifier_name -equals collected_by -def "NA" -element GBQualifier_value >> ${output}_raw.tsv
			
			echo -e "\tThe file extracted from '`basename -- ${output}.xml`' has been stored as '`basename -- ${output}_raw.tsv`' and has the following columns:: \n${element}.\n\tWe will proceed to reformat the columns..."

			$AWK_EXEC -v element="$element1" 'BEGINFILE {gsub(/[[:space:]]/, "\t", element); print element};
			BEGIN{ FS="\t"; OFS="\t" }; 
			FNR == 1 { next }; {
			if ($8 ~ /^.*gnl.*$/) {split($8,a,"::"); {
				for (i in a) if (a[i] ~ /^gnl.*$/) {b = a[i]}
				}; split(b,c,"|"); d = c[3]; split(d,e,"."); { 
				if (d !~ /^[[:upper:]]{4,5}[[:digit:]]{3,5}\-[[:digit:]]{2}.*$/) {y = "NA"} 
				else y = e[1]} 
				} 
			else if ($8 ~ /^[[:upper:]]{4,5}[[:digit:]]{3,5}\-[[:digit:]]{2}$/) {y = $8} 
			else {y = "NA"} 
			}; {
			split($10,f," ");
			if (f[2] ~ /S/) {v = "-"f[1]} 
			else if (f[2] ~ /N/) {v = f[1]} 
			else if ($10 ~ /^[-+][[:digit:]]{1,3}\.[[:digit:]]{0,12}$/) {v = $10} 
			else {v = "NA"} 
			}; {
			split($10,g," ");
			if (g[4] ~ /W/) {w = "-"g[3]} 
			else if (g[4] ~ /E/) {w = g[3]} 
			else if ($11 ~ /^[-+][[:digit:]]{1,3}\.[[:digit:]]{0,12}$/) {w = $11} 
			else {w = "NA"} 
			} {
			print $1,$2,y,$3,$4,$5,$6,$7,$9,v,w,$11,$12
			}' ${output}_raw.tsv > ${output}.tsv

			echo ${element} | tr -s '[:blank:]' '\t' > ${output}_COI.tsv && grep -E 'COI|cytochrome c oxidase subunit 1|CO1|mitochondrion, complete genome|cytochrome oxidase subunit 1|COX1' ${output}.tsv >> ${output}_COI.tsv

			grep -E -v 'COI|cytochrome c oxidase subunit 1|CO1|mitochondrion, complete genome|cytochrome oxidase subunit 1|COX1' ${input_src}/${output_filename}.tsv > ${output}_non-COI.tsv
			if [ $? -eq 0 ]
			then
				echo -e "\tDONE. The output file has been stored in ${input_src}/ as ${output_filename}[.tsv|_COI.tsv|_non-COI.tsv]"
			fi

			gbtsv2fasta ${output}.tsv

			gbtsv2fasta ${output}_COI.tsv

			gbtsv2fasta ${output}_non-COI.tsv

		else
			echo -e "\n\tinput file error in `basename -- $i`: input file should be a .gb.xml format with name '[myfilename].gb.xml'"
			continue
		fi
	done
}

#===============================================================================================================================================================
fasta2nexus(){ # This function will take a FASTA format sequence file and convert it to a nexus for use in PopART and Bayesian Phylogenetic analysis.
	if [[ ( ! "$*" =~ ^-s[[:space:]][[:alnum:]\._]*[[:space:]]-c[[:space:]][[:digit:]]*.*$ ) || ( $# -lt 2 ) ]]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} -s <reference_aln> [-c <INTEGER>] [-t <phylogenetic_tree>]"
		return 1
	fi

	unset TREES
	unset REF_MSA
	unset NCLUSTER

	local OPTIND=1
	while getopts 's:c:t:' key
	do
		case "${key}" in
			s)
				if [ ! -f $OPTARG ]
				then
					echo -e "\tinput error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename -- $OPTARG` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
				then
					REF_MSA=$OPTARG
				fi
				;;
			c)
				if [[ ! $OPTARG =~ ^[1-9][0-9]*$ ]]
				then
					echo -e "\tinput error: the value $OPTARG is not an integer"
				elif [[ $OPTARG =~ ^[1-9][0-9]*$ ]]
				then
					NCLUSTER=$OPTARG
				fi
				;;
			t)
				if [ ! -f $OPTARG ]
				then
					echo -e "\tinput error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename -- $OPTARG` =~ .*\.(tree|tre|newick|.*) ) ]]
				then
					TREES=$OPTARG
				fi
				;;
			?)
				echo "Input error..."
				echo "Usage: ${FUNCNAME[0]} -s <reference_aln> [-c <INTEGER>] [-t <phylogenetic_tree>]"
				return 1
				;;
		esac
	done

	echo -e "\tGenerating a NEXUS format file from the FASTA format file `basename -- $REF_MSA`"
	i=${REF_MSA}
	concatenate_fasta_seqs ${i}
	rename #${REF_MSA}
	outfile=`echo "${src_dir_path}/${output_filename}.nexus"`
	outfile1=`echo "${src_dir_path}/${output_filename}_shrt.${filename_ext}"`

	$AWK_EXEC -v MSA="$REF_MSA" -v outfile=$outfile -v outfile1=$outfile1 -v nclust=$NCLUSTER -v input_filename=$input_filename -v output_filename=$output_filename -v input_src=$input_src 'BEGIN{FS="[>|]"};
	BEGINFILE {printf "#NEXUS\n[This Nexus file has been generated from a fasta file called "input_filename".]\n\nBEGIN TAXA;" > outfile;} /^>/{ hdr=$0;
	split($2,id,"("); Id[FNR]=id[1]; split($10,sp,"sp-"); SP[FNR]=sp[2];
	split($12,a,"-"); country[FNR]=a[2]; split($14,b,"_"); lat[FNR]=b[2];
	split($15,c,"_"); lon[FNR]=c[2]; split($16,d,"-"); alt[FNR]=d[2];
	KD[i]=$3; ntax=length(Id); next} {
	seq[FNR-1]=$0; nchar=length(seq[1]) } {
	for(i=1; i<=FNR; i+=2) if(length(seq[i]) != nchar) {
		print "The length of the first sequence does not match the length of the sequence" Id[i] "sequences must be aligned first" > "/dev/stdout"
		exit 1 }}; 
	ENDFILE{ total=ntax*2; 
	{print "\n\tDIMENSIONS NTAX="ntax";\n\tTAXLABELS" >> outfile } {
	for(i=1; i<=total; i+=2) printf Id[i]"\n" >> outfile}; {
		printf ";\nEND;\n\nBEGIN CHARACTERS;\n\tDIMENSIONS NCHAR="nchar";\n\tFORMAT DATATYPE=DNA MISSING=? GAP=- ;\n\tMATRIX\n" >> outfile } {
	for(i=1; i<=total; i+=2) print Id[i],"\t"seq[i] >> outfile } {
		printf";\nEND;\n\nBEGIN GeoTags;\n\tDimensions NClusts="nclust";\n\tFormat labels=yes separator=Spaces;\n\tMatrix\n" >> outfile } {
	for(i=1; i<=total; i+=2) if ( lat[i] ~ "NA" ) {
		print "["Id[i]," "lat[i]," "lon[i]"]" >> outfile;} else{
		print Id[i]," "lat[i]," "lon[i] >> outfile } }; {
	print";\nEnd;\n" >> outfile } {
	for(i=1; i<=total; i+=2) if (KD[i] ~ /^Outgroup.*$/) {SP[i]=KD[i]} { 
		print Id[i]__SP[i]"\n"seq[i] >> outfile1 }} }' $REF_MSA
	
	if [ $? -eq 0 ]
	then
		echo -e "\tDONE. The newick file has been stored in ${input_src}/ as ${output_filename}.nexus"
	else
		echo -e "\tERROR ENCOUNTERED!!!"
	fi
}

#===============================================================================================================================================================
subset_seqs(){ #This function takes a faster sequence file and split it into multiple files simply based on nucleotide sequence length
	
	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
		return 1
	fi

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename

			#Removing gaps
			remove_gaps $i
			i=${input_src}/${output_filename}_dgpd.${filename_ext}
			regexp1='^n|y|N|Y|No|Yes|NO|YES|no|yes$'
			unset choice
			#Introducing a field "l-xxx" that has the length of the sequence in the header
			echo -e "\tWould you like to introduce a field 'l-xxx* in the header that denotes the length of the sequence?'"
			until [[ "$choice" =~ $regexp1 ]]
			do
				read -p "Please enter [Yes] or [No] to proceed: " choice
			done
			case $choice in
				YES|Yes|yes|Y|y)
					echo -e "Adding a seq. length field, ';l-xxx*' in the header to denote the sequence length..."
					$AWK_EXEC '/^>/{
					hdr=$0; next} {
					seq=$0 } match(seq,/^.*$/) { 
					LEN=RLENGTH }{
					print hdr"|l-"LEN; print seq }' $i > ${input_src}/${output_filename}_dgpd_len.${filename_ext}
					mv ${input_src}/${output_filename}_dgpd_len.fasta ${input_src}/${output_filename}_dgpd.${filename_ext}
					echo -e "\tDONE adding seq. length to the header."
					;;
				No|NO|no|N|n)
					echo -e "\tSequence length field will not be added to the header..."
					;;
				*)
					echo "ERROR!!! Invalid selection"
					;;
			esac

			unset NAME
			unset CUTOFF1
			unset cutoff
			unset options

			i=${input_src}/${output_filename}_dgpd.${filename_ext}
			echo -e "\tProceeding with subsetting the file...\n\tTo Subset the file into: Under500, Over700, 650to600 and 500to700_data-650to660 OR Just extracting datasets with over specific number of sequences: then select one of the options below::"
			options[0]="Subset the records into: Under500, Over700, 650to600 and 500to700_data-650to660"
			options[1]="Extract the records with Over or under a specific number of nucleotides"
			options[2]="Exit"
			PS3='Select option [1|2] to proceed, or [3] to exit: '
			select option in "${options[@]}"
			do
				regexp='^[><=]{1,2}[0-9]+$'
				case $option in
					${options[0]})
						echo -e "\n\tProceeding with `basename -- ${i}`...\nThe file will be split into four files based on nucleotide sequence length: len under 500; len over 700; len of 650to660; len of 500to700 - 650to660."
						$AWK_EXEC -v output_filename=$output_filename -v filename_ext=$filename_ext -v input_src=$input_src 'BEGIN{FS="|";}; /^>/{
						hdr=$0; next} {
						seq=$0 } match(seq,/^.*$/) { len=RLENGTH }; {
						if(len<500){
							print hdr"\n" seq > input_src"/"output_filename"_Under500."filename_ext
						}
						else if (len>700){
							print hdr"\n" seq > input_src"/"output_filename"_Over700."filename_ext
						}
						else if (len>=650 && len<=660){
							print hdr"\n" seq > input_src"/"output_filename"_650to660."filename_ext
						}
						else {
							print hdr"\n" seq > input_src"/"output_filename"_500to700_data-650to660."filename_ext
						} }' $i

						rm ${input_src}/${output_filename}_dgpd.${filename_ext}
						if [ $? -eq 0 ]
						then
							echo -e "\n\tCongratulations. OPERATION DONE!\n\tRecords with less than 500 nucleotides have been saved in '${input_src}/${output_filename}_Under500.${filename_ext}'\n\tRecords with over 700 nucleotides have been saved in '${input_src}/${output_filename}_Over700.${filename_ext}'\n\tRecords with 650 to 660 nucleotides have been saved in '${input_src}/${output_filename}_650to660.${filename_ext}'\n\tAnd records with 500 to 700 nucleotides excluding those with 650 to 660 nucleotides have been saved in '${input_src}/${output_filename}_500to700_data-650to660.${filename_ext}'"
						fi
						break
						;;
					${options[1]})
						until [[ "$cutoff" =~ $regexp ]]
						do
							read -p "Please enter the cutoff value (enter >|<|=INT ):: " cutoff
						done
						if [[ $cutoff =~ ^\>.*$ ]]
						then
							NAME="Over"
						elif [[ "$cutoff" =~ ^\<.*$ ]]
						then
							NAME="Under"
						fi
						CUTOFF1=`echo $cutoff | sed 's/<//g; s/>//g; s/=//g'`
						CUTOFF2=`echo $cutoff | sed 's/[0-9]*//g'`
						
						$AWK_EXEC -v output_filename=$output_filename -v filename_ext=$filename_ext -v input_src=$input_src -v cutoff=$cutoff -v NAME=$NAME -v CUTOFF1=$CUTOFF1 -v CUTOFF2=$CUTOFF2 'BEGIN{FS="|";}; /^>/{
						hdr=$0; next} {
						seq=$0 } match(seq,/^.*$/) { len=RLENGTH }; {
						if ( CUTOFF2 ~ ">=" ) { 
							if ( len >= CUTOFF1 ) {
								print hdr"\n" seq > input_src"/"output_filename"_"NAME CUTOFF1-1"."filename_ext
							}
						} 
						else if ( CUTOFF2 ~ "<=" ) {
							if ( len <= CUTOFF1 ) {
								print hdr"\n" seq > input_src"/"output_filename"_"NAME CUTOFF1+1"."filename_ext
							}
						}
						else if ( CUTOFF2 ~ ">" ) {
							if ( len > CUTOFF1 ) {
								print hdr"\n" seq > input_src"/"output_filename"_"NAME CUTOFF1"."filename_ext
							}
						}
						else if ( CUTOFF2 ~ "<" ) {
							if ( len < CUTOFF1 ) {
								print hdr"\n" seq > input_src"/"output_filename"_"NAME CUTOFF1"."filename_ext
							}
						}
						else if ( CUTOFF2 ~ "=" ) {
						}
						else {print "Conditional Operator " CUTOFF2 " is wrong"}
						}' ${i}

						rm ${input_src}/${output_filename}_dgpd.${filename_ext}
						if [ $? -eq 0 ]
						then
							echo -e "Congratulations. OPERATION DONE!\n\tRecords with $cutoff have been stored in '${input_src}/${output_filename}_${NAME1}${CUTOFF1}|+/-1.${filename_ext}'"
						fi
						break
						;;
					${options[2]})
						echo -e "\n\tExiting rooting of $TREE..."
						break
						;;
					*)
						echo -e "\n\tINVALID choice $REPLY"
						;;
				esac
			done
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
			continue
		fi
	done
}


#======================================================================================
#genereting headers from csv files with inputs...

#One of the input files came as excel .xml spreadsheet, which was easily converted to a text.csv file by the excel-compatible GUI LibreOffice Calc in ubuntu. To build fasta-headers from the text.csv file the following code was used:
#awk 'BEGIN { FS=","; OFS="|" } ; { for(i=1; i<=NF; i++) if($i ~ /^ *$/) $i = "NA" }; { print  ">" $1, $4, "gs-"$5, "sp-"$6, "subsp-"$7, "country-"$8,"exactsite-"$9, "lat_"$10, "lon_"$11, "elev-"$12 }' text.csv > headers && less headers

#identifying duplicates in two fasta files based on unique id...
#grep ">" seq1.fasta | awk 'BEGIN { FS="|"}; {print $1}' > file1
#grep ">" seq2.fasta | awk 'BEGIN { FS="|"}; {print $1}' > file2
#comm -12 file1 file2 > repeats && less repeats

substitute_hdrs() { #This function takes an input file of edited_fasta_format_headers and searches through a fasta_format_sequence file and substitute it's headers if their uniq IDs match

 	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo -e "\tUsage: ${FUNCNAME[0]} -i 'file1.fasta* [file2.fasta* file3.fasta* ...]' [-h <headers>]"
      		return 1
        fi
	
	unset headers
	unset files

        local OPTIND=1
        while getopts 'i:h:' key
        do
                case "${key}" in
                        i)
                                files=$OPTARG
                                for inputfile in $files
                                do
                                        if [ ! -f $inputfile ]
                                        then
                                                echo -e "\tInput file error in `basename -- $inputfile`: provided file does not exist"
                                                echo -e "\tUsage: ${FUNCNAME[0]} -i 'file1.fasta* [file2.fasta* file3.fasta* ...]' [-l <INTEGER>] [-m <INTEGERg|G|m|M>]"
                                                return 1
                                        elif [[ ( -f $inputfile ) && ( `basename -- $inputfile` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
                                        then
						concatenate_fasta_seqs ${inputfile}
                                                continue
                                        fi
                                done
                                ;;
                        h)
				headers=$OPTARG
				if [ ! -f $headers ]
				then
					echo -e "\tinput file error: the headers file (-h) `basename -- $inputfile` does not exist"
					until [[ ( -f "$headers" ) && ( `basename -- "$headers"` =~ .*_(headers|fasta|fa|afa|aln)$ ) ]]
					do
						echo -e "\nFor the headers_[aln|fasta|fa|afa] input, provide the full path to the file, the filename included."
						read -p "Please enter the file to be used as the FASTA headers source: " headers
					done
				elif [[ ( -f "$headers" ) && ( `basename -- "$headers"` =~ .*_(headers|fasta|fa|afa|aln)$ ) ]]
				then
					continue
				else
					echo -e "The file containing headers should be provided via the flag -h <*_(headers|fasta|fa|afa|aln)>"
					return 1
				fi
				#$.*/[\]'^
				sed -i "s/\r$//g; s/ /_/g; s/\&/_n_/g; s/\//_/g; s/'//g; s/\[//g; s/\]//g" $headers
				;;
			?)
				echo "Input error..."
				echo 'Usage: ${FUNCNAME[0]} -i "file1.fasta* [file2.fasta* file3.fasta* ...]" [-h <headers>]'
				return 1
				;;
		esac
	done

	
	echo -e "\n\tStarting operation....\n\tPlease wait, this may take a while...."
	for i in "$files"
	do
		unset records
		number_of_replacements=0
		records=$( grep ">" $i | wc -l )
		unset x
		unset y
		unset z
		echo -e "\nProceeding with `basename -- $i`..."
		for line in `cat ${headers}`
		do
			#x=$( head -10 idrck_headers | tail -1 | awk 'BEGIN { FS="|"; }{print $1;}') && echo $x
			x=`echo "$line" | ${AWK_EXEC} 'BEGIN { RS="\n"; FS="[>|]"; }{ x = $2; print x; }'`
			y=`echo "$line" | ${AWK_EXEC} 'BEGIN { RS="\n"; FS="|"; }{ y = $0; print y; }'`
			#echo -e "\n $x \n $y"

			#Characters to replace from the headers as they will affect the performance of sed: carriage Returns (), white spaces ( ), back slashes (/), and ampersand '&' characters; they greately hamper the next step of header substitution.
			sed -i "s/\r$//g; s/ /_/g; s/\&/_n_/g; s/\//_/g; s/'//g; s/\[//g; s/\]//g" $i

			z=`grep "$x" $i`
			#echo "$z"
			for one_z in `echo -e "${z}"`
			do
				if [ $one_z == $y ]
				then
					echo -e "Change for ${x} already in place..."
					continue
				else
					echo -e "Substituting header for ${x}..."
					sed -i "s/${one_z}/${y}/g" $i
					#sed -i "s/^.*\b${x}\b.*$/${y}/g" $i
				fi
				number_of_replacements=$( expr $number_of_replacements + 1 )
			done
		done
		if [ $? -eq 0 ]
		then
			echo -e "\nDONE. $number_of_replacements replacements done in `basename -- $i` out of $records records it has"
		fi
	done
	echo -e "\n\tCongratulations...Operation done."
}

#===============================================================================================================================================================
edit_ids() { #This function edits the unique identifiers by comparing BOLD ids and GenBank accession numbers to those available within the headers. It requires a file with two columns of BOLD ids and accession numbers, to be indexed, and a fasta file of sequences, to be edited.

	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
		return 1
	fi

	unset input_ids
	unset ids_option

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
		then
			echo -e "Preoceeding with editing IDs in $( basename -- ${i} )"
			#sitting the BOLD-GenBank ids file
			regexp1='^n|N|No|NO|no$'
			if [[ "$ids_option" =~ $regexp1 ]]
			then
				unset input_ids
			fi
			until [[ -f ${input_ids}  ]]
			do
				read -p "Please enter the path to the file with BOLD and GenBank ids to be indexed:: " input_ids
				num_line=0
				while read line || [ -n "$line" ]
				do
					words=`echo "${line}" | wc -w`
					num_line=`expr ${num_line} + 1`
					if [ ${words} -eq 2 ]
					then
						sed -i -E 's/^[[:space:]]+//g; s/[[:space:]]+/\t/g' ${input_ids}
						continue
					else
						echo -e "ERROR! There is/are ${words} word(s) in input line ${num_line}: '${line}'"
						return 1
					fi
				done < ${input_ids}
				if [ $# -gt 1 ]
				then
					regexp='^n|y|N|Y|No|Yes|NO|YES|no|yes$'
					until [[ "$ids_option" =~ $regexp ]]
					do
						echo -e "\nWould you like to use `basename $input_ids` as the reference file for all input files?"
						read -p "Please enter [Yes] or [No] to proceed: " ids_option
					done
				fi
			done

			rename ${i}
			concatenate_fasta_seqs ${i}
			$AWK_EXEC -v input=$input_ids 'BEGIN{OFS="|"};FNR==NR{
			split($0,a,"\t"); ids[FNR,1]=a[1]; ids[FNR,2]=a[2]; Nids=NR }{
			ids[FNR,3]=ids[FNR,1]"("ids[FNR,2]")"} NR>FNR{
			if ( $0 ~ /^>/) {hdr=$0; next} {seq=$0} { 
				split(hdr, b, "[>|]"); $2=b[2]; $3=b[3];
				$4=b[4]; $5=b[5]; $6=b[6]; $7=b[7]; $8=b[8];
			       	$9=b[9]; $10=b[10]; $11=b[11]; $12=b[12];
				$13=b[13]; $14=b[14]; $15=b[15]; $16=b[16];
				$17=b[17]} {
				for(i=1; i<=Nids; i++) if($2 ~ ids[i,1]) {
					$2 = ids[i,3]}
					else if (split($2,d,"[)(]|__") && 
i						d[1] ~ ids[i,1] && 
						d[2] ~ ids[i,2]) {next}
					else if (split($2,d,"[)(]|__") &&
					       	d[2] ~ ids[i,2]) {
							$2 = ids[i,3]}
					else {continue}}{
				print ">"$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17"\n"seq}}' ${input_ids} ${i} > ${src_dir_path}/${output_filename}_edtd.${filename_ext}
			if [ $? -eq 0 ]
			then
				echo -e "\n\tDONE. All edited records have been stored in ${input_src}/${output_filename}_edtd.${filename_ext}\n"
			fi
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
			continue
		fi
	done
}

#===============================================================================================================================================================
delete_repeats() { #This function takes a fasta_format_sequences file and deletes repeats of sequences based on identical headers.
	#in multiple files at once: awk -F'[|]' 'FNR%2{f=seen[$1]++} !f' *
	#in each file: awk -F'[|]' 'FNR==1{delete seen} FNR%2{f=seen[$1]++} !f' *
	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
                return 1
        fi

        for i in "$@"
        do
		rename
		input_src=`dirname "$( realpath "${i}" )"`
		unset duplicate_headers
		unset choice
		unset option
		regexp1='^n|y|N|Y|No|Yes|NO|YES|no|yes$'
		duplicate_headers=`$AWK_EXEC 'BEGIN { FS="[>|(]"}; /^>/{if ($2 ~ /^NA$/) {print $3} else {print $2} }' $i | sort | uniq -d`
		if [ ! -z "$duplicate_headers" ]
		then
			echo -e "\t...`echo -e "$duplicate_headers" | wc -l` records are repeated in $( basename -- ${i} ),\n\t...would you like to proceed and delete all repeats?"
			until [[ "$choice" =~ $regexp1 ]]
			do
				read -p "Please enter [Yes] or [No] to proceed: " choice
			done
		else
			choice="No"
		fi
		case $choice in
			YES|Yes|yes|Y|y)
				echo -e "\t...Will proceed with deleting duplicates in $( basename -- ${i} )"
				concatenate_fasta_seqs $i
				$AWK_EXEC -F'[>|(]' 'FNR==1{delete seen} /^>/{if ($2 ~ /^NA$/) {f=seen[$3]++} else {f=seen[$2]++} } !f' $i > ${input_src}/${output_filename}_cleaned && mv ${input_src}/${output_filename}_cleaned $( realpath "${i}" )
				echo -e "\t...Duplicate records deleted"
				;;
			No|NO|no|N|n)
				if [ ! -z "$duplicate_headers" ]
				then
					echo -e "\tWould you like to save a list of the dublicates?"
					until [[ "$option" =~ $regexp1 ]]
					do
						read -p "Please enter [Yes] or [No] to proceed: " option
					done
					case $option in
						YES|Yes|yes|Y|y)
							echo -e "\tCancelling....\nThe list of repeated sequences is in file called '_duplicates'\n"
							echo -e "$duplicate_headers" > ${input_src}/${output_filename}_duplicates
							;;
						No|NO|no|N|n)
							echo -e "\tCancelling...\n"
							;;
						*)
							echo "ERROR!!! Invalid selection"
							;;
					esac
				else
					echo -e "\tNo duplicate records in $i\n"
				fi
				;;
			*)
				echo "ERROR!!! Invalid selection"
				;;
		esac
        done

}

#===============================================================================================================================================================
move_matched() { #This is updated version of move_unwanted function 
	# It is used within the filter_seqs function below. It moves and deletes the matching fasta records.
	for i in "$@"
	do
		matching_records=$(grep "$pattern_name" $i | wc -l)
		#echo -e "$i"
		#echo -e "${pattern_name}"
		echo -e "${matching_records} records match the pattern $pattern_name"
		if [ $matching_records -gt 0 ]
		then
			echo -e "\tSearching headers with '$pattern_name' word-pattern in `basename $i`..."
			$AWK_EXEC -v pat=$pattern_name '/^>/{
			hdr=$0; next} {
			seq=$0 } {
			pattern=pat; gsub(/\|/,"\\|",pattern)} hdr~pattern{
			print hdr; print seq }' $i >> ${input_src}/${output_filename}_${suffix}.${filename_ext}
			
			#Deleting matched records
			regexpno='^n|N|No|NO|no$'
			regexpyes='^y|Y|Yes|YES|yes$'
			if [[ $Choice =~ $regexpyes ]]
			then
				echo -e "The records that match $pattern_name have been deleted from `basename $i`"
				sed -i "/$pattern_name/,+1 d" $i
			elif [[ ${Choice} =~ $regexno ]]
			then
				echo -e "The records that match $pattern_name have been retained in `basename $i`"
			fi
		fi
        done
}


filter_seqs() { # Updated version of delete_unwanted function.
	#this function copys a record that fits a provided pattern, e.g a taxon_name_description; the arguments provided, are the files to be searched for the patterns
	# LOGIC behind the code: To get the list of orders in description_taxon_names and their frequencies, from  which to select the undesired patterns (names), do:
	#grep ">" seqs.fasta | awk 'BEGIN {FS="|"; OFS="|" ; }; {print $2}' |sort | uniq -c > seqs_orders && less seqs_orders
	
	set -E
	trap ' return 1 ' ERR
	
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
		return 1
	fi
	
	for i in "$@"
	do
		 if [ ! -f $i ]
 		 then
 			 echo "input error: file $i is non-existent!"
			 return 1
 		 elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln)$ ) ]]
 		 then
	 		 echo -e "\nProceeding with `basename -- $i`..."
	 		 rename
	 		 input_src=`dirname "$( realpath "${i}" )"`
	 		 concatenate_fasta_seqs ${i}
	 
			 unset options
	 		 echo -e "To extract sequences with specific words in the headers please select one of the options [1|2|3] to proceed or [4] to cancel"
	 		 options[0]="Copy records with word patterns specified in a file into one file"
	 		 options[1]="Copy records with string-patterns specified in a file into individual word-pattern-specific files"
	 		 options[2]="Copy records with specific single string into a file"
	 		 options[3]="Exit"
	 
			 PS3='Select one of the filter methods [1|2|3], or [4] to exit: '
	 		 select option in "${options[@]}"
	 		 do
	 			 unset pattern_name
	 			 unset input_pattern_file
	 			 regexp='^[a-zA-Z0-9/_-\ \|]+$'
	 			 regexp1='^n|y|N|Y|No|Yes|NO|YES|no|yes$'
				 
				 case $option in
	 				 ${options[0]})
	 					 #echo "no error"
	 					 until [[ -f ${input_pattern_file}  ]]
		 				 do
	 						 read -p "Please enter the path to the file with pattern names to be extracted:: " input_pattern_file
	 						 #cat ${input_pattern_file}
	 					 done
						 #deleting matches
	 					 echo -e "\nIf you wish to delete records that match the search patterns in $input_pattern_file from `basename $i` enter [YES] or [NO] if you wish to retain them.\n"
	 					 unset Choice
	 					 read -p "Please enter [Yes] or [NO] to proceed:: " Choice
	 					 until [[ "$Choice" =~ $regexp1 ]]
	 					 do
	 						 echo "INVALID choice $REPLY"
	 					 done
		
						 for line in `cat $(echo ${input_pattern_file})`
	 					 do
							 #echo -e "$line"
							 pattern_name=${line}
	 						 suffix="matched"
	 						 #echo "${pattern_name}"
	 						 move_matched "${i}"
	 					 done
	 					 echo -e "\n\tDONE. All matched records have been stored in '${output_filename}_${suffix}.fasta'"
	 					 break
	 					 ;;
	 				 ${options[1]})
	 					 until [[ -f ${input_pattern_file} ]]
	 					 do
	 						 read -p "Please enter the path to the file with pattern names to be extracted:: " input_pattern_file
	 					 done
	 
						 echo -e "\nIf you wish to delete records that match the search patterns in $input_pattern_file from `basename $i` enter [YES] or [NO] if you wish to retain them.\n"
	 					 unset Choice
	 					 read -p "Please enter [Yes] or [NO] to proceed:: " Choice
	 					 until [[ "$Choice" =~ $regexp1 ]]
	 					 do
	 						 echo "INVALID choice $REPLY"
	 					 done
	 
						 for line in `cat ${input_pattern_file}`
	 					 do
	 						 pattern_name=${line}
	 						 suffix=${pattern_name}
		 					 move_matched "${i}"
	 						 echo -e "\n\tDONE. All matched records have been stored in '${output_filename}_${suffix}.fasta'"
	 					 done
	 					 break
	 					 ;;
	 				 ${options[2]})
	 					 until [[ "$pattern_name" =~ $regexp ]]
	 					 do
	 						 read -p "Please enter string pattern to be searched:: " pattern_name
	 					 done
	 
						 echo -e "\nIf you wish to delete records that match the search pattern '$pattern_name' from `basename $i` enter [YES] or [NO] if you wish to retain them.\n"
	 					 unset Choice
	 					 read -p "Please enter [Yes] or [NO] to proceed:: " Choice
	 					 until [[ "$Choice" =~ $regexp1 ]]
	 					 do
	 						 echo "INVALID choice $REPLY"
	 					 done
			 
	 					 suffix=`echo ${pattern_name} | sed 's/|/\./g'`
	 					 move_matched "${i}"
	 					 echo -e "\n\tDONE. All matched records have been stored in '${output_filename}_${suffix}.fasta'"
	 					 break
	 					 ;;
	 				 ${options[3]})
	 					 echo -e "Exiting filtering of pattern-matched sequences..."
	 					 break
	 					 ;;
	 				 *)
	 					 echo "INVALID choice: Select option [1|2|3] to delete, or [4] to exit: "
	 					 ;;
	 			 esac
	 		 done
		 else
 			 echo "input file error in `basename $i`: input file should be a .afa file format"
 			 continue
 		 fi
	done
}



#===============================================================================================================================================================

trimming_seqaln() { #This function trims aligned sequences in a file on both ends and retains a desired region based on input field (sequence position) values.

	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]\nInput files must be well aligned files"
                return 1
        fi

	for i in "$@"
	do
		if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
                then
			input_src=`dirname "$( realpath "${i}" )"`
			rename
			echo -e "\tThis function will trim your sequences at specific positions and output the desired columns. Proceed and enter the desired start and end positions of the blocks to extract.\n\tTrimming `basename -- $i`..."
			unset start_pos
			unset end_pos
			regexp='^[0-9]+$'
			until [[ "$start_pos" =~ $regexp ]]
			do
				read -p "Please enter the start position: " start_pos
			done

			until [[ "$end_pos" =~ $regexp ]]
			do
				read -p "Please enter the end position: " end_pos
			done

			echo -e "\tYou are trimming `basename -- ${i}` at position ${start_pos} to ${end_pos}"

			concatenate_fasta_seqs $i
			$AWK_EXEC -v start_p=$start_pos -v end_p=$end_pos \
				'BEGIN{FS=""; OFS=""; }; /^>/ {if (FNR==1) {print $0; } else { print "\n" $0 }}; !/^>/ { for(v=start_p; v<=end_p; v++) { printf "%s", $v; if (v <= end_p) { printf "%s", OFS; } }}' $i > ${input_src}/${output_filename}_trmmd${start_pos}-${end_pos}.${filename_ext}
			#awk 'BEGIN {FS=""; OFS=""; }; /^>/ {print "\n" $0 }; !/^>/ { for(v=1087; v<=2574; v++) { printf "%s", $v; if (v <= 2574) { printf "%s", OFS; } else { printf "\n"; } }}' input.aln | less
			if [ $? -eq 0 ]
			then
				echo -e "\n\tDONE. All trimmed records have been stored in ${input_src}/${output_filename}_trmmd${start_pos}-${end_pos}.${filename_ext}\n"
			fi
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
		fi
	done
}


#===============================================================================================================================================================

delete_shortseqs_gaps() { #This function identifies and removes sequences that have specified number of gaps at the ends. It stores the cleaned sequences.

	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
                return 1
        fi

        for i in "$@"
        do
                if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
                then
                        input_src=`dirname "$( realpath "${i}" )"`
                        rename
			concatenate_fasta_seqs $i

			echo -e "\tThis function will remove sequences that have a specified number of gaps, '-', at the start or end of the sequence. Proceed and enter the accepted maximum number of gaps at the start and end positions of the sequences.\n\tIntegers only accepted!!!"
			unset start_gaps
			unset end_gaps
			regexp='^[0-9]+$'
                        until [[ "$start_gaps" =~ $regexp ]]
                        do
				read -p "Please enter the muximum number of gaps allowed at start position: " start_gaps
			done

                        until [[ "$end_gaps" =~ $regexp ]]
                        do
				read -p "Please enter the maximum number of gaps allowed at the end position: " end_gaps
			done

                        echo -e "\tRemoving sequences in `basename -- ${i}` that have more than ${start_gaps} gaps in start position and ${end_gaps} in end position"

			$AWK_EXEC -v start_g=$start_gaps -v end_g=$end_gaps ' /^>/ { hdr=$0; next }; match($0,/^-*/) && RLENGTH<=start_g && match($0,/-*$/) && RLENGTH<=end_g { print hdr; print }' $i > ${input_src}/${output_filename}_sn${start_gaps}-eg${end_gaps}.aln
			#awk -v start_g=$start_gaps -v end_g=$end_gaps ' /^>/ { hdr=$0; next }; (x="^-{"start_g+1",}")(y="-{"end_g+1",}$") !match($0, x) && !match($0, y)  {print x y hdr; print }' input_trmmd.aln | wc -l #Does not remove all as needed
			if [ $? -eq 0 ]
			then
				echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_sg${start_gaps}-eg${end_gaps}.aln\n"
			fi
                else
                        echo "input file error in `basename $i`: input file should be a .aln file format"
                        continue
                fi
        done
}

#===============================================================================================================================================================

set_start_Ngaps() { # This functions sets the value of the variable "start_Ngaps"
	until [[ "$start_Ngaps" =~ $regexp ]]
	do
		read -p "Please enter the muximum number of '-'s or Ns allowed at 3' start position: " start_Ngaps
	done
}
set_end_Ngaps() { #This function sets the value of the variable "end_Ngaps"
	until [[ "$end_Ngaps" =~ $regexp ]]
	do
		read -p "Please enter the maximum number of '-'s or Ns allowed at the 5' end position: " end_Ngaps
	done
}
set_sum_Ngaps() { #This function sets the value of the variable "sum_Ngaps"
	until [[ "$sum_Ngaps" =~ $regexp ]]
	do
		read -p "Please enter the total maximum number of '-'s and Ns allowed in both 3' and 5' ends of the sequence: " sum_Ngaps
	done
}
set_mid_Ns() { #This function sets the value of the variable "mid_Ns"
	 until [[ "$mid_Ns" =~ $regexp ]]
	 do
		 read -p "Please enter the maximum length of N-string allowed within the sequence: " mid_Ns
	 done
}


delete_shortseqs() { #This function identifies and removes sequences that have specified number of gaps "-" and Ns at the ends or Ns strings within. It stores the cleaned sequences.

	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
		return 1
	fi
	
	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename
			#concatenate_fasta_seqs $i
			unset options
						
			echo -e "\tYou are going to remove sequences that have more than a specific number of undefined nucleotides, 'N', or gaps, '-', at the beginning and end or specific maximum length of N character-string within the sequence."
			echo -e "\tWhich type of characters are to consider?"
			PS3='Select a choice from the following options, Enter [1] or [2] or [3] or [4] or [5]: '
			options=("Gaps at the 3' or 5' ends of sequences" "Ns and gaps at the 3' and 5' ends" "Gaps & Ns in the entire sequence" "Summation of Ns and gaps at the 3' and 5' ends" "Quit")
			select opt in "${options[@]}"
			do
				regexp='^[0-9]+$'
				unset start_Ngaps
				unset end_Ngaps
				unset mid_Ns
				unset sum_Ngaps

				case $opt in
					${options[0]})
						echo -e "\tProceed and enter the accepted maximum number of '-'s at the start and end positions of the sequences.\n\tIntegers only accepted!!!"
						set_start_Ngaps

						set_end_Ngaps

						$AWK_EXEC -v start_g=$start_Ngaps -v end_g=$end_Ngaps ' /^>/ { hdr=$0; next }; match($0,/^-*/) && RLENGTH<=start_g && match($0,/-*$/) && RLENGTH<=end_g { print hdr; print }' $i > ${input_src}/${output_filename}_sg${start_Ngaps}-e${end_Ngaps}.aln
						
						echo -e "\n\tDONE. All cleaned records have been stored in ${input_src}/${output_filename}_sg${start_Ngaps}-e${end_Ngaps}.aln\n"
						break
						;;
					${options[1]})
						echo -e "\tProceed and enter the accepted maximum number of Ns or '-'s at the start and end positions of the sequences.\n\tIntegers only accepted!!!"
						set_start_Ngaps

						set_end_Ngaps

						$AWK_EXEC -v start_N=$start_Ngaps -v end_N=$end_Ngaps ' /^>/ { hdr=$0; next }; match($0,/^[-Nn]*/) && RLENGTH<=start_N && match($0,/[-Nn]*$/) && RLENGTH<=end_N { print hdr; print }' $i > ${input_src}/${output_filename}_s${start_Ngaps}-e${end_Ngaps}.aln
						echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_s${start_Ngaps}-e${end_Ngaps}.aln\n"
						break
						;;
					${options[2]})
						echo -e "\tProceed and enter the accepted maximum number of Ns or '-'s at the start, within and end positions of the sequences.\n\tIntegers only accepted!!!"
						set_start_Ngaps

						set_mid_Ns
						
						set_end_Ngaps
						
						echo -e "\tRemoving sequences in `basename -- ${i}` that have more than ${start_Ngaps} Ns or '-'s in start position, ${mid_Ns} length N string within  and ${end_Ngaps} in end position"
						$AWK_EXEC -v start_N=$start_Ngaps -v mid_N=$mid_Ns -v end_N=$end_Ngaps '
						/^>/{hdr=$0; next}
						{ seq=$0 }
						match(seq,/^[-Nn]*/) && RLENGTH > start_N { next }
						{ seq=substr(seq,RSTART+RLENGTH) }
						match(seq,/[-Nn]*$/) && RLENGTH > end_N { next }
						{ seq=substr(seq,1,RSTART-1) }
						{ while (match(seq,/[-Nn]+/)) {
							if(RLENGTH>mid_N) next
							seq=substr(seq,RSTART+RLENGTH)
							}
						}
						{ print hdr; print $0 }' $i > ${input_src}/${output_filename}_s${start_Ngaps}-${mid_Ns}-e${end_Ngaps}.aln
						echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_s${start_Ngaps}-${mid_Ns}-e${end_Ngaps}.aln\n"
						break
						;;
					${options[3]})
						echo -e "\tProceed and enter the maximum sum of Ns or '-'s at the 3' and 5' ends of the sequences.\n\tIntegers only accepted!!!"
						set_sum_Ngaps

						$AWK_EXEC -v sum_Ngaps=$sum_Ngaps ' /^>/ { hdr=$0; next }; match($0,/^[-Nn]*/) {start_N=RLENGTH}; match($0,/[-Nn]*$/) {end_N=RLENGTH}; sum_Ngaps>=start_N+end_N { print hdr; print }' $i > ${input_src}/${output_filename}_ends${sum_Ngaps}.aln
						echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_ends${sum_Ngaps}.aln\n"
						break
						;;
					${options[4]})
						break
						;;
					*) echo "INVALID choice $REPLY"
						;;
				esac
			done
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}
#===============================================================================================================================================================

remove_gaps() { # Removing gaps, "-" in a sequence.
	# Works only for singular line header and sequence lines, i.e does not concatenate separate lines of the same sequence.
	#awk 'BEGIN{ RS="\n";ORS="\n" }/^>/{print}; !/^>/{ gsub("-","",$0); print $0 }' enafroCOI_500to700_data-650to660_st22n1006-en1474n3479.aln | less -S

	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
		return 1
	fi

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
   		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename
			concatenate_fasta_seqs $i

			echo -e "\tRemoving gaps, '-', from `basename -- ${i}`"
			$AWK_EXEC 'BEGIN{ RS="\n";ORS="\n" }/^>/{print}; !/^>/{ gsub("-","",$0); print $0 }' $i > ${input_src}/${output_filename}_dgpd.${filename_ext}
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done

}


#===============================================================================================================================================================

concatenate_fasta_seqs() { # This function converts a multiple line FASTA format sequence into a two line record of a header and a sequnce lines.
	#awk '/^>/ {if (FNR==1) print $0; else print "\n" $0; }; !/^>/ {gsub("\n","",$0); printf $0}' renafroCOI_500to700_data-650to660_st22n1006-en1474n3479_head25000_tail125001.afa | less -S


	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} seqfile1.fasta [seqfile2.fasta seqfile3.fasta ...]"
		return 1
	fi

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
		then
			echo -e "\t...Concatinating sequence lines for each record in `basename -- ${i}`..."
			input_src=`dirname "$( realpath "${i}" )"`
			$AWK_EXEC '/^>/ {if (FNR==1) print $0; else print "\n" $0; }; !/^>/ {gsub("\n","",$0); printf $0}' $i > ${input_src}/outfile.afa #&& mv ${input_src}/outfile.afa ${i}
			if [ $? -eq 0 ]
                        then
				echo -e "\t moving ${input_src}/outfile.afa to ${input_src}/${i}"
				mv ${input_src}/outfile.afa ${input_src}/${i}
			fi
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}


#===============================================================================================================================================================

retrive_originalseqs() { # Retriving a subset of unaligned sequences from original parent file. The input is an FASTA file that has been modified (trimmed or Ns or gaps removed).

	set -E
	trap ' return 1 ' ERR

	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} seqfile1.fasta [seqfile2.fasta seqfile3.fasta seqfile4.fasta]"
		return 1
	fi
	
	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa) ) ]]
		then
			inputfile=${i}
			unset parentfile
			until [[ ( -f $parentfile ) && ( `basename -- "$parentfile"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
			do
				read -p "Please enter the path to the parent file with the orginal sequences:: " parentfile
			done

			rename
			outputfilename=${output_filename}
			concatenate_fasta_seqs ${inputfile} ${parentfile}
			# lets open and save both files using an editor i.e vim to ensure all lines have an "newline" ending.
			vim ${inputfile} <<- EOF
			^[
			:wq!
			EOF
			vim ${parentfile} <<- EOF
			^[
			:wq!
			EOF
			input_src=`dirname "$( realpath "${inputfile}" )"`
			echo -e "\t...Copying ${inputfile} to ./retrinput.fasta"
			cat ${inputfile} > ${input_src}/retrinput.fasta
			echo -e "\t...Appending ${parentfile} to ./retrinput.fasta"
			cat ${parentfile} >> ${input_src}/retrinput.fasta
			delete_repeats ${input_src}/retrinput.fasta <<- EOF
			y
			EOF
			x=$( cat ${inputfile} | wc -l )
			echo -e "\t...Extracting records from ./retrinput.fasta not found in ${inputfile} to ./retrinput1.fasta"
			$AWK_EXEC -v x=$x '{if ( FNR<=x ) {next} else {print $0} }' ${input_src}/retrinput.fasta > ${input_src}/retrinput1.fasta
			echo -e "\t...Copying ./retrinput1.fasta to ./retrinput2.fasta"
			cat ${input_src}/retrinput1.fasta > ${input_src}/retrinput2.fasta
			echo -e "\t...Appending ${parentfile} to ./retrinput2.fasta"
			cat ${parentfile} >> ${input_src}/retrinput2.fasta 
			delete_repeats ${input_src}/retrinput2.fasta <<- EOF
			y
			EOF
			y=$( cat ${input_src}/retrinput1.fasta | wc -l )
			echo -e "\t...Extracting records from ./retrinput2.fasta not found in ./retrinput1.fasta to .${outputfilename}_original.fasta"
			$AWK_EXEC -v y=$y '{if ( FNR<=y ) {next} else {print $0} }' ${input_src}/retrinput2.fasta > ${input_src}/${outputfilename}_original.fasta
			rm ${input_src}/retrinput2.fasta ${input_src}/retrinput1.fasta ${input_src}/retrinput.fasta 
			if [ $? -eq 0 ]
			then
				echo -e "The retrived original records are saved in ${input_src}/${outputfilename}_original.fasta"
			fi
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}


