/*
* Facedetection.cpp
* Using OpenCV

To pass arguments i.e. input
* This project uses images from a data folder. This should further be placed in the project file.
* Go to project, right click 'properties' -> Debugging -> Command Arguments -- and set  ../data/ in the argument directory and 'Apply'.
*/


#include "opencv2/highgui/highgui.hpp"
#include <string> 
#include <iostream>
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

static  Mat formatImagesForPCA(const vector<Mat> &data) {
	Mat dst(static_cast<int>(data.size()), data[0].rows*data[0].cols, CV_32F);
	for (unsigned int i = 0; i < data.size(); i++) {
		Mat image_row = data[i].clone().reshape(1, 1);
		Mat row_i = dst.row(i);
		image_row.convertTo(row_i, CV_32F);
	}
	return dst;
}


// Normalizes a given image into a value range between 0 and 255.
//	Given the address to the matrix.
Mat norm_0_255(const Mat& src) {
	// Create and return normalized image:
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
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
				//Target pixel value and store it in collect array to perform pixel operations above 255
				// Scalar type is a 3-channel data type. We just want the first channel
				Scalar intensity = image.at<uchar>(n, k);
				collect[n][k] += intensity.val[0];
			}
		}
		//store image in vector
		faces.push_back(image);
		//3. input average back into one image to get the AVERAGE face
		for (int n = 0; n < 50; n++) {
			for (int k = 0; k < 50; k++) {
				//Scalar type is necessary to keep the 3-channel data type of a Matrix
				Scalar intensity2 = average.at<uchar>(n, k);
				intensity2.val[0] = (collect[n][k]) / 10;
				average.at<uchar>(n, k) = intensity2.val[0];
			}
		}
	}
	//subtract average faces from every face:
	for (int i = 0; i<10; i++) {
		faces[i] -= average;
		//imshow("window" + i, faces[i]);
		imwrite("featureface" + to_string(i) + ".jpg", faces[i]);
	}

	//Get the covariance matrix
	Mat combine = formatImagesForPCA(faces); //function for pushing all images into one matrix
	Mat covariance, mean2;
	//OpenCV function for getting the covariance matrix. Flag specify rows or columns
	calcCovarMatrix(combine, covariance, mean2, COVAR_ROWS| cv::COVAR_NORMAL);
	//cout << "covriance = " << endl << " " << covariance << endl << endl;
	cout << covariance.size() << endl;
	imwrite("Covariance.jpg", covariance);


	//Get Egienvalues and eigenvectors
	Mat eigenval, eigenvect;
	PCA pca(combine, Mat(), PCA::DATA_AS_ROW, 10);
	eigenval = pca.eigenvalues;
	eigenvect = pca.eigenvectors;
	cout << "eigenval = " << endl << " " << eigenval << endl << endl;
	//cout << "eigenvect = " << endl << " " << eigenvect << endl << endl;
	cout << "Eigenval size: " << eigenval.size() << " Eigenvect size: " << eigenvect.size() << endl;


	//Get the top 4 vectors from top 4 eigenvalues
	Mat top4vectors;
	top4vectors.push_back(eigenvect.row(0));
	top4vectors.push_back(eigenvect.row(1));
	top4vectors.push_back(eigenvect.row(2));
	top4vectors.push_back(eigenvect.row(3));

	//Multiply each eigenvector with each of the (face - average) matrix
	//Vector of images and vectors
	vector<Mat> eigenfacesimage;
	vector<Mat> eigenfacesvector;

	for (int n = 0; n < 10; n++) {
		for (int i = 0; i < 4; i++) {
			//Nth row of (face-average) x ith row of eigenvector by component multiplcication .mul()
			Mat eigenfacevector = combine.row(n).mul(top4vectors.row(i));
			eigenfacesimage.push_back(norm_0_255(eigenfacevector).reshape(1, faces[0].rows));
			eigenfacesvector.push_back(eigenfacevector);
			
		}
	}
	cout << "The amount of Eigenfaces are: "<< eigenfacesimage.size() << endl;
	
	for (int i = 0; i < eigenfacesimage.size(); i++) {
		imwrite("eigenface" + to_string(i) + ".jpg", eigenfacesimage[i]);
	}
	

		
	//TESTING PHASE!!!
	Mat testimage = readimage("test2.png");
	//Get the feature vector by subtracting the average of test phase
	testimage -= average;
	imwrite("featurevector.jpg", testimage);
	//Convert the image to vector row project the image on the eigenspac
	Mat testvect2 = testimage.reshape(0, 1);
	Mat testvect;
	testvect2.convertTo(testvect, CV_32FC1);

	//Multiplication by components
	//Vector of images and vectors
	vector<Mat> imageprojection;
	vector<Mat> vectprojection;

	for (int i = 0; i < 4; i++) {
		//Nth row of (face-average) x ith row of eigenvector by component multiplcication .mul()
		Mat temporary = testvect.mul(top4vectors.row(i));
		imageprojection.push_back(norm_0_255(temporary).reshape(1, faces[0].rows));
		imwrite("testprojection" + to_string(i)+ ".jpg", imageprojection.back());
		vectprojection.push_back(temporary);
	}
	
	//Calculate Euclidian distance
	vector<float>euclidiandist;
	for (int i = 0; i < 10; i++) {
		for (int n = 0; n < 4; n++) {
			double dist = norm(eigenfacesvector[i]- vectprojection[n], NORM_L2); //Euclidian distance
			euclidiandist.push_back(dist);
		}
	}

	float sum = 0;
	int threshhold = 30;
	bool face = 0;
	for (int i = 0; i < euclidiandist.size(); i++) {
		if (threshhold > euclidiandist[i]) {
			face = 1;
		}
		cout << euclidiandist[i] << endl;
		sum += euclidiandist[i];
	}
	float averagenumb = sum / euclidiandist.size();
	cout << "Average distance: "<< averagenumb << endl;

	if (face){
		cout << "This is a face" << endl;
	}
	imwrite("average.jpg", average);
	namedWindow("MyWindow", WINDOW_NORMAL); //create a window with the name "MyWindow"
	imshow("MyWindow", average); //display the image which is stored in the 'img' in the "MyWindow" window

	waitKey(0); //wait infinite time for a keypress

	destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"


	return 0;
}