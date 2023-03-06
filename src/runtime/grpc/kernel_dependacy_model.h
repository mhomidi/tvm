
#include <map>
#include <string>
#include <vector>

class KernelDependancyModel {
 public:
  static KernelDependancyModel* GetInstance();
  std::string GetLastDependency(size_t arg);
  void SendDependency(size_t arg, std::string kernel_name);

 private:
  KernelDependancyModel() {}

  static KernelDependancyModel* instance;
  std::map<size_t, std::string> last_dep;
};