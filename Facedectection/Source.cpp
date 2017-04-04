/*
* Facedetection.cpp
*
* Using OpenCV
* Linking opencv to VisualStuido:
*	right click project in the Solution Explorer and click 'Properties', click 'C/C++'.
*	Let's add the OpenCV resource by pasting the include directory into 'Additional Include Libraries', for example: C:\Users\Magnus\Documents\opencv\build\include, and click 'Apply'
*	To link the libraries: go to 'Linker' -> 'General' and pase C:\Users\Magnus\Documents\opencv\build\x64\vc14\lib in 'Additional Library Directories'. 

* Go to 'Linker' -> 'Input' ->'Additional Dependencies' -> paste 'opencv_world320d.lib' (remember - 'd' in this library file denotes debugging, 320 denotes version 3.20)

To pass arguments i.e. input?
* go to the directory where your project is saved. Create a folder named 'data' under the project folder.
* add resources to the 'data' folder. let's say you have saved a image 'img.jpg'.
* Go to project, right click 'properties' -> Debugging -> Command Arguments -- now type   ../data/img1.jpg and 'Apply'.
*/


#include "opencv2/photo.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include <string> 
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, const char** argv)
{
	//read first image to create the blueprint
	Mat average = imread("1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (average.empty()) { //Checkpoint
		cout << "Error [blueprint creation] : Image cannot be loaded" << endl;
		return -1;
	}
	//empty 2D array for doing arithmetic operations on
	int **collect;
	collect = new int*[50];
	for (int x = 0; x < 50; ++x) {
		collect[x] = new int[50];
		for (int y = 0; y < 50; ++y) {
			collect[x][y] = 0;
		}
	}
	//combined vector
	int **vectors;
	vectors = new int*[11];
	for (int x = 0; x < 11; ++x) {
		vectors[x] = new int[2500];
		for (int y = 0; y < 2500; ++y) {
			vectors[x][y] = 0;
		}
	}

	//read the rest of the images to add them up pixel by pixel
	Mat img;
	int s = 0;
	for (int i = 2; i <= 10; i++) {
		//1. read in the file
		string file = to_string(i) + ".jpg";
		img = imread(file, CV_LOAD_IMAGE_GRAYSCALE);
		if (img.empty()) { //Checkpoint
			cout << "Error[imageload loop] : Image cannot be loaded" << endl;
			return -1;
		}
		//2. collect total values
		s = 0;
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				Scalar intensity = img.at<uchar>(n, k);
				collect[n][k] += intensity.val[0];
				vectors[i][s] = intensity.val[0];
				++s;
			}
		}
		//3. input average back into one image
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				Scalar intensity2 = average.at<uchar>(n, k);
				intensity2.val[0] = (collect[n][k]) / 10;
				average.at<uchar>(n, k) = intensity2.val[0];
			}
		}

	}
	//subtract average face vector from face vectors


	//imwrite("result.jpg", average);
	namedWindow("MyWindow", WINDOW_NORMAL); //create a window with the name "MyWindow"
	imshow("MyWindow", average); //display the image which is stored in the 'img' in the "MyWindow" window

	waitKey(0); //wait infinite time for a keypress

	destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
	
	for (int i = 0; i < 50; ++i) {
		delete[i]collect;
	}
	
	for (int i = 0; i < 11; ++i) {
			delete[i] vectors;
	}
	return 0;
}
