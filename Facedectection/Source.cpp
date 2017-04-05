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
#include <vector>
using namespace cv;
using namespace std;

/**Function readimage
	1 argument - string: file
	-Read .jpg file associated with that filename and store in a matrix
	Return matrix - Mat: image
*/
Mat readimage(string file) {
	Mat image = imread(file, CV_LOAD_IMAGE_GRAYSCALE);
	if (image.empty()) { //Checkpoint
		cout << "Error [image read in] : Image cannot be loaded" << endl;
	}
	return image;
}

//Main Function
int main(int argc, const char** argv) {
	//read first image then create the blueprint
	Mat image = readimage("1.jpg");
	Mat average = Mat::zeros(image.size(), image.type());

	//empty 2D array for accumulating pixel values over 255 and computing average
	int **collect;
	collect = new int*[50];
	for (int x = 0; x < 50; ++x) {
		collect[x] = new int[50];
		for (int y = 0; y < 50; ++y) {
			collect[x][y] = 0;
		}
	}

	//read the rest of the images to add them up pixel by pixel to the collect array
	int no_of_cols = 11;
	int no_of_rows = 2500;
	int initial_value = 0;
	vector< vector<int > > trainingdata;
	trainingdata.resize(no_of_rows, std::vector<int>(no_of_cols, initial_value));
	int s = 0;
	for (int i = 1; i <= 10; ++i) {
		s = 0;
		//1. read in the file
		string file = to_string(i) + ".jpg";
		image = readimage(file);
		//2. collect total values
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				Scalar intensity = image.at<uchar>(n, k);
				collect[n][k] += intensity.val[0];
				trainingdata[s][i]= intensity.val[0];
				//vectors[i][s] = intensity.val[0];
				++s;
			}
		}
		s = 0;
		//3. input average back into one image
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				Scalar intensity2 = average.at<uchar>(n, k);
				intensity2.val[0] = (collect[n][k]) / 10;
				average.at<uchar>(n, k) = intensity2.val[0];
				trainingdata[0][s]= intensity2.val[0];
				++s;
			}
		}

	}

	//subtract average face vector from face vectors
	for (int i = 0; i < 10; ++i) {
		for (int n = 0; n < 2500; ++n) {
			trainingdata[i + 1][n] -= trainingdata[0][n];
		}
	}
	for (int i = 0; i < 2500; i++) {
		cout << trainingdata[1][i] << " ";
	}
	Mat trainingmatrix(trainingdata.size(), trainingdata[0].size(), average.type());
	for (size_t i = 0; i < trainingdata.size(); i++) {
		for (size_t j = 0; j < trainingdata[0].size(); j++) {
			trainingmatrix.at<int>(i, j) = trainingdata[i][j];
		}
	}
	//Calculate the Covariance matrix
	Mat trainingB;
	transpose(trainingmatrix, trainingB);
	Mat Covmatrix;
	Covmatrix= trainingmatrix * trainingB;
	cout << "Columns " << Covmatrix.cols << " Rows " << Covmatrix.rows << endl;

	//imwrite("result.jpg", average);
	namedWindow("MyWindow", WINDOW_NORMAL); //create a window with the name "MyWindow"
	imshow("MyWindow", average); //display the image which is stored in the 'img' in the "MyWindow" window

	waitKey(0); //wait infinite time for a keypress

	destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
	

	return 0;
}

/** 
//combined vector
int **vectors;
vectors = new int*[11];
for (int x = 0; x < 11; ++x) {
vectors[x] = new int[2500];
for (int y = 0; y < 2500; ++y) {
vectors[x][y] = 0;
}
}

for (int i = 0; i < 50; ++i) {
delete[i]collect;
}

for (int i = 0; i < 11; ++i) {
delete[i] vectors;
}
*/