printf 'building... '
gcc bencurses.c -lncursesw -ldl -lpthread -lm -o bencurses.exe

if [ $? -eq 0 ]; then
	printf 'succeeded!\n'
fi
