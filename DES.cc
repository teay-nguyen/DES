#include <bitset>
#include <cassert>
#include <cctype>
#include <vector>
#include <iostream>
#include <cstdint>
#include <ostream>
#include <string>
#include <fstream>
#include <utility>
#include <unordered_map>

using std::cout;

constexpr uint8_t N = 4;

constexpr static const uint8_t PC1[56] = {
  57, 49,  41, 33,  25,  17,  9,
   1, 58,  50, 42,  34,  26, 18,
  10,  2,  59, 51,  43,  35, 27,
  19, 11,   3, 60,  52,  44, 36,
  63, 55,  47, 39,  31,  23, 15,
   7, 62,  54, 46,  38,  30, 22,
  14,  6,  61, 53,  45,  37, 29,
  21, 13,   5, 28,  20,  12,  4
};

constexpr static const uint8_t PC2[48] = {
  14,    17,   11,    24,     1,    5,
   3,    28,   15,     6,    21,   10,
  23,    19,   12,     4,    26,    8,
  16,     7,   27,    20,    13,    2,
  41,    52,   31,    37,    47,   55,
  30,    40,   51,    45,    33,   48,
  44,    49,   39,    56,    34,   53,
  46,    42,   50,    36,    29,   32
};

constexpr static const uint8_t Shift[16] = {
  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

constexpr static const uint8_t IP[64] = {
  58,   50,  42,   34,   26,  18,   10,   2,
  60,   52,  44,   36,   28,  20,   12,   4,
  62,   54,  46,   38,   30,  22,   14,   6,
  64,   56,  48,   40,   32,  24,   16,   8,
  57,   49,  41,   33,   25,  17,    9,   1,
  59,   51,  43,   35,   27,  19,   11,   3,
  61,   53,  45,   37,   29,  21,   13,   5,
  63,   55,  47,   39,   31,  23,   15,   7
};

constexpr static const uint8_t IP_R[64] = {
  40,    8,  48,   16,   56,  24,   64,  32,
  39,    7,  47,   15,   55,  23,   63,  31,
  38,    6,  46,   14,   54,  22,   62,  30,
  37,    5,  45,   13,   53,  21,   61,  29,
  36,    4,  44,   12,   52,  20,   60,  28,
  35,    3,  43,   11,   51,  19,   59,  27,
  34,    2,  42,   10,   50,  18,   58,  26,
  33,    1,  41,    9,   49,  17,   57,  25
};

constexpr static const uint8_t E[48] = {
   32,    1,   2,    3,    4,   5,
    4,    5,   6,    7,    8,   9,
    8,    9,  10,   11,   12,  13,
   12,   13,  14,   15,   16,  17,
   16,   17,  18,   19,   20,  21,
   20,   21,  22,   23,   24,  25,
   24,   25,  26,   27,   28,  29,
   28,   29,  30,   31,   32,   1
};

constexpr static const uint8_t S[8][4][16] = {
  {
    {14, 4, 13, 1,  2,15, 11, 8,  3,10,  6,12,  5, 9,  0, 7},
    { 0,15,  7, 4, 14, 2, 13, 1, 10, 6, 12,11,  9, 5,  3, 8},
    { 4, 1, 14, 8, 13, 6,  2,11, 15,12,  9, 7,  3,10,  5, 0},
    {15,12,  8, 2,  4, 9,  1, 7,  5,11,  3,14, 10, 0,  6,13}
  },
  {
    {15, 1,  8,14,  6,11,  3, 4,  9, 7,  2,13, 12, 0,  5,10},
    { 3,13,  4, 7, 15, 2,  8,14, 12, 0,  1,10,  6, 9, 11, 5},
    { 0,14,  7,11, 10, 4, 13, 1,  5, 8, 12, 6,  9, 3,  2,15},
    {13, 8, 10, 1,  3,15,  4, 2, 11, 6,  7,12,  0, 5, 14, 9},
  },
  {
    {10, 0,  9,14,  6, 3, 15, 5,  1,13, 12, 7, 11, 4,  2, 8},
    {13, 7,  0, 9,  3, 4,  6,10,  2, 8,  5,14, 12,11, 15, 1},
    {13, 6,  4, 9,  8,15,  3, 0, 11, 1,  2,12,  5,10, 14, 7},
    { 1,10, 13, 0,  6, 9,  8, 7,  4,15, 14, 3, 11, 5,  2,12}
  },
  {
    { 7,13, 14, 3,  0, 6,  9,10,  1, 2,  8, 5, 11,12,  4,15},
    {13, 8, 11, 5,  6,15,  0, 3,  4, 7,  2,12,  1,10, 14, 9},
    {10, 6,  9, 0, 12,11,  7,13, 15, 1,  3,14,  5, 2,  8, 4},
    { 3,15,  0, 6, 10, 1, 13, 8,  9, 4,  5,11, 12, 7,  2,14}
  },
  {
    { 2,12,  4, 1,  7,10, 11, 6,  8, 5,  3,15, 13, 0, 14, 9},
    {14,11,  2,12,  4, 7, 13, 1,  5, 0, 15,10,  3, 9,  8, 6},
    { 4, 2,  1,11, 10,13,  7, 8, 15, 9, 12, 5,  6, 3,  0,14},
    {11, 8, 12, 7,  1,14,  2,13,  6,15,  0, 9, 10, 4,  5, 3}
  },
  {
    {12, 1, 10,15,  9, 2,  6, 8,  0,13,  3, 4, 14, 7,  5,11},
    {10,15,  4, 2,  7,12,  9, 5,  6, 1, 13,14,  0,11,  3, 8},
    { 9,14, 15, 5,  2, 8, 12, 3,  7, 0,  4,10,  1,13, 11, 6},
    { 4, 3,  2,12,  9, 5, 15,10, 11,14,  1, 7,  6, 0,  8,13}
  },
  {
    { 4,11,  2,14, 15, 0,  8,13,  3,12,  9, 7,  5,10,  6, 1},
    {13, 0, 11, 7,  4, 9,  1,10, 14, 3,  5,12,  2,15,  8, 6},
    { 1, 4, 11,13, 12, 3,  7,14, 10,15,  6, 8,  0, 5,  9, 2},
    { 6,11, 13, 8,  1, 4, 10, 7,  9, 5,  0,15, 14, 2,  3,12}
  },
  {
    {13, 2,  8, 4,  6,15, 11, 1, 10, 9,  3,14,  5, 0, 12, 7},
    { 1,15, 13, 8, 10, 3,  7, 4, 12, 5,  6,11,  0,14,  9, 2},
    { 7,11,  4, 1,  9,12, 14, 2,  0, 6, 10,13, 15, 3,  5, 8},
    { 2, 1, 14, 7,  4,10,  8,13, 15,12,  9, 0,  3, 5,  6,11}
  },
};

constexpr static const uint8_t P[32] = {
   16,  7, 20, 21,
   29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2,  8, 24, 14,
   32, 27,  3,  9,
   19, 13, 30,  6,
   22, 11,  4, 25
}; 


std::unordered_map<std::string, uint16_t> hex4toIntMap = {
  {"0000",  0}, {"0001",  1}, {"0010",  2}, {"0011",  3},
  {"0100",  4}, {"0101",  5}, {"0110",  6}, {"0111",  7},
  {"1000",  8}, {"1001",  9}, {"1010", 10}, {"1011", 11},
  {"1100", 12}, {"1101", 13}, {"1110", 14}, {"1111", 15}
};

std::unordered_map<std::string, uint16_t> hex2toIntMap = {
  {"00",  0}, {"01",  1}, {"10",  2}, {"11",  3}
};

std::unordered_map<char, std::string> hexToBinMap = {
  {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
  {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
  {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
  {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"}
};

std::unordered_map<std::string, char> bintoHexMap = {
  {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'},
  {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'},
  {"1000", '8'}, {"1001", '9'}, {"1010", 'A'}, {"1011", 'B'},
  {"1100", 'C'}, {"1101", 'D'}, {"1110", 'E'}, {"1111", 'F'}
};

std::pair<std::string, std::string> ConvStringToBin(std::string s) {
  std::string t0, t1;
  for(size_t i=0; i<s.size(); ++i) {
    std::bitset<N> b(s.c_str()[i]);
    t0 += b.to_string() + ' ';
    t1 += b.to_string();
  }

  t1 = std::string(64-t1.length(), '0') + t1;

  return std::pair<std::string, std::string>(t0, t1);
}

std::string ConvHexCharToBin(char s) {
  s = toupper(s);
  return hexToBinMap[s];
}

std::vector<std::string> DESGenSubKeys(const std::string& t) {
  std::vector<std::string> v;
  v.reserve(16);

  // initial key permutation
  std::string res="";
  for(int i=0; i<56; ++i)
    res += t[PC1[i]-1];

  std::string c0 = res.substr(0,28);
  std::string d0 = res.substr(28);

  for(int i=0; i<16; ++i) {
    c0 = c0.substr(Shift[i]) + c0.substr(0,Shift[i]);
    d0 = d0.substr(Shift[i]) + d0.substr(0,Shift[i]);

    res.clear();
    std::string c = c0+d0;
    for(int j=0; j<48; ++j)
      res += c[PC2[j]-1];

    v.push_back(res);
  }

  return v;
}

std::string DESEncrypt(const std::string& t, const std::vector<std::string> K) {
  assert(t.length() == 64);

  std::string res="";
  for(int i=0; i<64; ++i)
    res += t[IP[i]-1];

  std::string l0 = res.substr(0,32);
  std::string r0 = res.substr(32);

  for(int _t=0; _t<16; ++_t) {
    std::string tmp_r = r0;

    res.clear();
    for(int i=0; i<48; ++i)
      res += r0[E[i]-1];

    std::string f;
    for(int i=0; i<48; ++i) {
      if(K[_t][i]==res[i]) f += '0';
      else f += '1';
    }

    std::vector<std::string> Bl;
    for(size_t i=0; i<48; i+=6)
      Bl.push_back(f.substr(i,6));

    std::string tmp;
    for(size_t i=0; i<Bl.size(); ++i) {
      char iB = Bl[i][0];
      char jB = Bl[i][Bl[i].size()-1];

      std::string c0 = std::string(1,iB)+std::string(1,jB);
      std::string c1 = Bl[i].substr(1,4);

      int n0 = hex2toIntMap[c0], n1 = hex4toIntMap[c1];
      std::string s = std::bitset<4>(S[i][n0][n1]).to_string();
      tmp += s;
    }

    res.clear();
    for(int i=0; i<32; ++i)
      res += tmp[P[i]-1];

    r0.clear();
    for(size_t i=0; i<res.size(); ++i) {
      if (res[i]==l0[i]) r0 += '0';
      else r0 += '1';
    }

    l0 = tmp_r;
  }

  res.clear();
  std::string fin = r0+l0;
  for(int i=0; i<64; ++i)
    res += fin[IP_R[i]-1];

  std::string res_hex;
  for(int i=0; i<64; i+=4)
    res_hex += bintoHexMap[res.substr(i,4)];

  return res_hex;
}

std::string DESDecrypt(const std::string& t, const std::vector<std::string> K) {
  assert(t.length() == 64);

  std::string res="";
  for(int i=0; i<64; ++i)
    res += t[IP[i]-1];

  std::string l16 = res.substr(0,32);
  std::string r16 = res.substr(32);

  for(int _te=0; _te<16; ++_te) {
    std::string tmp_r = r16;

    res.clear();
    for(int i=0; i<48; ++i)
      res += r16[E[i]-1];

    std::string f;
    for(int i=0; i<48; ++i) {
      if(K[15-_te][i]==res[i]) f += '0';
      else f += '1';
    }

    std::vector<std::string> Bl;
    for(size_t i=0; i<48; i+=6)
      Bl.push_back(f.substr(i,6));

    std::string tmp;
    for(size_t i=0; i<Bl.size(); ++i) {
      char iB = Bl[i][0];
      char jB = Bl[i][Bl[i].size()-1];

      std::string c0 = std::string(1,iB)+std::string(1,jB);
      std::string c1 = Bl[i].substr(1,4);

      int n0 = hex2toIntMap[c0], n1 = hex4toIntMap[c1];
      std::string s = std::bitset<4>(S[i][n0][n1]).to_string();
      tmp += s;
    }

    res.clear();
    for(int i=0; i<32; ++i)
      res += tmp[P[i]-1];

    r16.clear();
    for(size_t i=0; i<res.size(); ++i) {
      if (res[i]==l16[i]) r16 += '0';
      else r16 += '1';
    }

    l16 = tmp_r;
  }

  res.clear();
  std::string fin = r16+l16;
  for(int i=0; i<64; ++i)
    res += fin[IP_R[i]-1];

  std::string res_hex;
  for(int i=0; i<64; i+=4)
    res_hex += bintoHexMap[res.substr(i,4)];

  return res_hex;
}

int main() {
  std::string plainM = ConvStringToBin("kill you").second;

  std::string M = "0123456789ABCDEF";
  std::string K = "133457799BBCDFF1";
  std::string kbin, mbin, enc_bin;

  for(char& c : K)
    kbin += ConvHexCharToBin(c);

  for(char& c : M)
    mbin += ConvHexCharToBin(c);

  std::vector<std::string> vk = DESGenSubKeys(kbin);

  std::string enc = DESEncrypt(mbin, vk);

  for(char& c : enc)
    enc_bin += ConvHexCharToBin(c);

  std::string dec = DESDecrypt(enc_bin, vk);
  cout << M << std::endl << enc << std::endl << dec << std::endl;

  return 0;
}
