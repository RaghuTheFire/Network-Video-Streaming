
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

using namespace cv;
using namespace std;

int main() 
{
    // Open the video capture device
    VideoCapture cap(0);

    // Check if the video capture device was opened successfully
    if (!cap.isOpened()) 
    {
        cerr << "Error: Could not open video capture device" << endl;
        return -1;
    }

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        cerr << "Error: Could not create socket" << endl;
        return -1;
    }

    // Set up the server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Replace with the desired port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with the desired IP address

    // Connect to the server
    if (connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        cerr << "Error: Could not connect to server" << endl;
        close(sockfd);
        return -1;
    }

    // Create a matrix to store the video frame
    Mat frame;

    // Loop to capture and send video frames
    while (true) 
    {
        // Capture a frame from the video capture device
        cap >> frame;

        // Check if the frame was captured successfully
        if (frame.empty()) 
        {
            cerr << "Error: Could not capture frame" << endl;
            break;
        }

        // Encode the frame as a vector of bytes
        vector<uchar> buffer;
        imencode(".jpg", frame, buffer);

        // Send the encoded frame over the TCP socket
        send(sockfd, buffer.data(), buffer.size(), 0);
    }

    // Clean up
    close(sockfd);
    cap.release();

    return 0;
}
