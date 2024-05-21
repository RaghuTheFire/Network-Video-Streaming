// UDP Video Streamer
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

void streamVideo(const char* serverIP, int serverPort) 
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) 
    {
        std::cerr << "Failed to open camera" << std::endl;
        close(sockfd);
        return;
    }

    cv::Mat frame;
    std::vector<uchar> buffer;
    while (true) 
    {
        cap >> frame;
        if (frame.empty()) 
        {
            std::cerr << "Failed to capture frame" << std::endl;
            break;
        }

        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 80};
        cv::imencode(".jpg", frame, buffer, params);

        int bytesSent = sendto(sockfd, buffer.data(), buffer.size(), 0,(struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (bytesSent < 0) 
        {
            std::cerr << "Failed to Send data" << std::endl;
            break;
        }
    }

    cap.release();
    close(sockfd);
}

void main()
{
  streamVideo("127.0.0.1, 2556); 
}

