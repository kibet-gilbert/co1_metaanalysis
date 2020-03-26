#!/usr/bin/python
#note, columns in 'output.txt' as follows:
import math 
import random
import os
import sys

#assumes following programs are executable
#sample_stats
#ms
#msReject

#takes arguments 'genus_species', Number of samples, number of segregating sites

#generate ms output
os.system("./ms %f 1 -s %f > %s.msout.txt" % ((float(sys.argv[2])), (float(sys.argv[3])), (sys.argv[1])))

#generate empirical sample stats file for input
os.system("head -6 %s.msout.txt > temp.txt" % (sys.argv[1]))
os.system("cat temp.txt %s.afa > %s.emp.txt" % ((sys.argv[1]), (sys.argv[1])))

#generate empirical summary stats using sample_stats
os.system("./sample_stats < %s.emp.txt | cut -f 2,4,6,8,10 > temp.ss.txt" % (sys.argv[1]))

#combines values with dummy variables to enable comparison to prior with msReject, writes ss text file
os.system("paste dummy.txt temp.ss.txt > %s.ss.txt" % (sys.argv[1]))


#generate test prior distributions
#defines number of draws for the test prior
tprior = 9

#counter for test
tcount = 0

while tcount <= tprior:
	#output file
	import sys
	t = open('toutput.txt', 'a')

	#parameter definitions
	Btneck = random.uniform(0.01, 1.0)
	Expando =  random.uniform(1.0, 10)
	Time = random.uniform(0.01, 0.4)

	#population bottleneck during last glacial period 
	os.system("./ms %f 1 -s %f -eN %f %f | ./sample_stats | cut -f 2,4,6,8,10 >> %s.toutput.txt" % ((float(sys.argv[2])), (float(sys.argv[3])), Time, Btneck, (sys.argv[1])))
	t.write('bneck\t%f\t%f\n' % (Btneck, Time))

	#population larger during last glacial period 
	os.system("./ms %f 1 -s %f -eN %f %f | ./sample_stats | cut -f 2,4,6,8,10 >> %s.toutput.txt" % ((float(sys.argv[2])), (float(sys.argv[3])), Time, Expando, (sys.argv[1])))
	t.write('expan\t%f\t%f\n' % (Expando, Time))
	tcount += 1

t.close()
	
#make tprior file
os.system("paste toutput.txt %s.toutput.txt | sort -d > %s.tprior.txt" % ((sys.argv[1]), (sys.argv[1])))
os.system("rm toutput.txt %s.toutput.txt" % (sys.argv[1]))


#defines the number of draws from the prior distribution (use small number for testing, then change)
Priorsize = 99999

#don't change this, it defines the starting number for Priorsize
Counter = 0

#while loop iterates draws from the prior for the number of models that you are generating a prior for - here, we have two models
while Counter <= Priorsize:

	#output file
	import sys
	f = open('output.txt', 'a')
	
	#parameter definitions
	Btneck = random.uniform(0.01, 1.0)
	Expando =  random.uniform(1.0, 10)
	Time = random.uniform(0.01, 0.4)
	
	#population bottleneck during last glacial period 
	os.system("./ms %f 1 -s %f -eN %f %f | ./sample_stats | cut -f 2,4,6,8,10 >> %s.output.txt" % ((float(sys.argv[2])), (float(sys.argv[3])), Time, Btneck, (sys.argv[1])))
	f.write('bneck\t%f\t%f\n' % (Btneck, Time))
	
	#population larger during last glacial period 
	os.system("./ms %f 1 -s %f -eN %f %f | ./sample_stats | cut -f 2,4,6,8,10 >> %s.output.txt" % ((float(sys.argv[2])), (float(sys.argv[3])), Time, Expando, (sys.argv[1])))
	f.write('expan\t%f\t%f\n' % (Expando, Time))
	
	Counter += 1

f.close()

#make prior file
os.system("paste output.txt %s.output.txt > %s.prior.txt" % ((sys.argv[1]), (sys.argv[1])))
os.system("rm output.txt %s.output.txt" % (sys.argv[1]))

#rejection threshold size
rts = 0.0005

os.system("./msReject %s.ss.txt %s.prior.txt %f 6 7 > %s.posterior.txt" % ((sys.argv[1]), (sys.argv[1]), float(rts), (sys.argv[1])))
os.system("grep -c 'bneck' < %s.posterior.txt >> %s.emp.match.txt" % ((sys.argv[1]), (sys.argv[1])))

#counter for rejection steps
bneckcnt = 1
expancnt = 11

#number of simulation tests
nst = 10

#while loop to iterate through lines of prior distribution
while bneckcnt <= nst:
	os.system("head -%s %s.tprior.txt | tail -1 > test_target.txt" %  (bneckcnt, (sys.argv[1])))
	os.system("./msReject test_target.txt %s.prior.txt %f 6 7 > %s.bneck.tposterior.txt" % ((sys.argv[1]), float(rts), (sys.argv[1])))
	os.system("grep -c 'bneck' < %s.bneck.tposterior.txt >> %s.bneck.tsimtest.match.txt" % ((sys.argv[1]), (sys.argv[1])))

	
	os.system("head -%s %s.tprior.txt | tail -1 > test_target.txt" %  (expancnt, (sys.argv[1])))
	os.system("./msReject test_target.txt %s.prior.txt %f 6 7 > %s.expan.tposterior.txt" % ((sys.argv[1]), float(rts), (sys.argv[1])))
	os.system("grep -c 'expan' < %s.expan.tposterior.txt >> %s.expan.tsimtest.match.txt" % ((sys.argv[1]), (sys.argv[1])))	
	
	bneckcnt += 1
	expancnt += 1
	
os.system("mkdir %s_files" % (sys.argv[1]))
os.system("mv %s.* %s_files" % ((sys.argv[1]), (sys.argv[1])))
os.system("rm temp.ss.txt temp.txt test_target.txt")
