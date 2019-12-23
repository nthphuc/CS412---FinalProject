#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <iostream>
#include "acedd.h"
#include "include/lshbox.h"

using namespace ACEDD_DESC;
using namespace std;
using namespace cv;

float averageDistance(float a[100]) {
	float sub;
	for (int i = 0; i < 100-1; i++) {
		for (int j = i + 1; j < 100; j++) {
			if (a[i] > a[j]) {
				sub = a[j];
				a[j] = a[i];
				a[i] = sub;
			}
		}
	}
	sub = 0;
	for (int i = 0; i < 15; i++) {
		sub += a[i];
	}
	return sub /= 15;
}

int main(int argc, const char* argv[]) {
	CEDD cedd;
	Mat img_a, img_b, desc_a, desc_b;
	string trainPath = "data/train/";
	string testPath = "data/test/";
	string trainedFilePath = "data/";
	string path = "";
	string path2 = "";
	string folderName[27] = { "airplane","alligator", "bat", "bicycle", "blimp", "boat", "bus", "camel", "car", "carriage", "cat", "cow", "crow", "dog", "dolphin", "eagle", "elephant", "elk", "hovercraft", "jetski", "lizard", "monkey", "motorcycle", "penguin", "semi", "ship", "snowmobile" };
	float sum;
	float dist;

	/*ofstream outFile;
	for (int i = 0; i < 27; i++) {
		sum = 0;
		outFile.open(trainPath+folderName[i]+".txt");
		for (int j = 1; j <= 100; j++) {
			path = trainPath + folderName[i] + "/" + to_string(j) + ".jpg";
			img_b = imread(path);
			cedd.Apply(img_b, desc_b);
			MatConstIterator_<unsigned char> descOneItr = desc_b.begin<unsigned char>();
			for (; descOneItr != desc_b.end<unsigned char>(); ++descOneItr) {
				outFile << (float)*descOneItr << " ";
			}
			outFile << endl;
		}
		outFile.close();
	}*/

	ifstream inFile;
	float inputVal;
	int count;
	ofstream resultFile;
	resultFile.open("result2.txt");
	for (int ii = 1; ii <= 270; ii++) {
		path2 = testPath + to_string(ii) + ".jpg";
		img_a = imread(path2);
		cedd.Apply(img_a, desc_a);
		MatConstIterator_<unsigned char> descItr;
		Mat descSub = desc_a.clone();
		sum = 0;
		float sub_array[100];
		cout << "Image " << ii << ": " << endl;
		for (int i = 0; i < 27; i++) {
			count = 0;
			sum = 0;
			int sub_count = 0;
			inFile.open(trainedFilePath + folderName[i] + ".txt");
			for (int j = 1; j <= 100; j++) {
				for (int k = 0; k < 144; k++) {
					inFile >> inputVal;
					descSub.at<unsigned char>(k) = inputVal;
				}
				dist = cedd.computeDistance(desc_a, descSub);//dis<=45
				sub_array[j - 1] = dist;
				if (dist <= 30) {
					sum += dist;
					sub_count++;
				}
				if (dist <= 40) count++;
			}
			inFile.close();

			//Method 1
			if (sub_count == 0) {
				sum = 100;
			}
			else {
				sum /= sub_count;
			}
			if (sum <= 26) {
				cout << folderName[i] << " ";
				resultFile << folderName[i] << " ";
			} 

			//Method 2
			//float subRes = averageDistance(sub_array);
			//if (subRes <= 40) cout << folderName[i] << " ";
		}
		resultFile << endl;
		cout << endl;
	}
	resultFile.close();
	return 0;
}