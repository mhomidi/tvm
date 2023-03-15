#include "kernel_dependency_model.h"

#include "vortex_grpc_client.h"

namespace tvm {
namespace vortexGRPC {

KernelDependencyModel* KernelDependencyModel::instance = nullptr;

KernelDependencyModel* KernelDependencyModel::GetInstance() {
  if (instance == nullptr) return instance = new KernelDependencyModel();
  return instance;
}

void KernelDependencyModel::SetNodeDependency(std::string node, std::string dep) {
  if (this->node_deps.find(node) == this->node_deps.end()) {
    this->node_deps[node] = std::set<std::string>();
  }
  this->node_deps[node].insert(dep);
}

std::set<std::string> KernelDependencyModel::GetKernelDependencies(std::string kernel_name) {
  std::set<std::string> dep_kernels;
  int kernel_index = atoi(kernel_name.substr(kernel_name.size() - 1).c_str());
  std::string node = kernel_name.substr(0, kernel_name.find("_kernel"));
  if (kernel_index > 0) {
    std::string dep = node + "_kernel" + std::to_string(kernel_index - 1);
    dep_kernels.insert(dep);
    return dep_kernels;
  }
  if (this->node_deps.find(node) == this->node_deps.end()) throw 1;

  std::set<std::string> dep_nodes;
  dep_nodes = this->node_deps[node];
  for (std::string it : dep_nodes) {
    if (this->last_kernels.find(it) != this->last_kernels.end())
      dep_kernels.insert(this->last_kernels[it]);
  }
  return dep_kernels;
}

void KernelDependencyModel::SetLastNodeKernel(std::string kernel_name) {
  std::string node = kernel_name.substr(0, kernel_name.find("_kernel"));
  this->last_kernels[node] = kernel_name;
}

}  // namespace vortexGRPC
}  // namespace tvm