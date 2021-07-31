g++ -g -std=c++14 wlp4gen.cc -o wlp4gen
./wlp4gen < sample.wlp4i
echo "____________________________"
./wlp4gen < sample2.wlp4i
echo "____________________________"
./wlp4gen < sample3.wlp4i
echo "____________________________"
./wlp4gen < dup.wlp4i
echo "____________________________"
./wlp4gen < failed.wlp4i