CROSSPREFIX = i686-w64-mingw32-
#CROSSPREFIX = 
CX := $(CROSSPREFIX)g++
CXX := $(CROSSPREFIX)gcc
CFLAGS := -c -O3 -Wall -std=c++11 -Wno-unknown-pragmas
ifneq ($(CXX),gcc)
	CFLAGS += -lstdc++ -I/usr/i686-w64-mingw32/include -L/usr/i686-w64-mingw32/lib -lws2_32 -liphlpapi
else
	CFLAGS += -ldl
endif
LDFLAGS := -O3
ifneq ($(CXX),gcc)
	LDFLAGS += -lstdc++ -I/usr/i686-w64-mingw32/include -L/usr/i686-w64-mingw32/lib -lws2_32 -liphlpapi
else
	LDFLAGS += -ldl
endif
OBJS_DIR := objs
RELEASE_DIR := release
EXECUTABLE = chatroom

SOURCES := main.cpp websocket.cpp sha1.cpp base64.cpp
OBJECTS := $(SOURCES:%.c=objs/%.o)

ifneq ($(CXX),gcc)
	EXECUTABLE := $(EXECUTABLE).exe
endif

.PHONY: all

all: $(OBJS_DIR) $(RELEASE_DIR) $(SOURCES) $(RELEASE_DIR)/$(EXECUTABLE)

$(RELEASE_DIR):
	@mkdir -p $(RELEASE_DIR)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

$(RELEASE_DIR)/$(EXECUTABLE): $(OBJECTS)
ifneq ($(CXX),gcc)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@ -lws2_32
	@cp -rf /usr/lib/gcc/i686-w64-mingw32/7.3-win32/libgcc_s_sjlj-1.dll $(RELEASE_DIR)/
	@cp -rf /usr/lib/gcc/i686-w64-mingw32/7.3-win32/libstdc++-6.dll $(RELEASE_DIR)/
else
	$(CX) $(OBJECTS) $(LDFLAGS) -o $@
endif

$(OBJS_DIR)/%.o : %.cpp
	$(CX) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJS_DIR)/*.o $(RELEASE_DIR)/*
