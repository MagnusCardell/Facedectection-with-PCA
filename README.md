Facedetection project

Using OpenCV we will calculate the PCA values.


Linking opencv to VisualStuido:
Right click project in the Solution Explorer and click 'Properties', click 'C/C++'.
Add the OpenCV resource by pasting the include directory into 'Additional Include Libraries', for example: C:\Users\Magnus\Documents\opencv\build\include, and click 'Apply'
To link the libraries: go to 'Linker' -> 'General' and pase C:\Users\Magnus\Documents\opencv\build\x64\vc14\lib in 'Additional Library Directories'. 

Go to 'Linker' -> 'Input' ->'Additional Dependencies' -> paste 'opencv_world320d.lib' (remember - 'd' in this library file denotes debugging, 320 denotes version 3.20)

To access images in your folder:
Go to the directory where your project is saved. Create a folder named 'data' under the project folder.
add resources to the 'data' folder. let's say you have saved a image 'img.jpg'.
Go to project properties, now under Debugging, you should fill in that folder in the 'Environment' box for example ../data/
Select 'Apply'.
