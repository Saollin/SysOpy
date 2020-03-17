CC = gcc
FLAGS = -Wall
LIB_NAME = cw01
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE = main.c
RAPORT_FILE = results3a.txt

static:
	make clear_all
	$(CC) $(FLAGS) main.c -o test -I$(LIB_PATH) -L$(LIB_PATH) -l$(LIB_NAME)
	make clear

shared:
	make clear_all
	$(CC) $(FLAGS) main.c -o test -I$(LIB_PATH) -L$(LIB_PATH) -l$(LIB_NAME) 
	make clear

dll:
	make clear_all
	$(CC) $(FLAGS) main.c -o test -I$(LIB_PATH) -ldl -D DLL
	make clear

ofile:
	$(CC) $(FLAGS) -c lib$(LIB_NAME).c -o lib$(LIB_NAME).o 

tests:
	#similiar files - small count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 4 compare_pairs "fs/s1.txt:fs/s2.txt fs/s1.txt:fs/s3.txt fs/s1.txt:fs/s4.txt fs/s1.txt:fs/s5.txt" end "Small count of similiar files - saving" start remove_block 0  remove_block 1 end "deleting 2 blocks" start compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 end "deleting and removing 3 times" >> $(RAPORT_FILE)

	#medium count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 14 compare_pairs "fs/s1.txt:fs/s2.txt fs/s1.txt:fs/s3.txt fs/s1.txt:fs/s4.txt fs/s1.txt:fs/s5.txt fs/s1.txt:fs/s6.txt fs/s1.txt:fs/s7.txt fs/s1.txt:fs/s8.txt fs/s1.txt:fs/s9.txt fs/s1.txt:fs/s10.txt fs/s1.txt:fs/s11.txt fs/s1.txt:fs/s12.txt fs/s1.txt:fs/s13.txt fs/s1.txt:fs/s14.txt fs/s1.txt:fs/s15.txt" end "Medium count of similiar files - saving" start remove_block 0 remove_block 1 remove_block 2 remove_block 3 end "deleting 4 blocks" start compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 end "deleting and removing 3 times" >> $(RAPORT_FILE)

	#big count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 29 compare_pairs "fs/s1.txt:fs/s2.txt fs/s1.txt:fs/s3.txt fs/s1.txt:fs/s4.txt fs/s1.txt:fs/s5.txt fs/s1.txt:fs/s6.txt fs/s1.txt:fs/s7.txt fs/s1.txt:fs/s8.txt fs/s1.txt:fs/s9.txt fs/s1.txt:fs/s10.txt fs/s1.txt:fs/s11.txt fs/s1.txt:fs/s12.txt fs/s1.txt:fs/s13.txt fs/s1.txt:fs/s14.txt fs/s1.txt:fs/s15.txt fs/s1.txt:fs/s16.txt fs/s1.txt:fs/s17.txt fs/s1.txt:fs/s18.txt fs/s1.txt:fs/s19.txt fs/s1.txt:fs/s20.txt fs/s1.txt:fs/s21.txt fs/s1.txt:fs/s22.txt fs/s1.txt:fs/s23.txt fs/s1.txt:fs/s24.txt fs/s1.txt:fs/s25.txt fs/s1.txt:fs/s26.txt fs/s1.txt:fs/s27.txt fs/s1.txt:fs/s28.txt fs/s1.txt:fs/s29.txt fs/s1.txt:fs/s30.txt" end "Big count of similiar files - saving" start remove_block 0 remove_block 1 remove_block 2 remove_block 3 end "deleting 4 blocks" start compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 compare_pairs "fs/s1.txt:fs/s2.txt" remove_block 0 end "deleting and removing 3 times" >> $(RAPORT_FILE)


	#half-similiar files - small count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 4 compare_pairs "fs/m1.txt:fs/m2.txt fs/m1.txt:fs/m3.txt fs/m1.txt:fs/m4.txt fs/m1.txt:fs/m5.txt" end "Small count of half-similiar files - saving" start remove_block 0  remove_block 1 end "deleting 2 blocks" start compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 end "deleting and removing 4 times" >> $(RAPORT_FILE)

	#medium count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 14 compare_pairs "fs/m1.txt:fs/m2.txt fs/m1.txt:fs/m3.txt fs/m1.txt:fs/m4.txt fs/m1.txt:fs/m5.txt fs/m1.txt:fs/m6.txt fs/m1.txt:fs/m7.txt fs/m1.txt:fs/m8.txt fs/m1.txt:fs/m9.txt fs/m1.txt:fs/m10.txt fs/m1.txt:fs/m11.txt fs/m1.txt:fs/m12.txt fs/m1.txt:fs/m13.txt fs/m1.txt:fs/m14.txt fs/m1.txt:fs/m15.txt " end "Medium count of half-similiar files - saving" start remove_block 0  remove_block 1 remove_block 2 remove_block 5 end "deleting 4 blocks" start compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 end "deleting and removing 4 times" >> $(RAPORT_FILE)

	#big count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 29 compare_pairs "fs/m1.txt:fs/m2.txt fs/m1.txt:fs/m3.txt fs/m1.txt:fs/m4.txt fs/m1.txt:fs/m5.txt fs/m1.txt:fs/m6.txt fs/m1.txt:fs/m7.txt fs/m1.txt:fs/m8.txt fs/m1.txt:fs/m9.txt fs/m1.txt:fs/m10.txt fs/m1.txt:fs/m11.txt fs/m1.txt:fs/m12.txt fs/m1.txt:fs/m13.txt fs/m1.txt:fs/m14.txt fs/m1.txt:fs/m15.txt fs/m1.txt:fs/m16.txt fs/m1.txt:fs/m17.txt fs/m1.txt:fs/m18.txt fs/m1.txt:fs/m19.txt fs/m1.txt:fs/m20.txt fs/m1.txt:fs/m21.txt fs/m1.txt:fs/m22.txt fs/m1.txt:fs/m23.txt fs/m1.txt:fs/m24.txt fs/m1.txt:fs/m25.txt fs/m1.txt:fs/m26.txt fs/m1.txt:fs/m27.txt fs/m1.txt:fs/m28.txt fs/m1.txt:fs/m29.txt fs/m1.txt:fs/m30.txt " end "Big count of half-similiar files - saving" start remove_block 0  remove_block 1 remove_block 2 remove_block 5 remove_block 6 end "deleting 5 blocks" start compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 compare_pairs "fs/m1.txt:fs/m3.txt" remove_block 0 end "deleting and removing 4 times" >> $(RAPORT_FILE)


	#non-similiar files - small count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 4 compare_pairs "fs/b1.txt:fs/b2.txt fs/b1.txt:fs/b3.txt fs/b1.txt:fs/b4.txt fs/b1.txt:fs/b5.txt" end "Small count of non-similiar files - saving" start remove_block 0  remove_block 1 end "deleting 2 blocks" start compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 end "deleting and removing 4 times" >> $(RAPORT_FILE)

	#medium count 
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 14 compare_pairs "fs/b1.txt:fs/b2.txt fs/b1.txt:fs/b3.txt fs/b1.txt:fs/b4.txt fs/b1.txt:fs/b5.txt fs/b1.txt:fs/b6.txt fs/b1.txt:fs/b7.txt fs/b1.txt:fs/b8.txt fs/b1.txt:fs/b9.txt fs/b1.txt:fs/b10.txt fs/b1.txt:fs/b11.txt fs/b1.txt:fs/b12.txt fs/b1.txt:fs/b13.txt fs/b1.txt:fs/b14.txt fs/b1.txt:fs/b15.txt" end "Medium count of non-similiar files - saving" start remove_block 0  remove_block 1 end "deleting 2 blocks" start compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 end "deleting and removing 4 times" >> $(RAPORT_FILE)

	#big count
	LD_LIBRARY_PATH=$(LIB_PATH) ./test start create_table 29 compare_pairs "fs/b1.txt:fs/b2.txt fs/b1.txt:fs/b3.txt fs/b1.txt:fs/b4.txt fs/b1.txt:fs/b5.txt fs/b1.txt:fs/b6.txt fs/b1.txt:fs/b7.txt fs/b1.txt:fs/b8.txt fs/b1.txt:fs/b9.txt fs/b1.txt:fs/b10.txt fs/b1.txt:fs/b11.txt fs/b1.txt:fs/b12.txt fs/b1.txt:fs/b13.txt fs/b1.txt:fs/b14.txt fs/b1.txt:fs/b15.txt fs/b1.txt:fs/b16.txt fs/b1.txt:fs/b17.txt fs/b1.txt:fs/b18.txt fs/b1.txt:fs/b19.txt fs/b1.txt:fs/b20.txt fs/b1.txt:fs/b21.txt fs/b1.txt:fs/b22.txt fs/b1.txt:fs/b23.txt fs/b1.txt:fs/b24.txt fs/b1.txt:fs/b25.txt fs/b1.txt:fs/b26.txt fs/b1.txt:fs/b27.txt fs/b1.txt:fs/b28.txt fs/b1.txt:fs/b29.txt fs/b1.txt:fs/b30.txt" end "Big count of non-similiar files - saving" start remove_block 0  remove_block 1 end "deleting 2 blocks" start compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 compare_pairs "fs/b1.txt:fs/b4.txt" remove_block 0 end "deleting and removing 4 times" >> $(RAPORT_FILE)

all:
	echo "static: " > $(RAPORT_FILE)
	make static
	make tests
	echo "shared: " >> $(RAPORT_FILE)
	make shared
	make tests
	echo "dll: " >> $(RAPORT_FILE)
	make dll
	make tests

clear:
	rm -f *.o

clear_all:
	rm -f *.o *.a *.so test result.txt