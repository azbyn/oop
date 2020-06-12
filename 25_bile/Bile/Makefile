DIR = .

main:
	msbuild

buildAndRun: main run


clean:
	msbuild -t:Clean

run:
	mono ./${DIR}/bin/Debug/*.exe
