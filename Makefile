CC=arm-hisiv200-linux-gcc
#CC=gcc
TARGET:=receive process audiocut reco weight
LIBS:=-lsqlite3 -lpthread
DLIBS:=-lpthread -shared -fPIC

all:$(TARGET)
	$(CC) main.c -L . -lreceive -lprocess -laudiocut -lreco -lmyHvite -lweight -o recomodel

receive:
	$(CC) $(DLIBS) -o libreceive.so receive.c
process:
	$(CC) $(DLIBS) -o libprocess.so process.c
audiocut:
	$(CC) $(DLIBS) -o libaudiocut.so audiocut.c
reco:
	$(CC) $(DLIBS) -L . -lmyHvite -o libreco.so reco.c
weight:
	$(CC) $(DLIBS) -lsqlite3 -o libweight.so weight.c
clean:
	rm -rf *.bak $(OBJS) $(TARGET)
