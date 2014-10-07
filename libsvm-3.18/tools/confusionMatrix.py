#!/usr/bin/env python
import sys
import os.path
from collections import defaultdict

validOptions = ["-r", "-h", "-o", "-a"]
if len(sys.argv) <= 1:
	print('Usage: {0} -options'.format(sys.argv[0]))
	print('Options are:')
	print('\t-r : rad confusion matrix')
	print('\t-h : hjpd confusion matrix')
	print('\t-o : hod confusion matrix')
	print('\t-a : print all (rad/hjpd/hod) confusion matrices')
	print('\nTo print all matrices use: \n\t{0} -a'.format(sys.argv[0]))
	print('You can specify more than one option, such as print confusion matrices for rad and hjpd.')
	print('Example: {0} -r -h\n'.format(sys.argv[0]))
	raise SystemExit

# Parse options
matrixOrder = []
if len(sys.argv) >= 2:	# have at least one option
	for i in range(1, len(sys.argv)):
		if sys.argv[i] not in validOptions:
			print('Invalid option: {0}'.format(sys.argv[i]))
			raise SystemExit
		else:
			if sys.argv[i] == '-r':
				matrixOrder.append("rad")
			elif sys.argv[i] == '-h':
				matrixOrder.append("hjpd")
			elif sys.argv[i] == '-o':
				matrixOrder.append("hod")
			elif sys.argv[i] == '-a' and len(matrixOrder) == 0:
				matrixOrder.append("rad")
				matrixOrder.append("hjpd")
				matrixOrder.append("hod")

activities = ["8", "10", "12", "13", "15", "16"]

for mfile in matrixOrder:
	# Initialize confusion matrix
	confusionMtx = defaultdict(dict)
	for activity in activities:
		for activity2 in activities:
			confusionMtx[activity][activity2] = 0

	testFileName = mfile + ".t"
	predictFileName = mfile + ".t.predict"
	if (os.path.isfile(testFileName) and os.path.isfile(predictFileName)):
		
		testFileActivies = []
		testFile = open(testFileName, 'r')
		for line in testFile:
			tActivity = line.split(" ")[0].rstrip("\n")
			testFileActivies.append(tActivity)

		predictedFileActivities = []
		predictFile = open(predictFileName, 'r')
		for line in predictFile:
			pActivity = line.rstrip("\n")
			predictedFileActivities.append(pActivity)

		assert len(testFileActivies) == len(predictedFileActivities), "Test and predicted activity number mismatch"
		# Calculate confusion matrix
		for i in range(0, len(testFileActivies)):
			test = testFileActivies[i]
			predict = predictedFileActivities[i]
			confusionMtx[test][predict] += 1
		# Output Confusion matrix
		print('\nPrinting confusion matrix for {0}'.format(mfile))
		print('{0:4s} {1:3s} {2:3s} {3:3s} {4:3s} {5:3s} {6:3s}'.format("   ", " 8", "10", "12", "13", "15", "16"))
		for act in activities:
			print('{0:3s} {1:3d} {2:3d} {3:3d} {4:3d} {5:3d} {6:3d}'.format(act, confusionMtx[act]["8"], confusionMtx[act]["10"],\
					confusionMtx[act]["12"], confusionMtx[act]["13"], confusionMtx[act]["15"], confusionMtx[act]["16"]))

		predictFile.close()
		testFile.close()
	else:	# Error with a file name
		print('\n***Error***')
		print('File: {0}.t and {0}.t.predict cannot be opened.'.format(mfile))
		print('Please make sure the files [rad | hjpd | hod] and their corresponding .t files have been copied over from /nygren-Project2-Code')
		print('and that you have run easy.py to produce the *.t.predict files')