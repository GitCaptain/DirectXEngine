# DirectXEngine

Small graphics framework for windows written in c++ using directx11. 

### Prerequisites

This projects depends on a few third party libraries (e.g. ImGui, AssImp, DirectXTK), all of them are added as submodules to the project. 
When you clone `DirectXEngine`, you should also clone its submodules  
either while cloning repo:
```
git clone --recurse-submodules https://github.com/GitCaptain/DirectXEngine.git
```
or after cloning:
```
cd DirectXEngine
git submodule update --init --recursive
```
