if [ ! -d "/mnt/nand/nfs" ]; then
  mkdir /mnt/nand/nfs
fi

mount -t nfs 192.168.1.249:/home/pamsimochen/arable_land/acs2521/dev/ipnc_rdk/target/filesys /mnt/nand/nfs/ -o nolock,proto=tcp
