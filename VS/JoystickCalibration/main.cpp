#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		cout << "Usage: JoystickCalibration <image>" << endl;
		return -1;
	}

	//Read the file
	Mat image;
	image = imread(argv[1], IMREAD_COLOR);
	if (!image.data)
	{
		cout << "Could not open " << argv[1] << endl;
		return -1;
	}

	//Create a window for display
	namedWindow("Display window", WINDOW_AUTOSIZE);
	imshow("Display window", image);

	//Wait for a keystroke in the window
	waitKey(0);
	return 0;
}