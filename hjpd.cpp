#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <cmath>
#include <ctype.h>
#include <sys/stat.h>
#include "point3d.h"
#include "bin.h"

using namespace std;

const int MAX_JOINTS = 20;
const int NUM_DIMENSIONS = 3;					// deltaX, deltaY, deltaZ
const int TOTAL_NUM_JOINT_DIFFERENCES = 19;
const int DELTA_X = 0;
const int DELTA_Y = 1;
const int DELTA_Z = 2;

/* User definable */
double joint_pos_diff_lower_bound = -1.10;		// Default
double joint_pos_diff_upper_bound = 1.30;		// Default
int joint_pos_diff_num_bins = 20;				// Default
int referenceJoint = 0;							// Default Center of hip
// static int testCounter = 0;
// static double minValue = 999999.0;
// static double nextMin;
// static double maxValue = -10000000.0;
// static double nextMax;

void checkArgs(int argc, char *argv[]);
bool is_real_number(const string &str);
void exit_with_help();
void exit_with_error(string option = "", string value = "");
bool is_dir(const char* path);
vector<Point3d> calc_joint_differences(const vector<Point3d> joints);
vector<Bin> compute_histogram(int histogram_dimension, double lower, double upper, int numBins, vector<vector<Point3d> > data);
void combine_feature_vector(vector<double>& feature, vector<Bin> input);
void write_LIBSVM(string id, vector<double> features, ofstream& output);
void transform_to_HJPD(const char *path, const string output);

int main(int argc, char *argv[]) 
{
	checkArgs(argc, argv);
	transform_to_HJPD(argv[argc-2], "hjpd");
	transform_to_HJPD(argv[argc-1], "hjpd.t");
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
		if (string(argv[i]).compare("-l") == 0)
		{
			if (is_real_number(string(argv[i+1])))
				joint_pos_diff_lower_bound = atof(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-u") == 0)
		{
			if (is_real_number(string(argv[i+1])))
				joint_pos_diff_upper_bound = atof(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-b") == 0)
		{
			if (isdigit(*argv[i+1]))
				joint_pos_diff_num_bins = atoi(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-r") == 0)
		{
			if (isdigit(*argv[i+1]) && (atoi(argv[i+1]) >= 0) && (atoi(argv[i+1]) < 20))
				referenceJoint = atoi(argv[i+1]);
			else
			{
				exit_with_error(argv[i], argv[i+1]);
			}
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

bool is_real_number(const string &str)
{
	return str.find_first_not_of("0123456789.-") == string::npos;
}

void exit_with_help() 
{
	cout << "\nUsage: joint_displace [options] <train_dir> <test_dir> \n";
	cout << "Options:\n";
	cout << "\t-l <#>: histogram lower bound (default " << joint_pos_diff_lower_bound << ")\n";
	cout << "\t-u <#>: histogram upper bound (default " << joint_pos_diff_upper_bound << ")\n";
	cout << "\t-b <#>: number of histogram bins (default " << joint_pos_diff_num_bins << ")\n";
	cout << "\t-r [0-19]: Reference joint to use (default 0, or center of hip)**See skeleton joint names and indices from Kinect SDK**\n";
	cout << "\nMake sure train_dir is the first, otherwise\n";
	cout << "The output hjpd file will contain incorrect data\n\n";
	exit(1);
}

void exit_with_error(string option, string value)
{
	string valueOrPath = (option.compare("dir") == 0) ? "path" : "value"; 
	cout << "\n***Error handling parameter: " << option << " with " + valueOrPath + ": " << value << endl;
	if (option.compare("-r") == 0)
		cout << "Reference Joint must be be a digit [0-19]\n";
	exit_with_help();
}

bool is_dir(const char* path)
{
	struct stat buffer;
	stat(path, &buffer);
	return S_ISDIR(buffer.st_mode);
}

vector<Point3d> calc_joint_differences(const vector<Point3d> joints)
{
	vector<Point3d> tempDifferences;
	for (int i = 0; i < joints.size(); i++)
		if (i != referenceJoint)
			tempDifferences.push_back(joints[referenceJoint].difference(joints[i]));
	return tempDifferences;
}

vector<Bin> compute_histogram(int histogram_dimension, double lower, double upper, int numBins, vector<vector<Point3d> > data)
{
	double interval = (upper - lower) / numBins;
	vector<Bin> histogram;

	// Initialize histogram of numBins
	for (int i = 0; i < numBins; i++)
	{
		histogram.push_back(Bin(lower, interval));
		lower += interval;
	}

	// cout << "Testing data in compute_histogram: " << endl;
	// for (int i = 0; i < data.size(); i++)
	// {
	// 	for (int j = 0; j < TOTAL_NUM_JOINT_DIFFERENCES; j++)
	// 	{
	// 		if (data[i][j].getDimension(histogram_dimension) < minValue)
	// 		{
	// 			// cout << "min value change to: " << data[i][j].getDimension(histogram_dimension) << endl;
	// 			nextMin = minValue;
	// 			minValue = data[i][j].getDimension(histogram_dimension);
	// 		}
	// 		if (data[i][j].getDimension(histogram_dimension) > maxValue)
	// 		{
	// 			nextMax = maxValue;
	// 			maxValue = data[i][j].getDimension(histogram_dimension);
	// 		}
	// 		// cout << "\t Frame: " << i << ", Joint: " << j << ", dimension: " << histogram_dimension << ",.... value: " << data[i][j].getDimension(histogram_dimension) << endl;
	// 	}
	// }
	for (int i = 0; i < data.size(); i++)						// T frames
	{
		for (int j = 0; j < TOTAL_NUM_JOINT_DIFFERENCES; j++)	// 1,...,19 deltas
		{
			for (int k = 0; k < histogram.size(); k++)
			{
				if (histogram[k].valueInBin(data[i][j].getDimension(histogram_dimension)))
				{ 
					histogram[k].addToBin();
					break;
				}
			}
		}
	}

	for (int j = 0; j < histogram.size(); j++)
		histogram[j].normalize(data.size() * TOTAL_NUM_JOINT_DIFFERENCES);

	return histogram;
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
	output << stoi(id);
	for (int i = 0; i < features.size(); i++)
	{
		output << " " << i+1 << ":" << features[i]; 
	}
	output << "\n";
}

// Transform the data from RAD to LIBSVM format
void transform_to_HJPD(const char *path, const string output) 
{
	DIR *direct;
	// Open directory, should be /dataset/t[rain|est]
	if (NULL == (direct = opendir(path))) 
	{
		cout << "Cannot open " << (output == "hjpd" ? "train" : "test") << " directory: " << path << endl;
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
		vector<Point3d> jointVector;					// Holds joints 1,...,20 for each frame
		vector<vector<Point3d> > jointDifferences;	// Holds all frame jd1,...,jd19 for all joint distance calculations
		int invalidFrame = -1;						// Identifies which frame is invalide (NaN data)

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

			jointVector.push_back(Point3d(xpos, ypos, zpos));

			// Done reading in all joints of a single frame
			if (joint == MAX_JOINTS) 
			{
				// Calculate joint differences
				// if (testCounter == 0)
				// 	cout << "File: " << string(file->d_name) << endl;
				jointDifferences.push_back(calc_joint_differences(jointVector));
				// Reset joint vector for next frame
				jointVector.clear();
			}
		}
		// file done
		// create historgrams
		vector<vector<Bin> > jointDiffHists;
		// if (testCounter == 0)
		// {
		// 	cout << "Size of jointDifferences vector: " << jointDifferences.size() << endl;
		// 	testCounter++;
		// }

		// Compute Histograms and place in histogram vector
		jointDiffHists.push_back(compute_histogram(DELTA_X, joint_pos_diff_lower_bound, joint_pos_diff_upper_bound, joint_pos_diff_num_bins, jointDifferences));
		jointDiffHists.push_back(compute_histogram(DELTA_Y, joint_pos_diff_lower_bound, joint_pos_diff_upper_bound, joint_pos_diff_num_bins, jointDifferences));
		jointDiffHists.push_back(compute_histogram(DELTA_Z, joint_pos_diff_lower_bound, joint_pos_diff_upper_bound, joint_pos_diff_num_bins, jointDifferences));


		vector<double> featureVector;
		// // Add all histograms into feature vector
		for (int i = 0; i < NUM_DIMENSIONS; i++)
			combine_feature_vector(featureVector, jointDiffHists[i]);

		write_LIBSVM(string(file->d_name).substr(1,2), featureVector, outputFile);

		inputFile.close();
	} // End looping through directory
	outputFile.close();
	// cout << "Min value: " << minValue << ", next min: " << nextMin << endl; 
	// cout << "Max value: " << maxValue << ", next max: " << nextMax << endl;
	closedir(direct);
} // End transform_to_HJPD
