HOME_DIR='/home/waso'
LINUX_DIR="$HOME_DIR/CS-596-RTOS/CS596-RT-2024Fall-Group05"
mount /dev/sd*1 $HOME_DIR/mnt/boot/

if [ $? -ne 0 ]; then 
  echo "USB 1 isn't mounted"
  return -1
fi

mount /dev/sd*2 $HOME_DIR/mnt/root/ 

if [ $? -ne 0 ]; then 
  echo "USB 2 isn't mounted"
  return -1
fi
env PATH=$PATH make -j $(nproc) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=$HOME_DIR/mnt/root modules_install | tail
export KERNEL=kernel7
cp $HOME_DIR/mnt/boot/$KERNEL.img $HOME_DIR/mnt/boot/$KERNEL-backup.img

cp $LINUX_DIR/arch/arm/boot/zImage $HOME_DIR/mnt/boot/$KERNEL.img

cp $LINUX_DIR/arch/arm/boot/dts/*.dtb $HOME_DIR/mnt/boot

umount $HOME_DIR/mnt/boot

echo "Move Relevant Files to ~/mnt/root and then unmount"
