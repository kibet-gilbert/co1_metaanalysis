#!/usr/bin/awk -f

BEGIN {FS="\t"}
{if (NR != 1) print  ">"$1F"|"$14F"|""l-"$83"\n"$82 > "./Data/bold_fasta_data"}
END {}

#BEGIN{FS="\t"; Header = substr($1,$14,$83)}
#{ if (NR != 1) printf">", Header; FS="|" }
