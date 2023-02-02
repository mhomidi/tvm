#include "opencl_client.h"

namespace tvm {
namespace sharedGPU {

Client::Client() 
{
    this->port = 8888;
    this->CreateConnection();
}


Client::~Client()
{
    this->CloseConnection();
} 

void Client::CreateConnection()
{

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sockfd == -1)
        exit(0);
    
    bzero(&(this->servaddr), sizeof(this->servaddr));

    // assign IP, PORT
    this->servaddr.sin_family = AF_INET;
    this->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->servaddr.sin_port = htons(this->port);

    // connect the client socket to server socket
    if (connect(this->sockfd, (SA *)&this->servaddr, sizeof(this->servaddr)) != 0)
        exit(0);
}

void Client::CloseConnection()
{
    close(this->sockfd);
}

int Client::SendData(void* data, size_t size)
{
    send(this->sockfd, data, size, 0);
    usleep(1000 * 100);
    return 0;
}

int Client::ReceiveData()
{
    recv(this->sockfd, this->recvData, 2, 0);
    return 0;
}

void Client::SendKernel(std::string kernelName, std::string kernelSource)
{
    size_t hashCode = this->getHash(kernelName);
    char* data = (char *)malloc(sizeof(char) * DATA_SIZE);

    memset(data, 0, DATA_SIZE);
    memcpy(data, KERNEL__NAME_TYPE, TYPE_SIZE);
    memcpy(data + TYPE_SIZE, (char*)&hashCode, HASH_SIZE);
    memcpy(data + TYPE_SIZE + HASH_SIZE, kernelName.c_str(), CHUNK_SIZE);
    this->SendData((void *)data, DATA_SIZE);

    int size = kernelSource.size();
    int chunk = 0;
    while (chunk <= size) {
        memset(data, 0, DATA_SIZE);
        memcpy(data, KERNEL_TYPE, TYPE_SIZE);
        memcpy(data + TYPE_SIZE, (char*)&hashCode, HASH_SIZE);
        memcpy(data + TYPE_SIZE + HASH_SIZE, (kernelSource.substr(chunk, CHUNK_SIZE - 1)).c_str(), CHUNK_SIZE);
        this->SendData((void*) data, DATA_SIZE);
        chunk += CHUNK_SIZE - 1;
    }
    
    free(data);
}

size_t Client::getHash(std::string str) 
{
    std::hash<std::string> strHash;
    return strHash(str);
}

size_t Client::getHash(void* addr) 
{
    std::hash<void*> strHash;
    return strHash(addr);
}

void Client::SendCreateBufferRequest(void* buffer, size_t size)
{
    size_t bufferHash = this->getHash(buffer);
    char* data = (char *)malloc(sizeof(char) * DATA_SIZE);
    memset(data, 0, DATA_SIZE);
    memcpy(data, BUFFER_CREATE_TYPE, TYPE_SIZE);
    memcpy(data + TYPE_SIZE, (char *)&bufferHash, HASH_SIZE);
    memcpy(data + TYPE_SIZE + HASH_SIZE, &size, sizeof(size_t));
    this->SendData(data, DATA_SIZE);
    free(data);
}

void Client::SendBufferData(size_t bufferHash, void* data, size_t size)
{
    size_t mainSize = (size + TYPE_SIZE + 2 * HASH_SIZE);
    char* sendData = (char *)malloc(sizeof(char) * DATA_SIZE);
    size_t chunk = 0;
    while (chunk <= size) {
        memset(sendData, 0, DATA_SIZE);
        memcpy(sendData, BUFFER_DATA_TYPE, TYPE_SIZE);
        memcpy(sendData + TYPE_SIZE, (char *)&bufferHash, HASH_SIZE);
        size_t chuckDataSize = (CHUNK_SIZE < size - chunk) ? CHUNK_SIZE : size - chunk;
        memcpy(sendData + TYPE_SIZE + HASH_SIZE, (char *)&chuckDataSize, HASH_SIZE);
        memcpy(sendData + TYPE_SIZE + 2 * HASH_SIZE, (char *)data + chunk, chuckDataSize);
        this->SendData(sendData, mainSize);
        chunk += CHUNK_SIZE;
    }
    free(sendData);
}

void Client::SendBufferSet(std::string kernelName, size_t bufferHash)
{
    size_t hashCode = this->getHash(kernelName);
    char* data = (char *)malloc(sizeof(char) * DATA_SIZE);
    memset(data, 0, DATA_SIZE);
    memcpy(data, SET_BUFFER_TYPE, TYPE_SIZE);
    memcpy(data + TYPE_SIZE, (char *)&hashCode, HASH_SIZE);
    memcpy(data + TYPE_SIZE + HASH_SIZE, &bufferHash, sizeof(size_t));
    this->SendData(data, DATA_SIZE);
    free(data);
}


void Client::SendGlobalWorkSize(std::string kernelName, void* data)
{
    size_t hashCode = this->getHash(kernelName);
    char* sentData = (char *)malloc(sizeof(char) * DATA_SIZE);
    memset(sentData, 0, DATA_SIZE);
    memcpy(sentData, GLOBAL_WORK_SIZE_TYPE, TYPE_SIZE);
    memcpy(sentData + TYPE_SIZE, (char *)&hashCode, HASH_SIZE);
    memcpy(sentData + TYPE_SIZE + HASH_SIZE, data, sizeof(size_t) * 6);
    this->SendData(sentData, DATA_SIZE);
    free(sentData);
}

void Client::SendWorkDim(std::string kernelName, size_t dim)
{
    size_t hashCode = this->getHash(kernelName);
    char* sentData = (char *)malloc(sizeof(char) * DATA_SIZE);
    memset(sentData, 0, DATA_SIZE);
    memcpy(sentData, WORK_DIM_TYPE, TYPE_SIZE);
    memcpy(sentData + TYPE_SIZE, (char *)&hashCode, HASH_SIZE);
    memcpy(sentData + TYPE_SIZE + HASH_SIZE, &dim, sizeof(size_t));
    this->SendData(sentData, DATA_SIZE);
    free(sentData);

}

// TODO: Remove
void Client::SendGetBufferRequest(size_t buffer)
{

    size_t bufferHash = buffer;
    char* data = (char *)malloc(sizeof(char) * DATA_SIZE);
    memset(data, 0, DATA_SIZE);
    memcpy(data, BUFFER_GET_TYPE, TYPE_SIZE);
    memcpy(data + TYPE_SIZE, (char *)&bufferHash, HASH_SIZE);
    this->SendData(data, DATA_SIZE);
    free(data);
}


}
}