#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils.h"
namespace UTUI {

class Parser {
public:
  std::vector<std::unordered_map<std::string, std::string>>
  appendFromFile(const std::string &filename) {
    std::vector<std::string> lines = Utils::readFile(filename, ';');
    if (!lines.empty())
      lines.pop_back();
    std::vector<std::unordered_map<std::string, std::string>> initParams;

    for (const std::string &line : lines) {
      initParams.push_back(getParams(line));
    }

    return initParams;
  }

private:
  std::vector<std::string> getTokens(const std::string &str) {
    std::string buffer = "";
    std::vector<std::string> tokens;

    bool braces = false;

    for (int i = 0; i < str.length(); i++) {
      const char &c = str[i];

      if (c == '{' && (i != 0 && str[i - 1] != '\\'))
        braces = true;
      else if (c == '}' && (i != 0 && str[i - 1] != '\\')) {
        braces = false;
      } else if (c == '\\' && (i != 0 && str[i - 1] != '\\'))
        buffer += c;
      else if (c == ' ' && !braces) {
        if (!buffer.empty())
          tokens.push_back(buffer);
        buffer.clear();
      } else if (braces && c == '\n')
        continue;
      else
        buffer += c;
    }
    if (!buffer.empty())
      tokens.push_back(buffer);

    return tokens;
  }
  std::pair<std::string, std::string> getParam(const std::string &token) {
    const int splitIndex = token.find_first_of('=');

    const std::string paramName = token.substr(0, splitIndex);
    const std::string paramValue = token.substr(splitIndex + 1);

    return {paramName, paramValue};
  }
  std::unordered_map<std::string, std::string>
  getParams(const std::string &str) {
    std::vector<std::string> tokens = getTokens(str);
    std::unordered_map<std::string, std::string> initParams;

    char typeChar = tokens[0][0];

    if (typeChar == '#') {
      initParams["type"] = tokens[0];
      tokens.erase(tokens.begin());
    }
    if (typeChar == '$') {
      std::pair<std::string, std::string> param = getParam(tokens[0]);
      initParams["type"] = param.first;
      if (param.second[0] == '@') {
        initParams["value"] =
            Utils::mergeString(Utils::readFile(param.second.substr(1)), '\n');
      } else {
        initParams["value"] = param.second;
      }

      tokens.erase(tokens.begin());
    }
    if (typeChar == '@') {
      initParams["type"] = tokens[0];
      tokens.erase(tokens.begin());
    }
    for (const std::string &token : tokens) {
      std::pair<std::string, std::string> param = getParam(token);
      initParams[param.first] = param.second;
    }

    return initParams;
  }
};

} // namespace UTUI
