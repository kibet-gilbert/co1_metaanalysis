#!/usr/bin/python
#collectdata.py
import os
import sys

t = open('counts.txt', 'w')
t.write("%s\t%s\t%s\t" % ((sys.argv[2]), (sys.argv[3]), (sys.argv[1])))
t.close()
os.system("paste %s_files/%s.bneck.tsimtest.match.txt %s_files/%s.expan.tsimtest.match.txt %s_files/%s.emp.match.txt counts.txt > %s.data.txt" % ((sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1])))
