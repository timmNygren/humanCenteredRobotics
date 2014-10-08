#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <cmath>
#include "point3d.h"
#include "point2d.h"
#include "bin.h"

using namespace std;

const int MAX_JOINTS = 20;
// const int NUM_DIMENSIONS = 3;
// const int TOTAL_NUM_JOINT_DIFFERENCES = 19;
// const double HOD_LOWER_BOUND = 0;
// const double HOD_UPPER_BOUND = 360;
// const int HOD_NUM_BINS = 8;
// const int XY_TRAJECT = 0;
// const int YZ_TRAJECT = 1;
// const int XZ_TRAJECT = 2;
const int X_DIM = 0;
const int Y_DIM = 1;
const int Z_DIM = 2;

// static int testCounter = 0;
// static double minValue = 999999.0;
// static double nextMin;
// static double maxValue = -10000000.0;
// static double nextMax;

void exit_with_help();
vector<Bin> compute_histogram(int jointStart, int inc, int trajectory, double lower, double upper, int numBins, vector<vector<Point2d> > data);
vector<double> create_3D_trajectory(vector<double> xyTraject, vector<double> yzTraject, vector<double> xzTraject);
vector<Bin> temporal_pyramid(vector<double> trajectory2D);
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

	// double length;
	// double angle;
	
	return histogram;
}

vector<double> create_3D_trajectory(vector<double> xyTraject, vector<double> yzTraject, vector<double> xzTraject)
{
	vector<Bin> xyHOD;
	// vector<Bin> yzHOD;
	// vector<Bin> xzHOD;

	xyHOD = temporal_pyramid(xyTraject);
	// yzHOD = temporal_pyramid(yzTraject);
	// xzHOD = temporal_pyramid(xzTraject);

	vector<double> trajectory3D;
	return trajectory3D;
}

vector<Bin> temporal_pyramid(vector<double> trajectory2D)
{

	vector<Bin> summedBin;
	return summedBin;
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

	vector<Point3d> allPoints;
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

		// Set up needed variables
		// vector<Point2d> joint2dTrajectory;				// Holds the three 2d trajectories (xy, yz, xz) for a joint

		int invalidFrame = -1;							// Identifies which frame is invalide (NaN data)
		int totalFrames = 0;
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

			allPoints.push_back(Point3d(xpos, ypos, zpos));
			// Done reading in all joints of a single frame
			// if (joint == MAX_JOINTS) 
			// {
				// frames.push_back(jointVector);
				// jointVector.clear();
			// }
			totalFrames = frame;
		}
		// file done
		cout << "Total frames: " << totalFrames << endl;
		cout << "Total points = frames * 20: " << allPoints.size() << endl;

		vector<vector<Point2d> > allJointsXY(MAX_JOINTS);
		vector<vector<Point2d> > allJointsYZ(MAX_JOINTS);
		vector<vector<Point2d> > allJointsXZ(MAX_JOINTS);
		// create historgrams
		for (int i = 0; i < MAX_JOINTS; i++)
		{
			// int frame = 1;
			// cout << "Joint: " << i << " with points: " << endl; 
			vector<Point2d> xypoints;
			vector<Point2d> yzpoints;
			vector<Point2d> xzpoints;
			for (int j = i; j < allPoints.size(); j+=MAX_JOINTS)
			{
				// cout << "\tFrame: " << frame << " " << allPoints[j] << endl;
				xypoints.push_back(Point2d(allPoints[j].getDimension(X_DIM), allPoints[j].getDimension(Y_DIM)));
				yzpoints.push_back(Point2d(allPoints[j].getDimension(Y_DIM), allPoints[j].getDimension(Z_DIM)));
				xzpoints.push_back(Point2d(allPoints[j].getDimension(X_DIM), allPoints[j].getDimension(Z_DIM)));
				frame++;
			}
			allJointsXY[i] = xypoints;
			allJointsYZ[i] = yzpoints;
			allJointsXZ[i] = xzpoints;
			xypoints.clear();
		}

		// for (int i = 0; i < allJointsXY.size(); i++)
		// {
		// 	cout << "Point: " << i << " with xy values: " << endl;
		// 	for(int j = 0; j < totalFrames; j++)
		// 	{
		// 		cout << "\t" << allJointsXY[i][j] << endl;
		// 	}
		// }
		vector<vector<double> > allPoint3DTrajectories(MAX_JOINTS);
		// vector<double> featureVector;

		// 3D trajectory is the concatination of {HOD_XY_Point_i, HOD_YZ_Point_i, HOD_XZ_Point_i}
		// for (int i = 0; i < MAX_JOINTS; i++)
			// allPoint3DTrajectories.push_back( create_3D_trajectory(allJointsXY[i], allJointsYZ[i], allJointsXZ[i]) );

		// // Add all histograms into feature vector
		// for (int i = 0; i < NUM_DIMENSIONS; i++)
		// 	combine_feature_vector(featureVector, jointDiffHists[i]);

		// write_LIBSVM(string(file->d_name).substr(1,2), featureVector, outputFile);
		// featureVector.clear();
		allPoints.clear();
		inputFile.close();
	} // End looping through directory
	
	// outputFile.close();
	// cout << "Min value: " << minValue << ", next min: " << nextMin << endl; 
	// cout << "Max value: " << maxValue << ", next max: " << nextMax << endl;
	closedir(direct);
} // End transform_to_HOD
