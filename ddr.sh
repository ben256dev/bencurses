printf 'building... '
gcc btree.c -g -O0 -lncursesw -ldl -lpthread -lm -o btree.exe
gdb btree.exe
rm *.exe
if [ $? -eq 0 ]; then
	printf 'succeeded!\n'
fi
