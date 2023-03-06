
#include "vortex_grpc_client.h"

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <service.pb.h>

namespace tvm {
namespace vortexGRPC {

Client* Client::instance = nullptr;

Client::Client() {
  auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
  this->stub = tvmgrpc::VortexService::NewStub(channel);
  this->setUserId();
}

Client* Client::GetInstance() {
  if (instance == nullptr) {
    return instance = new Client();
  }
  return instance;
}

void Client::setUserId() {
  tvmgrpc::UserData res;
  tvmgrpc::EmptyMessage req;
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  grpc::Status status = this->stub->GenerateNewUserId(&context, req, &res);
  this->userId = res.user_id();
}

void Client::SendKernel(std::string kernelName, std::string src) {
  tvmgrpc::Response res;
  tvmgrpc::KernelSource req;
  req.set_name(kernelName);
  req.set_source(src);
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  grpc::Status status = this->stub->SetKernelSource(&context, req, &res);
}

void Client::CreateBuffer(size_t bufferId, size_t size) {
  tvmgrpc::BufferCreation req;
  tvmgrpc::Response res;
  req.set_id(bufferId);
  req.set_size(size);
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  grpc::Status status = this->stub->CreateBuffer(&context, req, &res);
}

void Client::SendBufferData(size_t bufferId, float* data, size_t size) {
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  tvmgrpc::Response res;
  std::unique_ptr<grpc::ClientWriter<tvmgrpc::BufferData>> writer(
      (this->stub)->SetBufferData(&context, &res));
  for (size_t i = 0; i < size / sizeof(float); i++) {
    tvmgrpc::BufferData req;
    req.set_data(data[i]);
    req.set_id(bufferId);
    if (!writer->Write(req)) break;
  }
  writer->WritesDone();
  grpc::Status status = writer->Finish();
  if (status.ok())
    std::cout << bufferId << "'s data has been sent: " << size / sizeof(float) << std::endl;
  else
    throw 1;
}

void Client::SetBuffer(std::string kernelName, size_t bufferId) {
  tvmgrpc::BufferSet req;
  tvmgrpc::Response res;
  req.set_kernel_name(kernelName);
  req.set_buffer_id(bufferId);
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  grpc::Status status = this->stub->SetBufferToKernel(&context, req, &res);
}

void Client::SendThreadData(std::string kernelName, size_t* workSizes, size_t dim) {
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  tvmgrpc::Response res;
  std::unique_ptr<grpc::ClientWriter<tvmgrpc::WorkGroupData>> writer(
      (this->stub)->SetWorkGroupData(&context, &res));
  for (size_t i = 0; i < 6; i++) {
    tvmgrpc::WorkGroupData wg;
    wg.set_kernel_name(kernelName);
    wg.set_index(i);
    wg.set_size(workSizes[i]);
    if (!writer->Write(wg)) break;
  }
  writer->WritesDone();
  grpc::Status status = writer->Finish();
  if (!status.ok()) {
    throw 1;
  }
  tvmgrpc::WorkDim req;
  req.set_kernel_name(kernelName);
  req.set_dim(dim);
  grpc::ClientContext context2;
  context2.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  status = this->stub->SetKernelWorkDim(&context2, req, &res);
}

void Client::EnqueueKernel(std::string kernelName) {
  tvmgrpc::KernelSource req;
  tvmgrpc::Response res;
  req.set_name(kernelName);
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  grpc::Status status = this->stub->SetKernelReadyToExecute(&context, req, &res);
}

std::vector<float> Client::GetBufferData(size_t bufferId, size_t size) {
  tvmgrpc::BufferCreation req;
  tvmgrpc::BufferData res;
  req.set_id(bufferId);
  req.set_size(size);
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  std::vector<float> data;
  std::unique_ptr<grpc::ClientReader<tvmgrpc::BufferData>> reader(
      this->stub->GetBufferData(&context, req));
  while (reader->Read(&res)) {
    data.push_back(res.data());
  }
  grpc::Status status = reader->Finish();
  if (!status.ok()) {
    throw 1;
  }
  return data;
}

void Client::SendKernelDependency(std::string curr_kernel, std::string pred_kernel) {
  tvmgrpc::KernelDependency req;
  tvmgrpc::Response res;
  req.set_curr(curr_kernel);
  req.set_pred(pred_kernel);
  grpc::ClientContext context;
  context.AddMetadata(CLIENT_ID, grpc::to_string(this->userId));
  grpc::Status status = this->stub->SetKernelDependency(&context, req, &res);
}

}  // namespace vortexGRPC
}  // namespace tvm
