g++ -g -std=c++14 wlp4gen.cc -o wlp4gen
echo "____________________________TYPE ERROR: Argument mistmatch for procedure."
./wlp4gen < type1.wlp4i
echo "____________________________TYPE ERROR: Argument mistmatch for procedure."
./wlp4gen < type2.wlp4i
echo "____________________________TYPE ERROR: Procedure return must be of type int."
./wlp4gen < type3.wlp4i
echo "____________________________TYPE ERROR: Procedure return must be of type int."
./wlp4gen < type4.wlp4i
echo "____________________________TYPE ERROR: Type error term must be int for MULT/DIV/PCT."
./wlp4gen < type5.wlp4i
echo "____________________________TYPE ERROR: Type error term must be int for MULT/DIV/PCT."
./wlp4gen < type6.wlp4i
echo "___________V_________________"
./wlp4gen < type7v.wlp4i
echo "___________V_________________"
./wlp4gen < type8v.wlp4i
echo "____________________________TYPE ERROR: Second argument for WAIN cannot be a pointer."
./wlp4gen < type9.wlp4i
echo "____________________________TYPE ERROR: Too many arguments for procedure."
./wlp4gen < type10.wlp4i
echo "____________________________TYPE ERROR: Too few arguments for procedure."
./wlp4gen < type11.wlp4i
echo "____________________________TYPE ERROR: Too few arguments for procedure."
./wlp4gen < type12.wlp4i
echo "___________V_________________"
./wlp4gen < type13v.wlp4i
echo "____________________________TYPE ERROR: Only a pointer can be deleted."
./wlp4gen < type14.wlp4i
echo "____________________________TYPE ERROR: NEW key word must have a int expr."
./wlp4gen < type15.wlp4i
echo "___________V_________________"
./wlp4gen < type16v.wlp4i
echo "____________________________TYPE ERROR: Cannot get address of a pointer."
./wlp4gen < type17.wlp4i
echo "___________V_________________"
./wlp4gen < type18v.wlp4i
echo "____________________________TYPE ERROR: Type error term must be int for MULT/DIV/PCT."
./wlp4gen < type19.wlp4i
echo "___________V_________________"
./wlp4gen < type20v.wlp4i
echo "____________________________TYPE ERROR: Assignment of NULL to non int*."
./wlp4gen < type21.wlp4i
echo "____________________________TYPE ERROR: Assignment of NUM to non int."
./wlp4gen < type22.wlp4i
echo "____________________________TYPE ERROR: Comparisons need to be of the same type."
./wlp4gen < type23.wlp4i