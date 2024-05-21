
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;
using namespace cv;

int main() 
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        cerr << "Failed to create socket" << endl;
        return -1;
    }

    // Set up server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000); // Replace with the desired port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with the server IP address

    // Connect to the server
    if (connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
    {
        cerr << "Failed to connect to server" << endl;
        return -1;
    }

    // Create a window to display the video
    namedWindow("Video", WINDOW_NORMAL);

    // Receive and display video frames
    while (true) 
    {
        // Receive the frame size
        int frameSize;
        recv(sockfd, &frameSize, sizeof(frameSize), 0);

        // Receive the frame data
        vector<uchar> frameData(frameSize);
        int bytesReceived = 0;
        while (bytesReceived < frameSize) 
        {
            int result = recv(sockfd, frameData.data() + bytesReceived, frameSize - bytesReceived, 0);
            if (result == -1) 
            {
                cerr << "Failed to receive frame data" << endl;
                break;
            }
            bytesReceived += result;
        }

        // Create a Mat object from the received data
        Mat frame = imdecode(frameData, IMREAD_COLOR);

        // Display the frame
        imshow("Video", frame);

        // Check for user input (press 'q' to quit)
        char key = waitKey(1);
        if (key == 'q') 
        {
            break;
        }
    }

    // Close the socket
    close(sockfd);

    return 0;
}
