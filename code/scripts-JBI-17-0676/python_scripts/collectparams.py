#!/usr/bin/python
#collectdata.py
import os
import sys

#takes arguments 'genus_species'
t = open('name.txt', 'w')
t.write("%s" % ((sys.argv[1])))
t.close()
os.system("mv name.txt %s_files" % ((sys.argv[1])))

os.system("cut -f 2 < %s_files/%s.tprior.txt > %s_files/%s.temp.txt" % ((sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1])))
os.system("head -10 < %s_files/%s.temp.txt > %s_files/%s.bneck.param.txt" % ((sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1])))
os.system("tail -10 < %s_files/%s.temp.txt > %s_files/%s.expan.param.txt" % ((sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1])))

os.system("paste %s_files/%s.bneck.param.txt %s_files/%s.expan.param.txt %s_files/name.txt > %s.params.txt" % ((sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1]), (sys.argv[1])))
