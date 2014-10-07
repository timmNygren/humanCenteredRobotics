#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <cmath>
#include "point2d.h"
#include "bin.h"

using namespace std;

const int MAX_JOINTS = 20;
// const int NUM_DIMENSIONS = 3;
const int TOTAL_NUM_JOINT_DIFFERENCES = 19;
const double HOD_LOWER_BOUND = 0;
const double HOD_UPPER_BOUND = 360;
const int HOD_NUM_BINS = 8;
const int XY_TRAJECT = 0;
const int YZ_TRAJECT = 1;
const int XZ_TRAJECT = 2;

int referenceJoint = 0;				// Default Center of hip
static int testCounter = 0;
// static double minValue = 999999.0;
// static double nextMin;
// static double maxValue = -10000000.0;
// static double nextMax;

void exit_with_help();
vector<Point2d> calc_joint_differences(const vector<Point2d> joints);
vector<Bin> compute_histogram(int jointStart, int inc, int trajectory, double lower, double upper, int numBins, vector<vector<Point2d> > data);
vector<double> create_final_descriptor(vector<vector<Point2d> > data);
vector<double> calc_3d_trajectory(int joint, vector<vector<Point2d> > data);
vector<Bin> temporal_pyramid(int joint, vector<vector<Point2d> > data);
void combine_feature_vector(vector<double>& feature, vector<Bin> input);
void write_LIBSVM(string id, vector<double> features, ofstream& output);
void transform_to_HOD(const char *path, const string output);

int main(int argc, char *argv[]) 
{

	if (argc != 3)
	{ 
		exit_with_help();
		exit(1);
	}	

	transform_to_HOD(argv[1], "hod");
	transform_to_HOD(argv[2], "hod.t");
	return 0;
}

void exit_with_help() 
{
	cout << "Usage: hjpd train_dir test_dir \n";
	cout << "Make sure train_dir is the first, otherwise\n";
	cout << "The output hjpd file will contain incorrect data\n";
}

vector<Point2d> calc_joint_differences(const vector<Point2d> joints)
{
	vector<Point2d> tempDifferences;
	// for (int i = 0; i < joints.size(); i++)
	// 	if (i != referenceJoint)
	// 		tempDifferences.push_back(joints[referenceJoint].difference(joints[i]));
	// if (testCounter == 0)
	// {
	// 	cout << "\tTesting calc joint differences size: " << tempDifferences.size() << endl;
	// 	for(int i = 0; i < tempDifferences.size(); i++)
	// 		cout << "\t Joint difference: " << i << " = " << tempDifferences[i] << endl;
	// 	testCounter++;
	// }

	return tempDifferences;
}

vector<Bin> compute_histogram(int jointStart, int inc, int trajectory, double lower, double upper, int numBins, vector<vector<Point2d> > data)
{
	// cout << "COMPUTE_HISTOGRAM NOT IMPLEMENTED\n";
	upper = upper * numBins / 360;
	lower = lower * numBins / 360;
	double interval = (upper - lower) / numBins;
	vector<Bin> histogram;

	// // Initialize histogram of numBins
	for (int i = 0; i < numBins; i++)
	{
		histogram.push_back(Bin(lower, interval));
		lower += interval;
	}

	// Go through all frames to calc distance and angles between
	// P_t, P_t+1
	double length;
	double angle;
	// double totalInBins = 0;
	// if (testCounter == 1)
	// {
	// cout << "data size: " << data.size() << endl;
	// testCounter++;
	for (int i = jointStart; i < data.size() - 1; i += inc*MAX_JOINTS)
	{
		// cout << "i: " << i << endl;
		// Calc distance and angle of P_t, P_t+1
		// P_t is data[i][joint][trajectory]
		// P_t+1 is data[i+1][joint][trajectory]
		// cout << "compute_histogram frame: " << i << endl;
		length = data[i][trajectory].distance(data[i+1][trajectory]);
		angle = data[i][trajectory].angle(data[i+1][trajectory]);
		// cout << "Frame: " << i << " joint: " << joint << " length: " << length << " angle: " << angle << endl;
		for (int j = 0; j < histogram.size(); j++)
		{
			if (histogram[j].valueInBin( (angle * HOD_NUM_BINS) / 360 ))
			{
				histogram[j].addToBin(length);
				// totalInBins += length;
				break;
			}
		}
	}
		// cout << "Total in bins: " << totalInBins << endl;
		// cout << "Testing histogram for first file: \n";
		// for (int j = 0; j < histogram.size(); j++)
		// 	histogram[j].normalize(totalInBins);
		// for (int i = 0; i < histogram.size(); i++)
		// {
		// 	cout << histogram[i] << endl;
		// }
	// }

	return histogram;
}

vector<double> create_final_descriptor(vector<vector<Point2d> > data)
{
	vector<vector<double> > allJoint3dTrajectories;

	for (int i = 0; i < MAX_JOINTS; i++)
	{
		allJoint3dTrajectories.push_back(calc_3d_trajectory(i, data));
	}
	vector<double> temp;
	return temp;
}

vector<double> calc_3d_trajectory(int joint, vector<vector<Point2d> > data)
{
	vector<Bin> xyTrajectory;
	// vector<Bin> yzTrajectory;
	// vector<Bin> xzTrajectory;

	vector<double> temp;

	xyTrajectory = temporal_pyramid(joint, data);
	// xyTrajectory = compute_histogram(joint, 1, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data);
	// yzTrajectory = compute_histogram(0, 1, joint, YZ_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data);
	// xzTrajectory = compute_histogram(0, 1, joint, XZ_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data);


	return temp;
}

vector<Bin> temporal_pyramid(int joint, vector<vector<Point2d> > data)
{
	vector<vector<Bin> > temporalVector;

	// Only having 3 levels
	// Main Level
	// temporalVector.push_back(compute_histogram(joint, 1, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// Next Level data.size() / 2
	// temporalVector.push_back(compute_histogram(joint, 2, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// temporalVector.push_back(compute_histogram(joint + MAX_JOINTS, 2, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// Next Level data.size() / 4
	// temporalVector.push_back(compute_histogram(0, 3, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// temporalVector.push_back(compute_histogram(1, 3, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// temporalVector.push_back(compute_histogram(2, 3, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// temporalVector.push_back(compute_histogram(3, 3, joint, XY_TRAJECT, HOD_LOWER_BOUND, HOD_UPPER_BOUND, HOD_NUM_BINS, data));
	// cout << "temporal_pyramid" << endl;
	if (testCounter == 1)
	{
		cout << "Testing temporalVector main: " << endl;
		for (int i = 0; i < temporalVector[0].size(); i++)
		{
			cout << temporalVector[0][i] << endl;
		}
		testCounter++;
	}

	// for (int i = 1; i < temporalVector.size(); i++)
	// {
	// 	for (int j = 0; j < HOD_NUM_BINS; j++)
	// 	{
	// 		temporalVector[0][j].addToBin(temporalVector[i][j].getBinSize());
	// 	}
	// }
	// if (testCounter == 2)
	// {

	// 	double totalInBins = 0.0;
	// 	for (int i = 0; i < allLevels.size(); i++)
	// 	{
	// 		for (int j = 0; j < HOD_NUM_BINS; j++)
	// 		{
	// 			histogram[j].addToBin(allLevels[i][j].getBinSize());
	// 			totalInBins += allLevels[i][j].getBinSize();
	// 		}
	// 	}

	// 	cout << "Total in bins in temporal_pyramid: " << totalInBins << endl;
	// 	cout << "Testing histogram for first file: \n";
	// 	for (int j = 0; j < histogram.size(); j++)
	// 		histogram[j].normalize(totalInBins);
	// 	for (int i = 0; i < histogram.size(); i++)
	// 	{
	// 		cout << histogram[i] << endl;
	// 	}
	// 	testCounter++;	
	// }

}

// Combine all normalized distance and angles from the histogram into a feature vector
void combine_feature_vector(vector<double>& feature, vector<Bin> input)
{
	for (int i = 0; i < input.size(); i++)
		feature.push_back(input[i].getFreq());
}

// writes the data in LIBSVM format using the activity category (i.e. a08 -> id = 08)
void write_LIBSVM(string id, vector<double> features, ofstream& output)
{
	output << id;
	for (int i = 0; i < features.size(); i++)
	{
		output << " " << i+1 << ":" << features[i]; 
	}
	output << "\n";
}

// Transform the data from RAD to LIBSVM format
void transform_to_HOD(const char *path, const string output) 
{
	DIR *direct;
	// Open directory, should be /dataset/t[rain|est]
	if (NULL == (direct = opendir(path))) 
	{
		cout << "Cannot open " << (output == "hod" ? "train" : "test") << " directory: " << path << endl;
		exit(1);
	}
	// Need to traverse each file in the directory
	struct dirent *file;
	// Declare vars to read from file
	int frame, joint;
	double xpos, ypos, zpos; 
	
	// Open output file
	// ofstream outputFile(output);

	// if (!outputFile)
	// {
	// 	cerr << "Error opening file: " << output << endl;
	// 	exit(1);
	// }
	
	// Read through each file
	// For each instance in Train and Test
	while ( (file = readdir(direct)) ) 
	{
		// Skip current and previous directory file names
		if ( strncmp(file->d_name, ".", 1) == 0 || strncmp(file->d_name, "..", 2) == 0) 
			continue;

		// Start of file
		ifstream inputFile(string(path) + string(file->d_name));
		// Check file for read error
		if (!inputFile) 
		{
			cerr << "Error reading file: " << file->d_name << endl;
			exit(1);
		}
		// for frame t = 1, ... ,T
		// Set up needed variables
		vector<Point2d> joint2dTrajectory;				// Holds the three 2d trajectories (xy, yz, xz) for a joint
		vector<vector<Point2d> > allJoints;				// Holds joints 1,...,20 for each frame
		// vector<vector<Point2d> frames;				// Holds all frames of joints
		int invalidFrame = -1;							// Identifies which frame is invalide (NaN data)

		// Read from the file
		while(inputFile >> frame >> joint >> xpos >> ypos >> zpos) 
		{
			// Discard any values that contain NaN's
			if ((isnan(xpos) == 1) || (isnan(ypos) == 1) || (isnan(zpos) == 1)) 
				invalidFrame = frame;

			if (invalidFrame == frame && joint == MAX_JOINTS) 
			{
				invalidFrame = -1;
				joint = -1;
			} 
			else if (invalidFrame == frame)
				continue;

			// Each joint 2d trajectory
			joint2dTrajectory.push_back(Point2d(xpos,ypos));		// xy trajectory
			joint2dTrajectory.push_back(Point2d(ypos,zpos));		// yz trajectory
			joint2dTrajectory.push_back(Point2d(xpos,zpos));		// xz trajectory

			// Add trajectories to a vector of all joints for a frame
			allJoints.push_back(joint2dTrajectory);

			// Clear the jointTrajectory for next joint
			joint2dTrajectory.clear();

			// Done reading in all joints of a single frame
			// if (joint == MAX_JOINTS) 
			// {
				// frames.push_back(jointVector);
				// jointVector.clear();
			// }
		}
		// file done
		// create historgrams

		if (testCounter == 0)
		{
			cout << "alljoints size: " << allJoints.size() << endl;

			testCounter++;
		}

		vector<double> featureVector;

		featureVector = create_final_descriptor(allJoints);

		// // Add all histograms into feature vector
		// for (int i = 0; i < NUM_DIMENSIONS; i++)
		// 	combine_feature_vector(featureVector, jointDiffHists[i]);

		// write_LIBSVM(string(file->d_name).substr(1,2), featureVector, outputFile);
		// featureVector.clear();
		inputFile.close();
	} // End looping through directory
	// outputFile.close();
	// cout << "Min value: " << minValue << ", next min: " << nextMin << endl; 
	// cout << "Max value: " << maxValue << ", next max: " << nextMax << endl;
	closedir(direct);
} // End transform_to_HOD
