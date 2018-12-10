#!/usr/bin/awk -f

BEGIN{Header = substr($1,$14,$83)}
{ if (NR != 1) printf">",$1,"|" }
