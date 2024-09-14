# for 32 bits
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig

KERNEL=kernel7
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs -j 8


# for 64 bits


#KERNEL=kernel8
#make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig
#make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
#make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs -j 8

