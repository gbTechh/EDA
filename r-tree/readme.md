g++ -std=c++17 -O2 -DMAXNODES=2 main_generate.cpp RTree.cpp -o gen_MAX2
./gen_MAX2
python3 grafica.py outputs/MAXNODES_2

g++ -std=c++17 -O2 -DMAXNODES=3 main_generate.cpp RTree.cpp -o gen_MAX3
./gen_MAX3
python3 grafica.py outputs/MAXNODES_3
