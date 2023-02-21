
#include <service.grpc.pb.h>

#define CLIENT_ID "client_id"

namespace tvm {
namespace vortexGRPC {

class Client {
 public:
  static Client* getInstance();
  void SendKernel(std::string kernelName, std::string src);
  void CreateBuffer(size_t bufferId, size_t size);
  void SendBufferData(size_t bufferId, float* data, size_t size);
  void SetBuffer(std::string kernelName, size_t bufferId);
  void SendThreadData(std::string kernelName, size_t* workSizes, size_t dim);
  void EnqueueKernel(std::string kernelName);
  std::vector<float> GetBufferData(size_t bufferId, size_t size);

 private:
  Client();
  void setUserId();
  static Client* instance;

  size_t userId;
  int port;
  std::unique_ptr<tvmgrpc::VortexService::Stub> stub;
};

}  // namespace vortexGRPC
}  // namespace tvm
