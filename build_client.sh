#!/bin/sh

set -e

#git reset --hard
#python build/replace_versions.py

if ! test -e build_client_ok
then
	./switch_build.sh client
	autoreconf || true
	automake --add-missing || true
	libtoolize || true
	autoreconf --install
	./configure --enable-embedded-cryptopp
	touch build_client_ok
fi
./switch_build.sh client
make
make dist
