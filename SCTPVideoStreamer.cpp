
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <netinet/sctp.h>

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

    // Create a SCTP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (sockfd < 0) 
    {
        cerr << "Error: Could not create SCTP socket" << endl;
        return -1;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8000);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        cerr << "Error: Could not bind SCTP socket" << endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) 
    {
        cerr << "Error: Could not listen on SCTP socket" << endl;
        return -1;
    }

    // Accept an incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (connfd < 0) 
    {
        cerr << "Error: Could not accept SCTP connection" << endl;
        return -1;
    }

    // Continuously read frames from the video capture device and send them over the SCTP socket
    Mat frame;
    vector<uchar> buffer;
    while (true) 
    {
        // Capture a frame from the video capture device
        cap >> frame;

        // Check if the frame was captured successfully
        if (frame.empty()) 
        {
            cerr << "Error: Could not capture frame from video capture device" << endl;
            break;
        }

        // Encode the frame as a vector of bytes
        imencode(".jpg", frame, buffer);

        // Send the encoded frame over the SCTP socket
        if (send(connfd, buffer.data(), buffer.size(), 0) < 0) 
        {
            cerr << "Error: Could not send data over SCTP socket" << endl;
            break;
        }
    }

    // Clean up
    cap.release();
    close(connfd);
    close(sockfd);

    return 0;
}
