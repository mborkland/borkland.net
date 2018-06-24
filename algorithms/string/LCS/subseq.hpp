#ifndef SUBSEQ_H
#define SUBSEQ_H

#include <string>
#include <vector>

void subseq_util(std::vector<std::string> &subsequences, std::string &origStr, std::string subseqStr, int i, int n)
{
  if (i == n)
  {
    subsequences.push_back(subseqStr);
    subsequences.push_back(subseqStr + origStr[i]);
    return;
  }

  subseq_util(subsequences, origStr, subseqStr, i + 1, n);
  subseq_util(subsequences, origStr, subseqStr + origStr[i], i + 1, n);
}

std::vector<std::string> subseq(std::string str)
{
  std::vector<std::string> subsequences;
  int n = str.size() - 1;
  subseq_util(subsequences, str, "", 0, n);
  return subsequences;
}

#endif
