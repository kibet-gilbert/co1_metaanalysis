#!/usr/bin/awk -f

BEGIN { FS="\t"; OFS="|" }
NR == 1 { next }
{ print  ">" $1, $14, "l-" $83 "\n" $82 }

#BEGIN{FS="\t"; Header = substr($1,$14,$83)}
#{ if (NR != 1) printf">", Header; FS="|" }
