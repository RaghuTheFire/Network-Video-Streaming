
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() 
{
    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) 
    {
        std::cerr << "Failed to open camera" << std::endl;
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080); // Set the destination port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set the destination IP address

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

        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
        cv::imencode(".jpg", frame, buffer, params);

        sendto(sockfd, buffer.data(), buffer.size(), 0, (sockaddr*)&servaddr, sizeof(servaddr));
    }

    close(sockfd);
    cap.release();
    return 0;
}
