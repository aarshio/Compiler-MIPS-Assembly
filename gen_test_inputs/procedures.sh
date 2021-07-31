g++ -g -std=c++14 wlp4gen.cc -o wlp4gen
./wlp4gen < procedures.wlp4i
echo "____________________________"
./wlp4gen < procedures2.wlp4i
echo "____________________________"
./wlp4gen < procedures3.wlp4i
echo "____________________________"
./wlp4gen < procedures4.wlp4i
echo "____________________________"
./wlp4gen < proceduresBefore.wlp4i
echo "____________________________"
./wlp4gen < proceduresCall.wlp4i
echo "____________________________"
./wlp4gen < proceduresControl.wlp4i
echo "____________________________"
./wlp4gen < proceduresRepeat.wlp4i
echo "____________________________"
./wlp4gen < proceduresUndeclared.wlp4i
echo "____________________________"
./wlp4gen < proceduresValgrind.wlp4i