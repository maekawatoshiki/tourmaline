#include "error.hpp"

void Reporter::error(std::string filename, int line, const char *err_content, ...) {
  std::cout << filename << ":" << line << ": error: ";
  va_list args;
  va_start(args, err_content);
    vprintf(err_content, args); puts("");
  va_end(args);
  exit(0);
}

void Reporter::error_file_failed(std::string filename) {
  std::cout << "error: not found file '" << filename << "'" << std::endl;
  exit(0);
}
