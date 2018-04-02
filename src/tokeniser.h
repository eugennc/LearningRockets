#ifndef __TOKENISER_H__
#define __TOKENISER_H__

#include <vector>
#include <string>

namespace SdimpleRocket {

std::vector<std::string> inline tokenise(const std::string &source, const char *delimiter = " ", bool keepEmpty = false) {
  std::vector<std::string> results;
  size_t prev = 0;
  size_t next = 0;
  while ((next = source.find_first_of(delimiter, prev)) != std::string::npos) {
    if (keepEmpty || (next - prev != 0)) {
      results.push_back(source.substr(prev, next - prev));
    }
    prev = next + 1;
  }

  if (prev < source.size()) {
    results.push_back(source.substr(prev));
  }
  return results;
}

}
#endif //__TOKENISER_H__
