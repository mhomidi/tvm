#include "kernel_dependacy_model.h"

#ifdef WITH_GRPC
#include "vortex_grpc_client.h"
#endif

KernelDependancyModel* KernelDependancyModel::instance = nullptr;

KernelDependancyModel* KernelDependancyModel::GetInstance() {
  if (instance == nullptr) return instance = new KernelDependancyModel();
  return instance;
}

std::string KernelDependancyModel::GetLastDependency(size_t arg) {
  if (this->last_dep.find(arg) == this->last_dep.end()) return "";
  return this->last_dep[arg];
}

void KernelDependancyModel::SendDependency(size_t arg, std::string kernel_name) {
  std::string last_dep_kernel = this->GetLastDependency(arg);
  tvm::vortexGRPC::Client* client = tvm::vortexGRPC::Client::GetInstance();
  client->SendKernelDependency(kernel_name, last_dep_kernel);
  this->last_dep[arg] = kernel_name;
}