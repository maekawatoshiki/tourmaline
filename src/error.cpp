#include "error.hpp"

template<typename ... Args>
void Reporter::error(std::string filename, int line, char *err_content, Args ... args) {
  std::cout << filename << ":" << line << ": error: ";
  printf(err_content, args ...);
  exit(EXIT_SUCCESS);
}

void Reporter::error_file_failed(std::string filename) {
  std::cout << "error: not found file '" << filename << "'" << std::endl;
  exit(0);
}
