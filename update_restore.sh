#!/bin/sh

set -e
set -x

sed -i 's/\$(CRYPTOPP_LIBS)/\/usr\/local\/lib\/libcryptopp.a/g' Makefile.am_client

./switch_build.sh client

autoreconf --install

./configure CC=clang CXX=clang++ CXXFLAGS="-DRESTORE_CLIENT -flto" CFLAGS="-flto" LDFLAGS="-flto" --enable-headless --enable-embedded-cryptopp

make -j4

LANG=en

mkdir -p restore_cd/urbackup/restore
cp urbackupclient/backup_client.db restore_cd/urbackup/
touch restore_cd/urbackup/new.txt

cp urbackupclientbackend restore_cd/urbackuprestoreclient
cp urbackupserver/restore/$LANG/* restore_cd/urbackup/restore/
cp urbackupserver/restore/* restore_cd/urbackup/restore/ || true
chmod +x restore_cd/urbackup/restore/*.sh
#strip restore_cd/urbackuprestoreclient

cd restore_cd
tar -cJf ../restore_cd_2.tar.xz *
cp ../restore_cd_2.tar.xz /var/www/restore_cd_2.tar.xz
