// UDP Video Receiver
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

void receiveVideo(int serverPort) 
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
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        std::cerr << "Failed to bind socket" << std::endl;
        close(sockfd);
        return;
    }

    cv::Mat frame;
    std::vector<uchar> buffer(65536);
    while (true) 
    {
        socklen_t addrLen = sizeof(serverAddr);
        int bytesReceived = recvfrom(sockfd, buffer.data(), buffer.size(), 0,(struct sockaddr*)&serverAddr, &addrLen);
        if (bytesReceived < 0) 
        {
            std::cerr << "Failed to receive data" << std::endl;
            break;
        }

        frame = cv::imdecode(cv::Mat(buffer.data(), 1, bytesReceived, CV_8UC1), cv::IMREAD_COLOR);
        if (frame.empty()) 
        {
            std::cerr << "Failed to decode frame" << std::endl;
            continue;
        }

        cv::imshow("Video Stream", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    close(sockfd);
    cv::destroyAllWindows();
}

void main()
{
  receiveVideo(2556); 
}
