
VERSION=$(shell python -c "from dsremap_serve import VERSION; print(VERSION)")

all: bundle

bundle:
	rm -rf build
	mkdir -p build/dist/opt/dsremap
	mkdir -p build/dist/etc/init.d

	cd ../pairing && make clean all
	cp -a ../pairing/pairing build/dist/opt/dsremap/pairing
	cd ../proxy && make clean all
	cp -a ../proxy/proxy build/dist/opt/dsremap/proxy

	cp -a dsremap_serve.py build/dist/opt/dsremap/dsremap_serve.py
	cp -a dsremap_serve.sh build/dist/opt/dsremap/dsremap_serve
	cp -a requirements.txt build/dist/opt/dsremap/requirements.txt
	cp -a dsremap-initscript build/dist/etc/init.d/dsremap

	if [ -e pre.sh ]; then cp -a pre.sh build/pre.sh; fi
	if [ -e post.sh ]; then cp -a post.sh build/post.sh; fi

	cd build; tar czf ../dsremap-service-$(VERSION).tgz *
	tarsize=`du -b dsremap-service-$(VERSION).tgz | cut -f1`; sed -e "s/@TARSIZE@/$$tarsize/g" installer.sh.in > dsremap-service-$(VERSION).sh
	cat dsremap-service-$(VERSION).tgz >> dsremap-service-$(VERSION).sh
