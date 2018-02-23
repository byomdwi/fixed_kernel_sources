PATH=${PATH}:/path/to/toolchain/bin                                          #for adding the toolchain to path variable
export ARCH=arm64                                                            #for adding architcture
make manning_defconfig ARCH=arm64 CROSS_COMPILE=aarch64-linux-android-       #for making .config file using default kernel configuration file of manning
#make menuconfig ARCH=arm64                                                  #uncomment this line if you want to modify the configuration file 
make -j5 ARCH=arm64 CROSS_COMPILE=aarch64-linux-android-                     #for finally building up the kernel
