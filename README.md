# LabDIP
Digital Image Processing - Lab 1 & 2

Implemented features: scale, rotation, Fourier transformation (brute force), Gaussian noise generation, pepper & salt noise generation, median filter, arithmetic mean filter, Gaussian filter, and Wiener filter. 

OpenCL accelerated features: scale, rotation, Fourier transformation (brute force), Gaussian noise generation & filter. This project doesn't support CUDA yet. 

# Prereq
For AMD GPUs, you should configure [OCL SDK Light](https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/releases) first (don't change the default directory). When NVIDIA GPU applied, you just install CUDA and change the linker directory. 

In order to successfully do brute force DFT, you should modify the default TDR settings. Check this [Zhihu post](https://zhuanlan.zhihu.com/p/38141415).