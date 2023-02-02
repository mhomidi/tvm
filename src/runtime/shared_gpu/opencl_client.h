// Write CPP code here
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <dmlc/parameter.h>

#define SA struct sockaddr
#define CHUNK_SIZE 1000 * 16
#define DATA_SIZE 1024 * 16
#define TYPE_SIZE 1
#define HASH_SIZE 8

#define KERNEL_TYPE "k"
#define KERNEL__NAME_TYPE "n"
#define BUFFER_CREATE_TYPE "b"
#define BUFFER_GET_TYPE "c"
#define BUFFER_DATA_TYPE "d"
#define SET_BUFFER_TYPE "s"
#define GLOBAL_WORK_SIZE_TYPE "g"
#define WORK_DIM_TYPE "w"

namespace tvm {
namespace sharedGPU {
    
class Client {
public:
    Client();
    ~Client();
    int SendData(void* data, size_t size);
    int ReceiveData();
    void SendKernel(std::string kernelName, std::string kernelSource);
    void SendCreateBufferRequest(void* buffer, size_t size);
    size_t getHash(std::string str);
    size_t getHash(void* addr);
    void SendBufferData(size_t bufferHash, void* data, size_t size);
    void SendBufferSet(std::string kernelName, size_t bufferHash);
    void SendGlobalWorkSize(std::string kernelName, void* data);
    void SendWorkDim(std::string kernelName, size_t dim);
    void CloseConnection();

    // TODO : remove
    void SendGetBufferRequest(size_t buffer);

private:
    int sockfd;
    struct sockaddr_in servaddr;
    int port;
    int userId;
    char recvData[20];

    fd_set readfds;
    fd_set wrfds;
    void CreateConnection();

};

}
}