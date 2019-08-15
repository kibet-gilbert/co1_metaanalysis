#!/usr/bin/awk -f

BEGIN { FS="\t"; OFS="|" }
NR == 1 { next }
{ for(i=1; i<=NF; i++) if($i ~ /^ *$/) $i = "NA" }; { print  ">" $1, $10, $12, $14, "fam-"$16, "subfam-"$18, "tri-NA", "gs-"$20, "sp-"$22, "subsp-"$24, "country-"$55,"exactsite-"$59, "lat_"$47, "lon_"$48, "elev-"$51, "l-" $83 "\n" $82 }

#$ awk 'BEGIN { FS="\t"; OFS="|" } ; NR == 1 { next } { print  ">" $1, $14, "gs-"$20, "sp-"$22, "subsp-"$24, "country-"$55,"exactsite-"$59, "lat_"$47, "lon_"$48, "elev-"$51, "l-" $83 "\n" $82 }' COI_all_data.tsv | less
#{ for(i=1; i<=NF; i++) if($i ~ /^ *$/) $i = "NA" }; 1' file
