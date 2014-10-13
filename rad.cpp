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
const int RAD_DISTANCE_ANGLE_NUM = 5;		// Compute 5 distances/angles for star skeleton
const int CENTROID = 0;						// Center of hip
const int HEAD = 3;

/* User definable */
double distance_lower_bound = 0.45;
double distance_upper_bound = 1.95;
int distance_num_bins = 10;
double angle_lower_bound = 0.1;
double angle_upper_bound = 2.15;
int angle_num_bins = 40;
int leftArmJoint = 10;				// Joint of star skeleton, can be either 10 for wrist or 11 for hand (default)
int rightArmJoint = 6;				// Joint of star skeleton, can be either 6 for wrist or 7 for hand (default)
int leftLegJoint = 18;				// Joint of star skeleton, can be either 18 for ankle or 19 for foot (default)
int rightLegJoint = 14;				// Joint of star skeleton, can be either 14 for ankle or 15 for foot (default)

vector<double> calc_angles(const vector<Point3d> joints);
vector<double> calc_distances(const vector<Point3d> joints);
void checkArgs(int argc, char *argv[]);
void combine_feature_vector(vector<double>& feature, vector<Bin> input);
vector<Bin> compute_histogram(int histogram_i, vector<vector<double> > data, int type);
void exit_with_error(string option = "", string value = "");
void exit_with_help();
bool is_dir(const char* path);
bool is_real_number(const string &str);
void transform_to_RAD(const char *path, const string output);
void write_LIBSVM(string id, vector<double> features, ofstream& output);

int main(int argc, char *argv[]) 
{
	checkArgs(argc, argv);
	transform_to_RAD(argv[argc-2], "rad");
	transform_to_RAD(argv[argc-1], "rad.t");
	return 0;
}

vector<double> calc_angles(const vector<Point3d> joints)
{
	vector<double> temp;
	temp.push_back(joints[CENTROID].angle(joints[HEAD], joints[leftArmJoint]));				// theta1
	temp.push_back(joints[CENTROID].angle(joints[HEAD], joints[rightArmJoint]));			// theta2
	temp.push_back(joints[CENTROID].angle(joints[rightArmJoint], joints[rightLegJoint]));	// theta3
	temp.push_back(joints[CENTROID].angle(joints[leftArmJoint], joints[leftLegJoint]));		// theta4
	temp.push_back(joints[CENTROID].angle(joints[rightLegJoint], joints[leftLegJoint]));	// theta5
	return temp;
}

vector<double> calc_distances(const vector<Point3d> joints)
{
	vector<double> temp;
	temp.push_back(joints[CENTROID].distance(joints[HEAD]));				// d1
	temp.push_back(joints[CENTROID].distance(joints[leftArmJoint]));		// d2
	temp.push_back(joints[CENTROID].distance(joints[leftLegJoint]));		// d3
	temp.push_back(joints[CENTROID].distance(joints[rightArmJoint]));		// d4
	temp.push_back(joints[CENTROID].distance(joints[rightLegJoint]));		// d5
	return temp;
}

void checkArgs(int argc, char *argv[])
{
	if (argc < 3)
	{ 
		exit_with_help();
	}

	for (int i = 1; i < argc - 2; i+=2)		// check for -options flags
	{
		if (string(argv[i]).compare("-dl") == 0)			// Distance lower bound
		{
			if (is_real_number(string(argv[i+1])))
				distance_lower_bound = atof(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-du") == 0)		// Distance upper bound
		{
			if (is_real_number(string(argv[i+1])))
				distance_upper_bound = atof(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-db") == 0)		// Distance bin number
		{
			if (isdigit(*argv[i+1]))
				distance_num_bins = atoi(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-al") == 0)		// Angle lower bound
		{
			if (is_real_number(string(argv[i+1])))
				angle_lower_bound = atof(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-au") == 0)		// Angle upper bound
		{
			if (is_real_number(string(argv[i+1])))
				angle_upper_bound = atof(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-ab") == 0)		// Angle bin number
		{
			if (isdigit(*argv[i+1]))
				angle_num_bins = atoi(argv[i+1]);
			else
				exit_with_error(argv[i], argv[i+1]);
		}
		else if (string(argv[i]).compare("-la") == 0)		// Left arm joint
		{
			if (isdigit(*argv[i+1]) && ( (atoi(argv[i+1]) == 10) || (atoi(argv[i+1]) == 11) ))
				leftArmJoint = atoi(argv[i+1]);
			else
			{
				exit_with_error(argv[i], argv[i+1]);
			}
		}
		else if (string(argv[i]).compare("-ll") == 0)		// Left leg joint
		{
			if (isdigit(*argv[i+1]) && ( (atoi(argv[i+1]) == 18) || (atoi(argv[i+1]) == 19)) )
				leftLegJoint = atoi(argv[i+1]);
			else
			{
				exit_with_error(argv[i], argv[i+1]);
			}
		}
		else if (string(argv[i]).compare("-ra") == 0)		// Right arm joint
		{
			if (isdigit(*argv[i+1]) && ( (atoi(argv[i+1]) == 6) || (atoi(argv[i+1]) == 7)) )
				rightArmJoint = atoi(argv[i+1]);
			else
			{
				exit_with_error(argv[i], argv[i+1]);
			}
		}
		else if (string(argv[i]).compare("-rl") == 0)		// Right leg joint
		{
			if (isdigit(*argv[i+1]) && ( (atoi(argv[i+1]) == 14) || (atoi(argv[i+1]) == 15)) )
				rightLegJoint = atoi(argv[i+1]);
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

// Combine all normalized distance and angles from the histogram into a feature vector
void combine_feature_vector(vector<double>& feature, vector<Bin> input)
{
	for (int i = 0; i < input.size(); i++)
		feature.push_back(input[i].getFreq());
}

vector<Bin> compute_histogram(int histogram_i, double lower, double upper, int numBins, vector<vector<double> > data)
{
	double interval = (upper - lower) / numBins;
	vector<Bin> histogram;

	// Initialize histogram of numBins
	for (int i = 0; i < numBins; i++)
	{
		histogram.push_back(Bin(lower, interval));
		lower += interval;
	}

	// Go through each data value and put it in a bin
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < histogram.size(); j++)
		{
			if (histogram[j].valueInBin(data[i][histogram_i]))
			{
				histogram[j].addToBin();
				break;
			}

		}
	}
	// Normalize the histogram bins by the number of valid frames
	for (int j = 0; j < histogram.size(); j++)
		histogram[j].normalize(data.size());

	return histogram;
}

void exit_with_help() 
{
	cout << "\nUsage: star [options] <train_dir> <test_dir> \n";
	cout << "Options:\n";
	cout << "\t-dl <#>: distance: histogram lower bound (default " << distance_lower_bound << ")\n";
	cout << "\t-du <#>: distance: histogram upper bound (default " << distance_upper_bound << ")\n";
	cout << "\t-db <#>: distance: number of histogram bins (default " << distance_num_bins << ")\n";
	cout << "\t-al <#>: angle: histogram lower bound (default " << angle_lower_bound << ")\n";
	cout << "\t-au <#>: angle: histogram upper bound (default " << angle_upper_bound << ")\n";
	cout << "\t-ab <#>: angle: number of histogram bins (default " << angle_num_bins << ")\n";
	cout << "\t-la <#>: left arm joint number, either 10 or 11 (default " << leftArmJoint << ")\n";
	cout << "\t-ll <#>: left leg joint number, either 18 or 19 (default " << leftLegJoint << ")\n";
	cout << "\t-ra <#>: right arm joint number, either 6 or 7 (default " << rightArmJoint << ")\n";
	cout << "\t-rl <#>: left arm joint number, either 14 or 15 (default " << rightLegJoint << ")\n";
	cout << "***See skeleton joint names and indicies from Kinect SDK for more information about joint numbers\n";
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

bool is_real_number(const string &str)
{
	return str.find_first_not_of("0123456789.-") == string::npos;
}

// Transform the data from RAD to LIBSVM format
void transform_to_RAD(const char *path, const string output) 
{
	DIR *direct;
	// Open directory, should be /dataset/t[rain|est]
	if (NULL == (direct = opendir(path))) 
	{
		cout << "Cannot open " << (output == "rad" ? "train" : "test") << " directory: " << path << endl;
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
		vector<Point3d> jointVector;				// Holds joints 1,...,20 for each frame
		vector<vector<double> > distances;		// Holds all frame d1,...,d5 distances calculated
		vector<vector<double> > angles;			// Holds all frame theta1,...,theta5 angles calculated
		int invalidFrame = -1;					// Identifies which frame is invalide (NaN data)

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
				// Calculate distances d1,...,d5
				distances.push_back(calc_distances(jointVector));
				// Calculate angles theta1,...,theta5
				angles.push_back(calc_angles(jointVector));

				// Reset joint vector for next frame
				jointVector.clear();
			}
		}
		// file done, create histograms
		vector<vector<Bin> > distanceHists;						// Contains d1,..,d5 histograms
		vector<vector<Bin> > angleHists;						// Constains theta1,..,theta5 histograms

		// Compute Histograms and place in histogram vector
		for (int i = 0; i < RAD_DISTANCE_ANGLE_NUM; i++)
		{
			distanceHists.push_back(compute_histogram(i, distance_lower_bound, distance_upper_bound, distance_num_bins, distances));
			angleHists.push_back(compute_histogram(i, angle_lower_bound, angle_upper_bound, angle_num_bins, angles));
		}

		vector<double> featureVector;
		// Add all normalized values from the distance and angle histograms
		for (int i = 0; i < RAD_DISTANCE_ANGLE_NUM; i++)
			combine_feature_vector(featureVector, distanceHists[i]);
		for (int i = 0; i < RAD_DISTANCE_ANGLE_NUM; i++)
			combine_feature_vector(featureVector, angleHists[i]);

		write_LIBSVM(string(file->d_name).substr(1,2), featureVector, outputFile);

		inputFile.close();
	} // End looping through directory
	outputFile.close();
	closedir(direct);
} // End transform_to_RAD

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

