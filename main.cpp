#include <iostream>
#include <sstream>
#include "rpc/server.h"
#include "seal/seal.h"
#include "hash.hpp"

using namespace seal;
using namespace std;

unordered_map<string, size_t> password;
unordered_map<string, unordered_map<string, Ciphertext>> stores;
std::shared_ptr<SEALContext> context;

void init() {
  EncryptionParameters parms(scheme_type::bfv);
  size_t poly_modulus_degree = 4096;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
  parms.set_plain_modulus(1024);
  context = std::make_shared<SEALContext>(parms);
  password["Gamestop"] = Hash("111111", 6, 0);
  password["Walmart"] = Hash("222222", 6, 0);
  password["BestBuy"] = Hash("333333", 6, 0);
  password["Target"] = Hash("444444", 6, 0);
}

stringstream to_stream(vector<unsigned char> const& v)
{
  std::stringstream ss;
  ss.write((char const*)v.data(), std::streamsize(v.size()));
  return ss;
}

vector<unsigned char> to_vector(stringstream& ss)
{
  // discover size of data in stream
  ss.seekg(0, std::ios::beg);
  auto bof = ss.tellg();
  ss.seekg(0, std::ios::end);
  auto stream_size = std::size_t(ss.tellg() - bof);
  ss.seekg(0, std::ios::beg);

  // make your vector long enough
  std::vector<unsigned char> v(stream_size);

  // read directly in
  ss.read((char*)v.data(), std::streamsize(v.size()));

  return v;
}

pair<vector<unsigned char>,int> Avg(const string& user, const string& product) {
  cout << "[" << user << "] Avg(" << product << ")" << endl;
  seal::Ciphertext cipher_result(*context);
  int numEntries = 0;
  Evaluator evaluator(*context);
  for (auto& [store,salesData] : stores) {
    auto it = salesData.find(product);
    if (it != salesData.end()) {
      evaluator.add_inplace(cipher_result, it->second);
      numEntries++;
    }
  }
  stringstream ss;
  cipher_result.save(ss);
  return {to_vector(ss), numEntries};
}

void Set(const string& store, const string& product, const vector<unsigned char>& data) {
  cout << "[" << store << "] Set(" << product << ")" << endl;
  Ciphertext ciphertext;
  stringstream ss = to_stream(data);
  ciphertext.load(*context, ss);
  stores[store][product] = ciphertext;
}

bool Login(const string& username, size_t hashPassword) {
  cout << "User <" << username << "> is trying to login, ";
  bool succ = password[username] == hashPassword;
  if (succ) {
    cout << "Granted access" << endl;
  } else {
    cout << "Denied access" << endl;
  }
  return succ;
}

int main(int argc, char *argv[]) {
  init();
  rpc::server srv(8081);

  srv.bind("Login", &Login);
  srv.bind("Avg", &Avg);
  srv.bind("Set", &Set);

  cout << "Start!" << endl;

  srv.run();

  return 0;
}