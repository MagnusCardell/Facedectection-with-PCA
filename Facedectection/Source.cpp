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
#include <opencv2/highgui.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core.hpp>
#include "opencv2/imgcodecs.hpp"
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
struct params
{
	Mat data;
	int ch;
	int rows;
	PCA pca;
	string winName;
};
Mat asRowMatrix(vector<Mat>& faces) {
	// Number of samples:
	int n = faces.size();
	if (n == 0) {
		return Mat();
	}
	// dimensionality of (reshaped) samples
	int d = faces[0].total();
	// Create returning data matrix and copy over data:
	Mat data(n, d, CV_32FC1);
	for (int i = 0; i < n; i++) {
		if (faces[i].empty()) {
			string error_message = format("Image number %d was empty, please check your input data.", i);
		}
		if (faces[i].total() != d) {
			string error_message = format("Wrong number of elements in matrix #%d! Expected %d was %d.", i, d, faces[i].total());
		}
		// Get a hold of the current row:
		Mat xi = data.row(i);
		// Make reshape happy by cloning for non-continuous matrices:
		if (faces[i].isContinuous()) {
			faces[i].reshape(1, 1).convertTo(xi, CV_32FC1, 1, 0);
		}
		else {
			faces[i].clone().reshape(1, 1).convertTo(xi, CV_32FC1, 1, 0);
		}
	}
	return data;
}
static  Mat formatImagesForPCA(const vector<Mat> &data)
{
	Mat dst(static_cast<int>(data.size()), data[0].rows*data[0].cols, CV_32F);
	for (unsigned int i = 0; i < data.size(); i++)
	{
		Mat image_row = data[i].clone().reshape(1, 1);
		Mat row_i = dst.row(i);
		image_row.convertTo(row_i, CV_32F);
	}
	return dst;
}

//Main Function
int main(int argc, const char** argv) {
	//read first image then replace all values to create a blueprint
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
	//Vector of Matrices will collect all images
	vector<Mat> faces;


	//read the rest of the images to add them up pixel by pixel to the collect array
	int s = 0;
	for (int i = 1; i <= 10; ++i) {
		//1. read in the file
		string file = to_string(i) + ".jpg";
		image = readimage(file);
		//2. collect total values
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				//Target pixel value and store it in collect array
				Scalar intensity = image.at<uchar>(n, k);
				collect[n][k] += intensity.val[0];
			}
		}
		//store image in vector
		faces.push_back(image);
		//3. input average back into one image to get the EIGENFACE
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				Scalar intensity2 = average.at<uchar>(n, k);
				intensity2.val[0] = (collect[n][k]) / 10;
				average.at<uchar>(n, k) = intensity2.val[0];
			}
		}

	}
	//subtract average faces:
	for (int i = 0; i<10; i++) {
		faces[i] -= average;
		imshow("window" + i, faces[i]);
	}
	//Get the covariance matrix
	Mat combine = formatImagesForPCA(faces); //function for pushing all images into one matrix
	Mat covariance, mean2;
	calcCovarMatrix(combine, covariance, mean2, COVAR_ROWS| cv::COVAR_NORMAL);

	//cout << "covriance = " << endl << " " << covariance << endl << endl;
	cout << covariance.size() << endl;
	imshow("Covariance", covariance);

	//Get Egienvalues and eigenvectors
	Mat eigenval, eigenvect;
	PCA pca(combine, Mat(), PCA::DATA_AS_ROW, 10);
	eigenval = pca.eigenvalues;
	eigenvect = pca.eigenvectors;
	cout << "eigenval = " << endl << " " << eigenval << endl << endl;
	cout << "Eigenval size: " << eigenval.size() << " Eigenvect size: " << eigenvect.size() << endl;
	//cout << "eigenvect = " << endl << " " << eigenvect << endl << endl;
	imshow("eigen", eigenval);
	/**
	//Choose best eigenvector values
	Mat vectoreigen(40,2500,CV_32FC1);
	vectoreigen.push_back(eigenvect.row(0));
	vectoreigen.push_back(eigenvect.row(1));
	vectoreigen.push_back(eigenvect.row(2));
	vectoreigen.push_back(eigenvect.row(3));
	cout << vectoreigen.size() << endl;

	Mat faces[0] = faces[0] - vectoreigen.row(0);

	*/
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