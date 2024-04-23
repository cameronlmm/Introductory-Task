//Introductory Task
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

// Declaring namespaces here to avoid repetition of namespaces throughout code
using namespace cv;
using namespace std;

int main() 
{
    // Load the image
    Mat image = imread("/Users/cameron/Desktop/RedCar.bmp"); // Path to image

    // Display original image pre-modifications
    namedWindow("Original Image");
    imshow("Original Image", image);

    // Convert image to HSV
    Mat imageHSV;
    cvtColor(image, imageHSV, COLOR_BGR2HSV);

    // Define HSV thresholds to isolate each color and create respective HSV mask
    Mat lowRed;
    inRange(imageHSV, Scalar(0, 2, 13), Scalar(10, 255, 255), lowRed); // HSV threshold for red (low end of spectrum)
    Mat highRed;
    inRange(imageHSV, Scalar(130, 24, 0), Scalar(180, 255, 255), highRed); // HSV threshold for red (high end of spectrum)
    Mat redMask;
    bitwise_or(lowRed, highRed, redMask); // Because red is on both ends of the spectrum, the two thresholds have to be combined
    
    Mat blueMask;
    inRange(imageHSV, Scalar(85, 82, 0), Scalar(144, 255, 255), blueMask); // HSV threshold for blue
    
    Mat greenMask;
    inRange(imageHSV, Scalar(36, 75, 25), Scalar(74, 255, 255), greenMask); // HSV threshold for green

    vector<Scalar> colors = {Scalar(0, 150, 255), Scalar(200, 0, 200), Scalar(0, 255, 255)}; // vector for BGR colors for drawing contours
    vector<string> colorNames = {"RED", "BLUE", "GREEN"}; // Vector for storing identified colors (labels)
    vector<Mat> combinedMask = {redMask, blueMask, greenMask}; // Vector which stores the masks of isolated colors above

    double maxArea = 0.0; // Initialise variable for storing contour with largest area
    string largestObjectColor; // Initialise variable for storing the name of the object

    // Iterate over each color's HSV mask
    for (int i = 0; i < combinedMask.size(); i++) 
    {
        // Morphology Operations -- morph each mask to filter out noise, etc
        Mat morphed; // Variable for storing morphed imaged
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7)); // using ellipse structuring element for morphing
        morphologyEx(combinedMask[i], morphed, MORPH_OPEN, kernel); // 'open' operation for each mask to remove small spots and noise
        morphologyEx(morphed, morphed, MORPH_CLOSE, kernel); // 'close' operation for each mask to close small spots in isolated color mask

        // Defining contours for morphed images
        vector<vector<Point>> contours; // vector for storing detected contours
        vector<Vec4i> hierarchy; // Vector for storing contour hierarchy data
        findContours(morphed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find (external/outer) contours in each morphed image

        // Iterate through all the contours found
        for (const auto &contour : contours) 
        {
            double area = contourArea(contour); // calculate area of contour
            if (area > maxArea) // if the area of the current contour in the loop is greater than the current highest area
            {
                maxArea = area; // update maximum area to the area of this contour
                largestObjectColor = colorNames[i]; // set the name of the largest object to the name of this color
            }
            drawContours(image, contours, -1, colors[i], 2); // draw all contours on image (-1) using lines (width of 2) and BGR colors declared above
        }
    }
    
    // Print the color of the object
    cout << "The color of object is: " << "*** " << largestObjectColor << " ***" << " (Predominant Color)" << endl;

    // Display masks of isolated colors and final image with contours
    namedWindow("Isolated Red");
    imshow("Isolated Red", redMask); // Isolated red HSV mask
    
    namedWindow("Isolated Blue");
    imshow("Isolated Blue", blueMask); // Isolated blue HSV mask
    
    namedWindow("Isolated Green");
    imshow("Isolated Green", greenMask); // Isolated green HSV mask
    
    namedWindow("Object Detected");
    imshow("Object Detected", image); // Final image with contours

    waitKey(); // Wait for a key press to exit

    return 0; // exit when any key pressed
}
