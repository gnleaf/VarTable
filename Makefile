all:
	g++ -std=c++11 -g -fno-omit-frame-pointer -O3 -o var_table main.cpp
	g++ -std=c++11 -g -fno-omit-frame-pointer -o var_table_dbg main.cpp

clean:
	rm -f var_table
	rm -f var_table_dbg
