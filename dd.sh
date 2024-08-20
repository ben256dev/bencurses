printf 'building... '
gcc -g -O0 btree.c -lncursesw -ldl -lpthread -lm -o btree.exe

if [ $? -eq 0 ]; then
	printf 'succeeded!\n'
fi
