printf 'building... '
gcc bencurses.c -lncursesw -ldl -lpthread -lm -o bencurses.exe
./bencurses.exe
rm *.exe
if [ $? -eq 0 ]; then
	printf 'succeeded!\n'
fi
