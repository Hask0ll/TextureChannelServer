<!-- GETTING STARTED -->
## Getting Started

### Installation

1. Install [Cmake](https://cmake.org/download/), and [vcpkg](https://vcpkg.io/en/getting-started)
2. Clone the repo
   ```sh
   git clone https://github.com/Hask0ll/TextureChannelServer.git
   ```
3. Create the build directory
      ```sh
      mkdir build
      ```
4. From the build directory generate the project with cmake
    ```sh
     cmake -DCMAKE_TOOLCHAIN_FILE=${Path_To_vcpkg}/vcpkg/scripts/buildsystems/vcpkg.cmake ..
     ```
5. Open the .sln file in the build directory
