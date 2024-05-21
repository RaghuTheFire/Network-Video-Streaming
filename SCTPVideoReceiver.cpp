
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <opencv2/opencv.hpp>

int main() 
{
    // Create SCTP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (sockfd < 0) 
    {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // Set up server address
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8000);

    // Bind socket to address
    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) 
    {
        std::cerr << "Failed to bind socket" << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) 
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -1;
    }

    std::cout << "Listening for incoming connections..." << std::endl;

    // Accept incoming connection
    sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    int connfd = accept(sockfd, (sockaddr*)&clientaddr, &clientlen);
    if (connfd < 0) 
    {
        std::cerr << "Failed to accept connection" << std::endl;
        return -1;
    }

    std::cout << "Connection accepted" << std::endl;

    // Receive and display video frames
    cv::VideoCapture cap;
    cap.open(connfd);
    if (!cap.isOpened()) 
    {
        std::cerr << "Failed to open video stream" << std::endl;
        return -1;
    }

    cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;
    while (true) 
    {
        cap >> frame;
        if (frame.empty()) 
        {
            break;
        }
        cv::imshow("Video", frame);
        if (cv::waitKey(30) >= 0) 
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    close(connfd);
    close(sockfd);

    return 0;
}
