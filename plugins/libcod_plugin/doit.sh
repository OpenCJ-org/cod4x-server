NAME='libcod'
no_warning_spam="-Wall -Wno-pragmas -Wno-pragma -Wno-write-strings -Wno-pointer-arith -Wno-format -Wno-parentheses -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-return-type -Wno-sign-compare -Wno-unused-local-typedefs"

#Compiling: debugging
echo `gcc $no_warning_spam -g -m32 -Wall -O1 -s -fvisibility=hidden -mtune=core2 -I"/usr/include/mysql" -c *.cpp`

#Compiling: release
#echo `g++ $no_warning_spam -m32 -Wall -O1 -s -fvisibility=hidden -mtune=core2 -c *.cpp`

#Linking
echo `gcc $no_warning_spam -m32 -shared -fvisibility=hidden -o libcod4_1_8x.so *.o -L/usr/lib/i386-linux-gnu/ -lmysqlclient`

#Cleaning up
echo `rm *.o`

chmod 700 libcod4_1_8x.so
