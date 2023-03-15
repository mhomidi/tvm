
#include <map>
#include <set>
#include <string>

namespace tvm {
namespace vortexGRPC {

class KernelDependencyModel {
 public:
  static KernelDependencyModel* GetInstance();
  std::set<std::string> GetKernelDependencies(std::string kernel_name);
  void SetNodeDependency(std::string node, std::string dep);
  void SetLastNodeKernel(std::string kernel_name);

 private:
  KernelDependencyModel() {}

  static KernelDependencyModel* instance;
  std::map<std::string, std::set<std::string>> node_deps;
  std::map<std::string, std::string> last_kernels;
};
}  // namespace vortexGRPC

}  // namespace tvm
