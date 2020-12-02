# Homomorphism-Encryption-KV-Store-Server


## Install
Make sure you have the latest version of <strong>gcc</strong> and <strong>cmake</strong> before proceeding

### Make sure you clone all the submodules (SEAL and rpclib)
```
git clone --recurse-submodules https://github.com/laiKwunSing/Homomorphism-Encryption-KV-Store-Server.git
```

### Comile and build the submodules (Skip this if you have done this in the client repo)
```
cd SEAL && cmake . && make -j && sudo make install
```

```
cd rpclib && mkdir build && cd build 
cmake .. && make -j && sudo make install
```

### Compile and build itself
```
mkdir build && cd build
cmake .. && make -j
```

## Execution
```
cd build
./HE_kv_store_server
```

