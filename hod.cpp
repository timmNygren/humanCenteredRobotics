#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <cmath>
#include <sys/stat.h>
#include "point3d.h"
#include "point2d.h"
#include "bin.h"

using namespace std;

const int MAX_JOINTS = 20;
const double HOD_LOWER_BOUND = 0;
const double HOD_UPPER_BOUND = 360;
const int X_DIM = 0;
const int Y_DIM = 1;
const int Z_DIM = 2;

/* User defined vars */
int hod_num_bins = 10;

void checkArgs(int argc, char *argv[]);
void combine_feature_vector(vector<double>& feature, vector<double>input);
void combine_2D_to_3D_trajectory(vector<double>& feature, vector<Bin> input);
vector<Bin> compute_histogram(int jointStart, int inc, double lower, double upper, int numBins, vector<Point2d> data);
vector<double> create_3D_trajectory(vector<Point2d> xyTraject, vector<Point2d> yzTraject, vector<Point2d> xzTraject);
void exit_with_error(string option = "", string value = "");
void exit_with_help();
bool is_dir(const char* path);
void sumBins(vector<Bin>& sum, vector<Bin> input);
vector<Bin> temporal_pyramid(vector<Point2d> trajectory2D);
void transform_to_HOD(const char *path, const string output);
void write_LIBSVM(string id, vector<double> features, ofstream& output);

int main(int argc, char *argv[]) 
{
	checkArgs(argc, argv);
	transform_to_HOD(argv[argc-2], "hod");
	transform_to_HOD(argv[argc-1], "hod.t");
	return 0;
}

void checkArgs(int argc, char *argv[])
{
	if (argc < 3)
	{ 
		exit_with_help();
	}

	for (int i = 1; i < argc - 2; i+=2)		// check for -options flags
	{
		if (string(argv[i]).compare("-b") == 0)
		{
			if (isdigit(*argv[i+1]))
				hod_num_bins = atoi(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else
			exit_with_error(string(argv[i]), string(argv[i+1]));
	}

	// Check last two params to make sure they are directories
	if (is_dir(argv[argc-2]) != 1)
	{
		exit_with_error("dir", argv[argc-2]);
	} 
	cout << "";							// Need for next conditional to work...?
	if (is_dir(argv[argc-1]) != 1)
	{
		exit_with_error("dir", argv[argc-1]);
	}
}

// Combine all normalized distance and angles from the histogram into a feature vector
void combine_feature_vector(vector<double>& feature, vector<double> input)
{
	for (int i = 0; i < input.size(); i++)
		feature.push_back(input[i]);
}

// Combine all normalized distance and angles from the histogram into a feature vector
void combine_2D_to_3D_trajectory(vector<double>& feature, vector<Bin> input)
{
	for (int i = 0; i < input.size(); i++)
		feature.push_back(input[i].getFreq());
}

vector<Bin> compute_histogram(int jointStart, int inc, double lower, double upper, int numBins, vector<Point2d> data)
{
	// Set up histogram params
	upper = upper * numBins / 360;
	lower = lower * numBins / 360;
	double interval = (upper - lower) / numBins;

	// // Initialize histogram of numBins
	vector<Bin> histogram;
	for (int i = 0; i < numBins; i++)
	{
		histogram.push_back(Bin(lower, interval));
		lower += interval;
	}

	double length;
	double angle;

	for (int i = jointStart; i < data.size() - inc; i += inc)
	{
		length = data[i].distance(data[i+1]);
		angle = data[i].angle(data[i+1]);
		for (int j = 0; j < histogram.size(); j++)
		{
			if (histogram[j].valueInBin( (angle * numBins) / 360 ))
			{
				histogram[j].addToBin(length);
				break;
			}
		}
	}
	
	return histogram;
}

vector<double> create_3D_trajectory(vector<Point2d> xyTraject, vector<Point2d> yzTraject, vector<Point2d> xzTraject)
{
	vector<vector<Bin> > HOD2d;
	HOD2d.push_back(temporal_pyramid(xyTraject));
	HOD2d.push_back(temporal_pyramid(yzTraject));
	HOD2d.push_back(temporal_pyramid(xzTraject));

	vector<double> trajectory3D;
	for (int i = 0; i < HOD2d.size(); i++)
		combine_2D_to_3D_trajectory(trajectory3D, HOD2d[i]);

	return trajectory3D;
}

void exit_with_error(string option, string value)
{
	string valueOrPath = (option.compare("dir") == 0) ? "path" : "value"; 
	cout << "\n***Error handling parameter: " << option << " with " + valueOrPath + ": " << value << endl;
	if (option.compare("-r") == 0)
		cout << "Reference Joint must be be a digit [0-19]\n";
	exit_with_help();
}

void exit_with_help() 
{
	cout << "\nUsage: hod train_dir test_dir \n";
	cout << "Options:\n";
	cout << "\t-b <#>: number of bins used in HOD (default " << hod_num_bins << ")\n";
	cout << "Make sure train_dir is the first, otherwise\n";
	cout << "The output hod file will contain incorrect data\n\n";
	exit(1);
}

bool is_dir(const char* path)
{
	struct stat buffer;
	stat(path, &buffer);
	return S_ISDIR(buffer.st_mode);
}

void sumBins(vector<Bin>& sum, vector<Bin> input)
{
	// If sum is not initialized
	if (sum.size() == 0)
	{
		double upper = HOD_UPPER_BOUND * hod_num_bins / 360;
		double lower = HOD_LOWER_BOUND * hod_num_bins / 360;
		double interval = (upper - lower) / hod_num_bins;

		// // Initialize histogram of numBins
		for (int i = 0; i < hod_num_bins; i++)
		{
			sum.push_back(Bin(lower, interval));
			lower += interval;
		}
	}
	// add input bins to corresponding sum bins
	for(int i = 0; i < sum.size(); i++)
	{
		sum[i].addToBin(input[i].getBinSize());
	}
}

vector<Bin> temporal_pyramid(vector<Point2d> trajectory2D)
{
	vector<Bin> summedBin;

	// Perform temporal pyramid
	sumBins(summedBin, compute_histogram(0, 1, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // First Level
	sumBins(summedBin, compute_histogram(1, 2, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // Second Level L
	sumBins(summedBin, compute_histogram(0, 2, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // Second Level R
	sumBins(summedBin, compute_histogram(0, 4, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // Third LeveL LL
	sumBins(summedBin, compute_histogram(2, 4, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // Third Level LR
	sumBins(summedBin, compute_histogram(1, 4, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // Third Level RL
	sumBins(summedBin, compute_histogram(3, 4, HOD_LOWER_BOUND, HOD_UPPER_BOUND, hod_num_bins, trajectory2D)); // Third Level RR

	// Get total distance by summing all bin values
	double totalDistanceSize = 0;
	for (int i = 0; i < summedBin.size(); i++)
		totalDistanceSize += summedBin[i].getBinSize();
	// Normalize by total bin values
	for (int i = 0; i < summedBin.size(); i++)
		summedBin[i].normalize(totalDistanceSize);

	return summedBin;
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
	ofstream outputFile(output);

	if (!outputFile)
	{
		cerr << "Error opening file: " << output << endl;
		exit(1);
	}
	// Hold all points as 3D points
	vector<Point3d> allPoints;
	// Read through each file
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

			allPoints.push_back(Point3d(xpos, ypos, zpos));
		}
		// file done
		// Create vectors to hold all joints of specified 2D planes
		vector<vector<Point2d> > allJointsXY(MAX_JOINTS);
		vector<vector<Point2d> > allJointsYZ(MAX_JOINTS);
		vector<vector<Point2d> > allJointsXZ(MAX_JOINTS);

		vector<Point2d> xypoints;
		vector<Point2d> yzpoints;
		vector<Point2d> xzpoints;
		// create historgrams
		for (int i = 0; i < MAX_JOINTS; i++)
		{
			for (int j = i; j < allPoints.size(); j+=MAX_JOINTS)
			{
				// Add all xy, yz, xz points for each point 0,...,19
				xypoints.push_back(Point2d(allPoints[j].getDimension(X_DIM), allPoints[j].getDimension(Y_DIM)));
				yzpoints.push_back(Point2d(allPoints[j].getDimension(Y_DIM), allPoints[j].getDimension(Z_DIM)));
				xzpoints.push_back(Point2d(allPoints[j].getDimension(X_DIM), allPoints[j].getDimension(Z_DIM)));
			}
			// allJoints*[] will contains all xy, yz and xz points for point i
			// e.g., If i == 0, allJointsXY[0], allJointsYZ[0], and allJointsXZ[0] will contain all frame xy,yx,xz points
			// for the center of the hip
			allJointsXY[i] = xypoints;
			allJointsYZ[i] = yzpoints;
			allJointsXZ[i] = xzpoints;
			xypoints.clear();
			yzpoints.clear();
			xzpoints.clear();
		}

		// 3D trajectory is the concatination of {HOD_XY_Point_i, HOD_YZ_Point_i, HOD_XZ_Point_i}
		vector<vector<double> > allPoint3DTrajectories(MAX_JOINTS);
		for (int i = 0; i < MAX_JOINTS; i++)
			allPoint3DTrajectories.push_back( create_3D_trajectory(allJointsXY[i], allJointsYZ[i], allJointsXZ[i]) );

		// // Add all histograms into feature vector
		vector<double> featureVector;
		for (int i = 0; i < allPoint3DTrajectories.size(); i++)
			combine_feature_vector(featureVector, allPoint3DTrajectories[i]);

		write_LIBSVM(string(file->d_name).substr(1,2), featureVector, outputFile);

		allPoints.clear();
		inputFile.close();
	} // End looping through directory
	
	outputFile.close();
	closedir(direct);
} // End transform_to_HOD


// writes the data in LIBSVM format using the activity category (i.e. a08 -> id = 08)
void write_LIBSVM(string id, vector<double> features, ofstream& output)
{
	output << stoi(id);
	for (int i = 0; i < features.size(); i++)
	{
		output << " " << i+1 << ":" << features[i]; 
	}
	output << "\n";
}
